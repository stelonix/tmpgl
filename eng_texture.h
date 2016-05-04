#include <GL/gl.h>
#include <tuple>

namespace engine {
	struct eng_texture {
		int w, h;
		float x_unit, y_unit;
		GLuint texture_id;

		eng_texture(GLuint texture_id, int w, int h);

		std::tuple<int,int> normalize(int u, int v);
	};
}