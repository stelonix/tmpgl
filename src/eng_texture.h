#include <tuple>
#include <GL/gl.h>

namespace engine {
	float normalize(int v, float unit);

	struct eng_texture {
		int w, h;
		int internal_w, internal_h;
		float x_unit, y_unit;
		GLuint texture_id;

		eng_texture(GLuint texture_id, int w, int h, int rw, int rh);

		float normalize_u(int u);
		float normalize_v(int v);
	};
}