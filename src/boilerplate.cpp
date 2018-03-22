#include <iostream>
#include <map>
#include <vector>
#include "helpers/string"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GL/glx.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include "boilerplate.h"
#include "engine/engine.h"

#include "cfg.h"
using namespace cfg;

#define RGBA(r, g, b, a) r/255.0, g/255.0, b/255.0, a/255.0
#define RGB(r, g, b, a) r/255.0, g/255.0, b/255.0
extern void pan_view(float x, float y);
typedef GLXContext (*glXCreateContextAttribsARBProc)(Display*, GLXFBConfig, GLXContext, Bool, const int*);
extern std::map<int, int> keys;
extern game_engine* eng;
namespace glx {
	bool mouse_pressed = false;
	int moving_start_x = 0, moving_start_y = 0;
	Window win;
	Display *display;
	GLXContext ctx = 0;
	XSetWindowAttributes swa;
	Colormap cmap;
	bool done = false;
	void poll () {
		XEvent event;

		int len;

		while ( XPending(display) > 0 ){
			XNextEvent(display, &event);
			switch (event.type)
			{
				case KeyPress:
				{
					char buf[20];
					KeySym keysym_return;
					XLookupString(&event.xkey, buf, 1, &keysym_return, NULL);
					keys[keysym_return] = 1;
					//printf("press: %d +\n", event.xkey.keycode);
					XFlush(display);
					for (auto it = keys.begin(); it != keys.end(); it++) {
						//std::cout << it->first << ": " << it->second << std::endl;
					}

					break;
				}
				case KeyRelease:
				{
					char buf[20];
					KeySym keysym_return;
					XLookupString(&event.xkey, buf, 1, &keysym_return, NULL);
					keys[keysym_return] = 0;
					XFlush(display);
					//printf("release %d\n", event.xkey.keycode);

					for (auto it = keys.begin(); it != keys.end(); it++) {
						//std::cout << it->first << ": " << it->second << std::endl;
					}
					break;
				}
				case ButtonPress:
				{
					mouse_pressed = true;
					auto old_sel = eng->selected;
					eng->click_event(event.xbutton.x, event.xbutton.y);
					if (eng->selected) {
						moving_start_x = event.xbutton.x - eng->selected->x;
						moving_start_y = event.xbutton.y - eng->selected->y;
						//printf("clicked %d, %d\n", moving_start_x, moving_start_y);
					}
					break;
				}
				case ButtonRelease:
				{
					mouse_pressed = false;
					break;
				}
				case MotionNotify: {
					if (mouse_pressed && eng->selected) {
						eng->selected->x = event.xmotion.x - moving_start_x;
						eng->selected->y = event.xmotion.y - moving_start_y;
						//printf("moving  %d, %d\n", eng->selected->x, eng->selected->y);
					}
					break;
				}
				case ClientMessage:
				{
					done = true;
					break;
				}
				case Expose:
				{
					/*XGetWindowAttributes(dpy, win, &gwa);
					glViewport(0, 0, gwa.width, gwa.height);*/

					glXSwapBuffers(display, win);
					break;
				}
				default:
				{
					printf("Unhandled event: %d\n",event.type);
					break;
				}
			}
		}
	}
	// Helper to check for extension string presence.	Adapted from:
	//	 http://www.opengl.org/resources/features/OGLextensions/
	bool isExtensionSupported(const char *extList, const char *extension) {
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

	void set_title(const char* title)
	{
		XStoreName(display, win, title);
	}

	void setup_x(int w, int h) {
		keys[XK_Up] = false;
		keys[XK_Down] = false;
		keys[XK_Left] = false;
		keys[XK_Right] = false;
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
		XkbSetDetectableAutoRepeat(display, True, NULL);
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

		swa.colormap = cmap = XCreateColormap(
				display, RootWindow(display, vi->screen), vi->visual, AllocNone);
		swa.background_pixmap = None;
		swa.border_pixel = 0;
		swa.event_mask = StructureNotifyMask;
		Atom WM_DELETE_WINDOW = XInternAtom(display, "WM_DELETE_WINDOW", False);


		//printf("Creating window\n");
		win = XCreateWindow(display, RootWindow(display, vi->screen), 0, 0,
															 w, h, 0, vi->depth, InputOutput, vi->visual,
															 CWBorderPixel | CWColormap | CWEventMask, &swa);
		XSetWMProtocols(display, win, &WM_DELETE_WINDOW, 1);
		if (!win) {
			printf("Failed to create window.\n");
			exit(1);
		}

		// Done with the visual info data
		XFree(vi);

		set_title("GL 3.0 Window");

		//printf("Mapping window\n");
		XMapWindow(display, win);
		XSelectInput(display, win,
			ExposureMask | KeyPressMask | KeyReleaseMask |
			ButtonPressMask | ButtonReleaseMask | PointerMotionMask);
		// Get the default screen's GLX extension list
		const char *glxExts =
				glXQueryExtensionsString(display, DefaultScreen(display));

		// NOTE: It is not necessary to create or make current to a context before
		// calling glXGetProcAddressARB
		glXCreateContextAttribsARBProc glXCreateContextAttribsARB = 0;
		glXCreateContextAttribsARB =
				(glXCreateContextAttribsARBProc)glXGetProcAddressARB(
						(const GLubyte *)"glXCreateContextAttribsARB");



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
	}
	void clean_x() {
		glXMakeCurrent(display, None, NULL);
		glXDestroyContext(display, ctx);
		XDestroyWindow(display, win);
		XFreeColormap(display, cmap);
		XCloseDisplay(display);
	}
	void swap() {
		glXSwapBuffers(display, win);
	}

	void init_glew() {
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			/* Problem: glewInit failed, something is seriously wrong. */
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
		}
	}

	void clear_buffers() {
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glClearColor(1.0, 0.3, 0.3, 0.0f);
	}

	void init_gl(int w, int h) {
		glViewport(0,0,w,h);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
}
