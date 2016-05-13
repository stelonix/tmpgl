#ifndef BOILERPLATE_H
#define BOILERPLATE_H

#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

extern float panx, pany;
namespace glx {
	void poll ();
	void setup_x(int w, int h);
	void init_glew();
	void init_gl(int w, int h);
	void clean_x();
	void swap();
	extern bool done;
}

#endif