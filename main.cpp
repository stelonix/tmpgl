#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <map>
#include <iostream>
//#include <execinfo.h>
#include <signal.h>
#define BUFFER_OFFSET(i) ((void*)(i))
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092
Window win;
Display *display;
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
extern void show_backtrace(void);
extern void backtrace(void);

void handler(int sig) {
	backtrace();
	exit(1);
}

std::vector<std::string> list_files(std::string dir) {
	auto dirp = opendir(dir.c_str());
	auto ret_val = std::vector<std::string>();
	dirent* dp;
	while ((dp = readdir(dirp)) != NULL) {
		auto name = std::string(dp->d_name);
		if (name != "." && name != "..")
			ret_val.push_back(dp->d_name);
	}
	closedir(dirp);
	return ret_val;
}

std::string read_file(std::string filename) {
	//std::cout << filename << std::endl;
	FILE* f = fopen(filename.c_str(), "r");
	fseek(f, 0, SEEK_END);
	size_t size = ftell(f);

	char* contents = new char[size];

	rewind(f);
	fread(contents, sizeof(char), size, f);
	auto retval = std::string(contents);
	delete[] contents;
	return retval;
}

extern GLuint png_texture_load(const char* file_name, int* width, int* height);
// Helper to check for extension string presence.	Adapted from:
//	 http://www.opengl.org/resources/features/OGLextensions/
static bool isExtensionSupported(const char *extList, const char *extension) {
	const char *start;
	const char *where, *terminator;

	/* Extension names should not have spaces. */
	where = strchr(extension, ' ');
	if (where || *extension == '\0')
		return false;

	/* It takes a bit of care to be fool-proof about parsing the
		 OpenGL extensions string. Don't be fooled by sub-strings,
		 etc. */
	for (start = extList;;) {
		where = strstr(start, extension);

		if (!where)
			break;

		terminator = where + strlen(extension);

		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0')
				return true;

		start = terminator;
	}

	return false;
}

static bool ctxErrorOccurred = false;
static int ctxErrorHandler(Display *dpy, XErrorEvent *ev) {
	ctxErrorOccurred = true;
	return 0;
}
void orthogonalStart (int w, int h) {
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glOrtho(0, 800, 600, 0, -1, 1);
	//glScalef(1, -1, 1);
	//glTranslatef(0, (GLfloat)-h, 0);
}

void orthogonalEnd (void) {
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
}
std::string operator "" s (const char* p, size_t) {
	return std::string(p);
}
#define SHADER_DIR "./shaders"s
namespace ns {

template <typename T, typename U>
void foo(T t, U u) {
  backtrace(); // <-------- backtrace here!
}

}  // namespace ns

template <typename T>
struct Klass {
  T t;
  void bar() {
    ns::foo(t, true);
  }
};

bool done = false;
void poll () {
	XEvent event;
	while ( XPending(display) > 0 ){
		XNextEvent(display, &event);
		switch (event.type){
		case KeyPress: {
			char buf[2];
			int len;
			KeySym keysym_return;
			len = XLookupString(&event.xkey, buf, 1, &keysym_return, NULL);

			if ( len != 0 ){
				printf("Char: %c",buf[0]);
			}
			break;
		}
		case ClientMessage: {
			done = true;
			break;
		}
		case Expose: {
			/*XGetWindowAttributes(dpy, win, &gwa);
			glViewport(0, 0, gwa.width, gwa.height);*/
			glXSwapBuffers(display, win);
			break;
		}
		default:
			printf("Unhandled event: %d\n",event.type);
			break;
		}
	}
}
int main(int argc, char *argv[]) {
	signal(SIGSEGV, handler);
	setvbuf(stdout, NULL, _IONBF, 0);
	auto shaders = std::map<std::string, std::string>();
	auto d = list_files(SHADER_DIR);
	//printf("shd: %d\n", d.size());
	for (std::string s : d) {
		shaders[s] = (read_file(SHADER_DIR+"/"+s));
		printf("%s\n---\n%s\n---\n", s.c_str(), shaders[s].c_str());
	}
	display = XOpenDisplay(NULL);

	if (!display) {
		printf("Failed to open X display\n");
		exit(1);
	}

	// Get a matching FB config
	static int visual_attribs[] = {
		GLX_X_RENDERABLE, True,				 GLX_DRAWABLE_TYPE, GLX_WINDOW_BIT,
		GLX_RENDER_TYPE,	GLX_RGBA_BIT, GLX_X_VISUAL_TYPE, GLX_TRUE_COLOR,
		GLX_RED_SIZE,		 8,						GLX_GREEN_SIZE,		8,
		GLX_BLUE_SIZE,		8,						GLX_ALPHA_SIZE,		8,
		GLX_DEPTH_SIZE,	 24,					 GLX_STENCIL_SIZE,	8,
		GLX_DOUBLEBUFFER, True,
		// GLX_SAMPLE_BUFFERS	, 1,
		// GLX_SAMPLES				 , 4,
		None
	};

	int glx_major, glx_minor;

	// FBConfigs were added in GLX version 1.3.
	if (!glXQueryVersion(display, &glx_major, &glx_minor) ||
			((glx_major == 1) && (glx_minor < 3)) || (glx_major < 1)) {
		printf("Invalid GLX version");
		exit(1);
	}

	//printf("Getting matching framebuffer configs\n");
	int fbcount;
	GLXFBConfig *fbc = glXChooseFBConfig(display, DefaultScreen(display),
																			 visual_attribs, &fbcount);
	if (!fbc) {
		printf("Failed to retrieve a framebuffer config\n");
		exit(1);
	}
	printf("Found %d matching FB configs.\n", fbcount);

	// Pick the FB config/visual with the most samples per pixel
	//printf("Getting XVisualInfos\n");
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

	int i;
	for (i = 0; i < fbcount; ++i) {
		XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
		if (vi) {
			int samp_buf, samples;
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

			/*printf("	Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
						 " SAMPLES = %d\n",
						 i, vi->visualid, samp_buf, samples);*/

			if (best_fbc < 0 || samp_buf && samples > best_num_samp)
				best_fbc = i, best_num_samp = samples;
			if (worst_fbc < 0 || !samp_buf || samples < worst_num_samp)
				worst_fbc = i, worst_num_samp = samples;
		}
		XFree(vi);
	}

	GLXFBConfig bestFbc = fbc[best_fbc];

	// Be sure to free the FBConfig list allocated by glXChooseFBConfig()
	XFree(fbc);

	// Get a visual
	XVisualInfo *vi = glXGetVisualFromFBConfig(display, bestFbc);
	//printf("Chosen visual ID = 0x%x\n", vi->visualid);

	//printf("Creating colormap\n");
	XSetWindowAttributes swa;
	Colormap cmap;
	swa.colormap = cmap = XCreateColormap(
			display, RootWindow(display, vi->screen), vi->visual, AllocNone);
	swa.background_pixmap = None;
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;
	Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False); 
	

	//printf("Creating window\n");
	win = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0,
														 800, 600, 0, vi->depth, InputOutput, vi->visual,
														 CWBorderPixel | CWColormap | CWEventMask, &swa);
	XSetWMProtocols(display, win, &WM_DELETE_WINDOW, 1);
	if (!win) {
		printf("Failed to create window.\n");
		exit(1);
	}

	// Done with the visual info data
	XFree(vi);

	XStoreName(display, win, "GL 3.0 Window");

	//printf("Mapping window\n");
	XMapWindow(display, win);
	XSelectInput(display, win, ExposureMask | KeyPressMask);
	// Get the default screen's GLX extension list
	const char *glxExts =
			glXQueryExtensionsString(display, DefaultScreen(display));

	// NOTE: It is not necessary to create or make current to a context before
	// calling glXGetProcAddressARB
	glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
	glXCreateContextAttribsARB =
			(glXCreateContextAttribsARBProc)glXGetProcAddressARB(
					(const GLubyte *)"glXCreateContextAttribsARB");

	GLXContext ctx = 0;

	// Install an X error handler so the application won't exit if GL 3.0
	// context allocation fails.
	//
	// Note this error handler is global.	All display connections in all threads
	// of a process use the same error handler, so be sure to guard against other
	// threads issuing X commands while this code is running.
	ctxErrorOccurred = false;
	int (*oldHandler)(Display *, XErrorEvent *) =
			XSetErrorHandler(&ctxErrorHandler);

	// Check for the GLX_ARB_create_context extension string and the function.
	// If either is not present, use GLX 1.3 context creation method.
	if (!isExtensionSupported(glxExts, "GLX_ARB_create_context") ||
			!glXCreateContextAttribsARB) {
		printf("glXCreateContextAttribsARB() not found"
					 " ... using old-style GLX context\n");
		ctx = glXCreateNewContext(display, bestFbc, GLX_RGBA_TYPE, 0, True);
	}

	// If it does, try to get a GL 3.0 context!
	else {
		int context_attribs[] = {
			GLX_CONTEXT_MAJOR_VERSION_ARB, 4, GLX_CONTEXT_MINOR_VERSION_ARB, 3,
			// GLX_CONTEXT_FLAGS_ARB				, GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
			None
		};

		//printf("Creating context\n");
		ctx =
				glXCreateContextAttribsARB(display, bestFbc, 0, True, context_attribs);

		// Sync to ensure any errors generated are processed.
		XSync(display, False);
		if (!ctxErrorOccurred && ctx)
			printf("Created GL 3.0 context\n");
		else {
			// Couldn't create GL 3.0 context.	Fall back to old-style 2.x context.
			// When a context version below 3.0 is requested, implementations will
			// return the newest context version compatible with OpenGL versions less
			// than version 3.0.
			// GLX_CONTEXT_MAJOR_VERSION_ARB = 1
			context_attribs[1] = 1;
			// GLX_CONTEXT_MINOR_VERSION_ARB = 0
			context_attribs[3] = 0;

			ctxErrorOccurred = false;

			printf("Failed to create GL 3.0 context"
						 " ... using old-style GLX context\n");
			ctx = glXCreateContextAttribsARB(display, bestFbc, 0, True,
																			 context_attribs);
		}
	}

	// Sync to ensure any errors generated are processed.
	XSync(display, False);

	// Restore the original error handler
	XSetErrorHandler(oldHandler);

	if (ctxErrorOccurred || !ctx) {
		printf("Failed to create an OpenGL context\n");
		exit(1);
	}

	// Verifying that context is a direct context
	if (!glXIsDirect(display, ctx)) {
		printf("Indirect GLX rendering context obtained\n");
	} else {
		printf("Direct GLX rendering context obtained\n");
	}
	//printf("Making context current\n");
	glXMakeCurrent(display, win, ctx);
		int ww,hh;
	//int texture = png_texture_load("indoor_free_tileset__by_thegreatblaid-d5x95zt.png",&ww,&hh);
	//printf("Texture 1: %d\n", texture);
	//int texture2 = png_texture_load("indoor_free_tileset__by_thegreatblaid-d5x95zt.png",&ww,&hh);
	XEvent xev;
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
	  /* Problem: glewInit failed, something is seriously wrong. */
	  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	  
	}
	glViewport(0,0,800,600);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	auto projection = glm::ortho( 0.f, 800.f, 600.f, 0.0f, 0.0f, 100.f ); 
	glm::mat4 VP = glm::mat4();
	glm::mat4 view = glm::lookAt(
				glm::vec3(1,1,1), // Camera is at (0,0,5), in World Space
				glm::vec3(0,0,0), // and looks at the origin
				glm::vec3(0,0,0)  // Head is up (set to 0,-1,0 to look upside-down)
		);
	VP = projection;
	auto program = glCreateProgram();
	auto asdf = shaders["basic.glsl"].c_str();
	auto fdsa = shaders["frag.glsl"].c_str();
	GLuint vs = glCreateShader (GL_VERTEX_SHADER);
		glShaderSource(vs, 1, &asdf, NULL);
		glCompileShader(vs);
		int result;
		glGetShaderiv(vs, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			printf("vert fail\n");
			exit(1);
		}
		glAttachShader(program, vs);
		//glLinkProgram(program);
	GLuint fs = glCreateShader (GL_FRAGMENT_SHADER);
		glShaderSource(fs, 1, &fdsa, NULL);
		glCompileShader(fs);
		glGetShaderiv(fs, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			printf("frag fail\n");
			exit(1);
		}
		glAttachShader(program, fs);
		glLinkProgram(program);
	
	unsigned int vao;
	unsigned int vbo;

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	int x = 4;
	int y = 5;

	float vertices[][3] ={
		{x*32.0f,		y*32.0f,		0.0f},
		{(x+1)*32.0f,	y*32.0f,		0.0f},
		{(x+1)*32.0f,	(y+1)*32.0f,	0.0f},
		{x*32.0f,		(y+1)*32.0f,	0.0f}
	};

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * sizeof(vertices), &vertices[0], GL_STATIC_DRAW);
	
	while (1) {
		poll();
		if (done) {
			glXMakeCurrent(display, None, NULL);
			glXDestroyContext(display, ctx);
			XDestroyWindow(display, win);
			XFreeColormap(display, cmap);
			XCloseDisplay(display);
			return 0;
		}
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0, 0.3, 0.3, 0.0f);
		
		glUseProgram(program);
		auto loc = glGetAttribLocation(program, "position");
		glEnableVertexAttribArray(loc);
		glVertexAttribPointer(loc, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		auto projection_Location = glGetUniformLocation(program, "projection");
		printf("\nLocation: %d\n", projection_Location);
		glUniformMatrix4fv(projection_Location, 1, GL_FALSE, glm::value_ptr(VP));
		
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);
		glXSwapBuffers(display, win);
		
	}
	

	

	glXMakeCurrent(display, 0, 0);
	glXDestroyContext(display, ctx);

	XDestroyWindow(display, win);
	XFreeColormap(display, cmap);
	XCloseDisplay(display);

	return 0;
}
