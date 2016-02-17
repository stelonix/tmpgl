#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/gl.h>
#include <GL/glx.h>
#include <dirent.h>
#include <string>
#include <vector>
#include <iostream>
//#include <execinfo.h>
#include <signal.h>
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
int getFileAndLine (unw_word_t addr, char *file, size_t flen, int *line)
{
	static char buf[256];
	char *p;

	// prepare command to be executed
	// our program need to be passed after the -e parameter
	sprintf (buf, "/usr/bin/addr2line -C -e ./bin/gl3 -f -i %lx", addr);
	FILE* f = popen (buf, "r");

	if (f == NULL)
	{
		perror (buf);
		return 0;
	}

	// get function name
	fgets (buf, 256, f);

	// get file and line
	fgets (buf, 256, f);

	if (buf[0] != '?')
	{
		int l;
		char *p = buf;

		// file name is until ':'
		while (*p != ':')
		{
			p++;
		}

		*p++ = 0;
		// after file name follows line number
		strcpy (file , buf);
		sscanf (p,"%d", line);
	}
	else
	{
		strcpy (file,"unkown");
		*line = 0;
	}

	pclose(f);
}
void show_backtrace(void) {
	char name[256];
	unw_cursor_t cursor; unw_context_t uc;
	unw_word_t ip, sp, offp;

	unw_getcontext(&uc);
	unw_init_local(&cursor, &uc);

	while (unw_step(&cursor) > 0)
	{
		char file[256];
		int line = 0;

		name[0] = '\0';
		unw_get_proc_name(&cursor, name, 256, &offp);
		unw_get_reg(&cursor, UNW_REG_IP, &ip);
		unw_get_reg(&cursor, UNW_REG_SP, &sp);

		//printf ("%s ip = %lx, sp = %lx\n", name, (long) ip, (long) sp);
		getFileAndLine((long)ip, file, 256, &line);
		printf("%s in file %s line %d\n", name, file, line);
	}
}
void handler(int sig) {
	show_backtrace();
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
	std::cout << filename << std::endl;
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

int main(int argc, char *argv[]) {
	signal(SIGSEGV, handler);
	setvbuf(stdout, NULL, _IONBF, 0);
	auto shaders = std::vector<std::string>();
	auto d = list_files(SHADER_DIR);
	printf("shd: %d\n", d.size());
	for (std::string s : d) {
		shaders.push_back(read_file(SHADER_DIR+"/"+s));
		printf("%s\n---\n%s\n---\n", s.c_str(), shaders.back().c_str());
	}
	Display *display = XOpenDisplay(NULL);

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

	printf("Getting matching framebuffer configs\n");
	int fbcount;
	GLXFBConfig *fbc = glXChooseFBConfig(display, DefaultScreen(display),
																			 visual_attribs, &fbcount);
	if (!fbc) {
		printf("Failed to retrieve a framebuffer config\n");
		exit(1);
	}
	printf("Found %d matching FB configs.\n", fbcount);

	// Pick the FB config/visual with the most samples per pixel
	printf("Getting XVisualInfos\n");
	int best_fbc = -1, worst_fbc = -1, best_num_samp = -1, worst_num_samp = 999;

	int i;
	for (i = 0; i < fbcount; ++i) {
		XVisualInfo *vi = glXGetVisualFromFBConfig(display, fbc[i]);
		if (vi) {
			int samp_buf, samples;
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLE_BUFFERS, &samp_buf);
			glXGetFBConfigAttrib(display, fbc[i], GLX_SAMPLES, &samples);

			printf("	Matching fbconfig %d, visual ID 0x%2x: SAMPLE_BUFFERS = %d,"
						 " SAMPLES = %d\n",
						 i, vi->visualid, samp_buf, samples);

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
	printf("Chosen visual ID = 0x%x\n", vi->visualid);

	printf("Creating colormap\n");
	XSetWindowAttributes swa;
	Colormap cmap;
	swa.colormap = cmap = XCreateColormap(
			display, RootWindow(display, vi->screen), vi->visual, AllocNone);
	swa.background_pixmap = None;
	swa.border_pixel = 0;
	swa.event_mask = StructureNotifyMask;
	Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False); 
	

	printf("Creating window\n");
	Window win = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0,
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

	printf("Mapping window\n");
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

		printf("Creating context\n");
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
	printf("Making context current\n");
	glXMakeCurrent(display, win, ctx);
		int ww,hh;
	//int texture = png_texture_load("indoor_free_tileset__by_thegreatblaid-d5x95zt.png",&ww,&hh);
	//printf("Texture 1: %d\n", texture);
	//int texture2 = png_texture_load("indoor_free_tileset__by_thegreatblaid-d5x95zt.png",&ww,&hh);
	XEvent xev;
	while (1) {
		XNextEvent(display, &xev);
		if (xev.type == Expose) {
			//XGetWindowAttributes(dpy, win, &gwa);
				//glViewport(0, 0, gwa.width, gwa.height);
			//DrawAQuad(); 
				//glXSwapBuffers(dpy, win);
		} else if (xev.type == KeyPress||xev.type == ClientMessage) {
			glXMakeCurrent(display, None, NULL);
			glXDestroyContext(display, ctx);
			XDestroyWindow(display, win);
			XFreeColormap(display, cmap);
			XCloseDisplay(display);
			return 0;
		}
		// ** //
		glViewport(0,0,800,600);
		orthogonalStart(800,600);
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			glTranslatef(0, 0, 0);
			glClearColor(1, 1, 0, 1);
			glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			glColor3f(0, 1, 1);
			glBegin(GL_LINE_LOOP);
				glVertex2i(0, 0+1);
				glVertex2i(0+32, 0+1);
				glVertex2i(0+31, 0+32);
				glVertex2i(0, 0+32);
			glEnd();
			glFlush();
		//orthogonalEnd();
		glXSwapBuffers(display, win);
	}
	

	

	glXMakeCurrent(display, 0, 0);
	glXDestroyContext(display, ctx);

	XDestroyWindow(display, win);
	XFreeColormap(display, cmap);
	XCloseDisplay(display);

	return 0;
}
