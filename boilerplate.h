#define GLX_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define GLX_CONTEXT_MINOR_VERSION_ARB 0x2092

namespace glx {
	void poll ();
	void setup_x();
	void clean_x();
	void swap();
	extern bool done;
}