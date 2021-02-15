#include "engine/datatypes/game_tileset.h"
#define _XOPEN_SOURCE 500			/* Required under GLIBC for nftw() */
//#define _XOPEN_SOURCE_EXTENDED 1	/* Same */
#include <unistd.h>
#include "include/imgui/imgui.h"
#include "include/imgui/imgui_impl_glfw.h"
#include "include/imgui/imgui_impl_opengl3.h"
#include "include/GL/gl3w.h"
#include <GLFW/glfw3.h>
#include "helpers/string_helper"
#include <iostream>
#include <map>
#include <stack>
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <dirent.h>
#include <signal.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <nlohmann/json.hpp>
#include "include/alphanum.hpp"
#include "include/prettyprint.hpp"
#include "loader/assets.h"
#include "cfg.h"
//#include "boilerplate.h"
#include "debug.h"
#include "engine/engine.h"
#include "engine/text/fonts.h"
#include "engine/datatypes/game_map.h"
#include "helpers/generators.h"
#include "engine/datatypes/game_sprite.h"
#include "helpers/helpers.h"
#include "engine/shader.h"
#include "engine/text/text_engine.h"
#include "engine/vbo.h"
#include <hb.h>
#include <hb-ft.h>
#include <cairo.h>
#include <cairo-ft.h>
#include "logging.h"
#include <libgen.h>
#include <ftw.h>
#include <dirent.h>
#include "helpers/util.h"
#include "include/imgui/imgui.h"
#include "loader/loader.h"
#include "engine/atlas/texture_atlas.h"

#define FONT "./Sevastopol-Interface.ttf"
#define FONT_SIZE 36
#define MARGIN (FONT_SIZE * .5)
using namespace cfg;
using json = nlohmann::json;
using shader_program = scene::shader_program;
		extern GLFWwindow* window;
shader_program* current_program;
std::map<int, int> keys;

glm::mat4 pan;
float panx = 0, pany = 0;

void clear_buffers() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1.0, 0.3, 0.3, 0.0f);
}


void pan_view(float x, float y)
{
	pan = glm::translate(glm::mat4(1.0f), glm::vec3(x, y, 0.0f));
}
loader p_loader;
vbo texture_viewer;
game_engine* eng;

timespec diff(timespec start, timespec end)
{
	timespec temp;
	temp.tv_sec = end.tv_sec-start.tv_sec;
	temp.tv_nsec = end.tv_nsec-start.tv_nsec;
	if ((end.tv_nsec - start.tv_nsec) < 0)
	{
			temp.tv_sec -= 1;
			temp.tv_nsec += 1000000000;
	}
	return temp;
}
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	printf("const char *__restrict __format, ...");
	if (action == GLFW_PRESS || action == GLFW_REPEAT)
        keys[key] = true;
	else
		keys[key] = false;
}
game_sprite* Sprites(string spr) {
	return p_loader.get_sprite_ptr(spr);
}
game_tileset* Tilesets(string tset) {
	return p_loader.get_tileset_ptr(tset);
}
eng_texture* Textures(string tset) {
	return p_loader.get_texture_ptr(tset);
}
void deal_input() {
	if (keys[GLFW_KEY_UP])	pany += float(MAG);
	if (keys[GLFW_KEY_DOWN])	pany -= float(MAG);
	if (keys[GLFW_KEY_LEFT])	panx += float(MAG);
	if (keys[GLFW_KEY_RIGHT])	panx -= float(MAG);
	if (keys[GLFW_KEY_ESCAPE]) glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int tick_count = 0;
int main(int argc, char *argv[])
{
	setvbuf(stdout, NULL, _IONBF, 0);
	panx = 0.0f;pany = 0.0f;
	eng = new game_engine(HORZ_RES, VERT_RES, &p_loader);

	p_loader.load_project("./sample_project/dirs.json");
	
	auto mymap = p_loader.get_map("maps/map.json");
	auto t = p_loader.get_texture_ptr("tiles/img/indoor_free_tileset__by_thegreatblaid-d5x95zt.png");

	t->build_cache(cfg::ATILE);
	auto VP = eng->projection * eng->view;
	auto sp = eng->make_shader({"basic.vert", "frag.glsl"});
		sp.use_shaders();
	auto wp = eng->make_shader({"basic.vert", "all_white.glsl"});
	auto tile_buffer = eng->prepare_for(mymap);

	text_engine te;
	te.init();
	te.load_font(FONT, FONT_SIZE);
	auto txt = te.draw_text(FONT, "Teste magnífico de encapsulamento de text_rendering né? ÇAÇABA!");
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	texture_viewer.init();
		texture_viewer.buffer(eng->texture_viewer(txt,"texview1", 300,200,9));
		texture_viewer
			.add_pointer("position", 3, GL_FLOAT)
			.add_pointer("tex_coord", 2, GL_FLOAT)
	.attach(texture_viewer);
	bool show_demo_window = true;
	bool show_another_window = true;
	eng->objects["texview1"].click_function = [](eng_object* t, int x, int y)
	{
		eng->selected = t;
		//printf("%s\n", t->name.c_str());
	};
	texture_atlas atlas;

	eng->make_atlas({"sprites/sprite.json"});
	//glx::done = true;
	auto placeholderSprite = Sprites("sprite.json");
	//atlas.add(tmps->states["walking"][0]);
	//atlas.add(tmps->states["walking"][1]);
	auto tframe = Tilesets("tileset.json")->tiles[0][0];
	//atlas.add(tframe.img, tframe.u, tframe.v, cfg::ATILE, cfg::ATILE);
	//atlas.pack(1024, 1024);
	auto mat_move = glm::vec3(300,200,0);
	long frames = 0;
	auto sprTexture = Textures("sprites/img/sprite_sheet___aege_by_destructionseries-d5dg2g2.png");
	sprTexture->build_cache(placeholderSprite->states["walking"]);
	eng->add_sprite(placeholderSprite, 1, 0, 9);
	eng->add_sprite(placeholderSprite, 10, 20, 9);
	glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback);
	IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
	ImGui::StyleColorsDark();
	ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");

	while (!glfwWindowShouldClose(window))
	{
		struct timespec start, end;
		
		glfwPollEvents();
		ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
		//if (show_demo_window)
            ImGui::ShowDemoWindow(&show_demo_window);

        // 2. Show a simple window that we create ourselves. We use a Begin/End pair to created a named window.
        {
            static float f = 0.0f;
            static int counter = 0;

            ImGui::Begin("Sprites");                          // Create a window called "Hello, world!" and append into it.
			ImGui::BeginGroup();
			{
				ImGui::BeginGroup();
					ImGui::Columns(3, "statscols", true);
						ImGui::Text("Charset");
						if (ImGui::ListBoxHeader("##charset", 5))
						{
							ImVec2 size = ImGui::GetItemRectSize();
							ImGui::Selectable("Selected", true);
							ImGui::Selectable("Not Selected", false);
							ImGui::ListBoxFooter();
							// @TODO: use tables branch here
							ImGui::Button("+##btn_add_sprite");
							ImGui::SameLine();
							ImGui::Button("-##btn_del_sprite");
							ImGui::SameLine();
							ImGui::Button("x2##btn_copy_sprite");
						}
						ImGui::NextColumn();
						ImGui::Text("State");
						if (ImGui::ListBoxHeader("##states", 5))
						{
							for (auto it = eng->sprites[0].states.begin(); it != eng->sprites[0].states.end(); it++) {
								ImGui::Selectable(it->first.c_str(), false);
							}
							ImGui::ListBoxFooter();
							// @TODO: use tables branch here
							ImGui::Button("+##btn_add_state");
							ImGui::SameLine();
							ImGui::Button("-##btn_del_sprite");
							ImGui::SameLine();
							ImGui::Button("x2##btn_copy_sprite");
						}
						ImGui::NextColumn();
						ImGui::Text("Frame");
						if (ImGui::ListBoxHeader("##frames", 5))
						{
							ImGui::Selectable("Selected", true);
							ImGui::Selectable("Not Selected", false);
							ImGui::ListBoxFooter();
							// @TODO: use tables branch here
							ImGui::Button("+##btn_add_state");
							ImGui::SameLine();
							ImGui::Button("ins##btn_insert_state");
							ImGui::SameLine();
							ImGui::Button("-##btn_del_sprite");
							ImGui::SameLine();
							ImGui::Button("x2##btn_copy_sprite");
							ImGui::Button("^");
							ImGui::SameLine();
							ImGui::Button("V");
						}
						ImGui::NextColumn();
					ImGui::Columns(1);
				ImGui::EndGroup();
				// @TODO: add current frame view
				if (ImGui::IsItemHovered())
					ImGui::SetTooltip("First group hovered");
      		}
// Capture the group size and create widgets using the same size
        ImVec2 size = ImGui::GetItemRectSize();
		int i0;
		ImGui::Text("Interval");
		ImGui::SameLine();
		ImGui::InputInt("##input_interval", &i0);
		ImGui::Button("Change coords...");
		ImGui::Button("Collision...");
		ImGui::Button("Heightmap...");
		ImGui::Button("Hitboxes...");
		ImGui::Button("Offsets...");

		
        const float values[5] = { 0.5f, 0.20f, 0.80f, 0.60f, 0.25f };
        ImGui::PlotHistogram("##values", values, IM_ARRAYSIZE(values), 0, NULL, 0.0f, 1.0f, size);

        ImGui::Button("ACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x)*0.5f, size.y));
        ImGui::SameLine();
        ImGui::Button("REACTION", ImVec2((size.x - ImGui::GetStyle().ItemSpacing.x)*0.5f, size.y));
        ImGui::EndGroup();
        ImGui::SameLine();

       
            ImGui::End();
        }

        // 3. Show another simple window.
        if (show_another_window)
        {
            ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
            ImGui::Text("Hello from another window!");
            if (ImGui::Button("Close Me"))
                show_another_window = false;
            ImGui::End();
        }
		clock_gettime(CLOCK_MONOTONIC, &start);

		
		deal_input();
		pan_view(panx,pany);
		::clear_buffers();
		// draw tiles
		glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t->texture_id);
		sp.use_shaders();
		{
			sp.uniform("projection", VP);
			sp.uniform("model", pan);
		}
		sp.draw(tile_buffer);
		glBindTexture(GL_TEXTURE_2D, sprTexture->texture_id);
		sp.draw(eng->sprite_vbo);
		mat_move = glm::vec3(eng->objects["texview1"].x, eng->objects["texview1"].y,0);
		/*glBindTexture(GL_TEXTURE_2D, txt.texture_id);
			sp.uniform("model", glm::mat4());
			sp.uniform("v_trans", mat_move);
		sp.draw(texture_viewer);*/

		if (eng->selected != NULL)
		{
			wp.use_shaders();
			wp.uniform("projection", VP);
			wp.uniform("model", glm::mat4());
			wp.uniform("v_trans", mat_move);
			glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
			wp.draw(texture_viewer);
		}
		sp.use_shaders();
		sp.uniform("v_trans", glm::vec3(0.,0.,0.));

		
        // Rendering
        ImGui::Render();
		glfwMakeContextCurrent(window);
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
		glfwSwapBuffers(window);
		//glx::swap();

		clock_gettime(CLOCK_MONOTONIC, &end);
		auto msecs = int(diff(start, end).tv_nsec/1.0e6);
		float fps;
		frames++;
		//if (eng->tick(msecs)) eng->build_sprites();
		tick_count += msecs;
		if (tick_count >= 1000)
		{
			fps = float(frames) / float(msecs)*1000;
			frames = 0;
			char buf[200];
			sprintf(buf, "gl3 %.1f", fps);
			tick_count = 0;
			//glx::set_title(buf);
		}
	}
	//glx::clean_x();
	glfwDestroyWindow(window);
    glfwTerminate();
	return 0;
}
