#include "helpers/string_helper"
#include <vector>

#include <stdio.h>
#include <stdlib.h>
#include "shader.h"
#include "debug.h"

glsl_type_set type_set [] = {
  GL_INVALID_ENUM,                              "invalid",
  GL_FLOAT,                                     "float",
  GL_FLOAT_VEC2,                                "vec2",
  GL_FLOAT_VEC3,                                "vec3",
  GL_FLOAT_VEC4,                                "vec4",
  GL_DOUBLE,                                    "double",
  GL_DOUBLE_VEC2,                               "dvec2",
  GL_DOUBLE_VEC3,                               "dvec3",
  GL_DOUBLE_VEC4,                               "dvec4",
  GL_INT,                                       "int",
  GL_INT_VEC2,                                  "ivec2",
  GL_INT_VEC3,                                  "ivec3",
  GL_INT_VEC4,                                  "ivec4",
  GL_UNSIGNED_INT,                              "unsigned int",
  GL_UNSIGNED_INT_VEC2,                         "uvec2",
  GL_UNSIGNED_INT_VEC3,                         "uvec3",
  GL_UNSIGNED_INT_VEC4,                         "uvec4",
  GL_BOOL,                                      "bool",
  GL_BOOL_VEC2,                                 "bvec2",
  GL_BOOL_VEC3,                                 "bvec3",
  GL_BOOL_VEC4,                                 "bvec4",
  GL_FLOAT_MAT2,                                "mat2",
  GL_FLOAT_MAT3,                                "mat3",
  GL_FLOAT_MAT4,                                "mat4",
  GL_FLOAT_MAT2x3,                              "mat2x3",
  GL_FLOAT_MAT2x4,                              "mat2x4",
  GL_FLOAT_MAT3x2,                              "mat3x2",
  GL_FLOAT_MAT3x4,                              "mat3x4",
  GL_FLOAT_MAT4x2,                              "mat4x2",
  GL_FLOAT_MAT4x3,                              "mat4x3",
  GL_DOUBLE_MAT2,                               "dmat2",
  GL_DOUBLE_MAT3,                               "dmat3",
  GL_DOUBLE_MAT4,                               "dmat4",
  GL_DOUBLE_MAT2x3,                             "dmat2x3",
  GL_DOUBLE_MAT2x4,                             "dmat2x4",
  GL_DOUBLE_MAT3x2,                             "dmat3x2",
  GL_DOUBLE_MAT3x4,                             "dmat3x4",
  GL_DOUBLE_MAT4x2,                             "dmat4x2",
  GL_DOUBLE_MAT4x3,                             "dmat4x3",
  GL_SAMPLER_1D,                                "sampler1D",
  GL_SAMPLER_2D,                                "sampler2D",
  GL_SAMPLER_3D,                                "sampler3D",
  GL_SAMPLER_CUBE,                              "samplerCube",
  GL_SAMPLER_1D_SHADOW,                         "sampler1DShadow",
  GL_SAMPLER_2D_SHADOW,                         "sampler2DShadow",
  GL_SAMPLER_1D_ARRAY,                          "sampler1DArray",
  GL_SAMPLER_2D_ARRAY,                          "sampler2DArray",
  GL_SAMPLER_1D_ARRAY_SHADOW,                   "sampler1DArrayShadow",
  GL_SAMPLER_2D_ARRAY_SHADOW,                   "sampler2DArrayShadow",
  GL_SAMPLER_2D_MULTISAMPLE,                    "sampler2DMS",
  GL_SAMPLER_2D_MULTISAMPLE_ARRAY,              "sampler2DMSArray",
  GL_SAMPLER_CUBE_SHADOW,                       "samplerCubeShadow",
  GL_SAMPLER_BUFFER,                            "samplerBuffer",
  GL_SAMPLER_2D_RECT,                           "sampler2DRect",
  GL_SAMPLER_2D_RECT_SHADOW,                    "sampler2DRectShadow",
  GL_INT_SAMPLER_1D,                            "isampler1D",
  GL_INT_SAMPLER_2D,                            "isampler2D",
  GL_INT_SAMPLER_3D,                            "isampler3D",
  GL_INT_SAMPLER_CUBE,                          "isamplerCube",
  GL_INT_SAMPLER_1D_ARRAY,                      "isampler1DArray",
  GL_INT_SAMPLER_2D_ARRAY,                      "isampler2DArray",
  GL_INT_SAMPLER_2D_MULTISAMPLE,                "isampler2DMS",
  GL_INT_SAMPLER_2D_MULTISAMPLE_ARRAY,          "isampler2DMSArray",
  GL_INT_SAMPLER_BUFFER,                        "isamplerBuffer",
  GL_INT_SAMPLER_2D_RECT,                       "isampler2DRect",
  GL_UNSIGNED_INT_SAMPLER_1D,                   "usampler1D",
  GL_UNSIGNED_INT_SAMPLER_2D,                   "usampler2D",
  GL_UNSIGNED_INT_SAMPLER_3D,                   "usampler3D",
  GL_UNSIGNED_INT_SAMPLER_CUBE,                 "usamplerCube",
  GL_UNSIGNED_INT_SAMPLER_1D_ARRAY,             "usampler2DArray",
  GL_UNSIGNED_INT_SAMPLER_2D_ARRAY,             "usampler2DArray",
  GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE,       "usampler2DMS",
  GL_UNSIGNED_INT_SAMPLER_2D_MULTISAMPLE_ARRAY, "usampler2DMSArray",
  GL_UNSIGNED_INT_SAMPLER_BUFFER,               "usamplerBuffer",
  GL_UNSIGNED_INT_SAMPLER_2D_RECT,              "usampler2DRect",
  GL_IMAGE_1D,                                  "image1D",
  GL_IMAGE_2D,                                  "image2D",
  GL_IMAGE_3D,                                  "image3D",
  GL_IMAGE_2D_RECT,                             "image2DRect",
  GL_IMAGE_CUBE,                                "imageCube",
  GL_IMAGE_BUFFER,                              "imageBuffer",
  GL_IMAGE_1D_ARRAY,                            "image1DArray",
  GL_IMAGE_2D_ARRAY,                            "image2DArray",
  GL_IMAGE_2D_MULTISAMPLE,                      "image2DMS",
  GL_IMAGE_2D_MULTISAMPLE_ARRAY,                "image2DMSArray",
  GL_INT_IMAGE_1D,                              "iimage1D",
  GL_INT_IMAGE_2D,                              "iimage2D",
  GL_INT_IMAGE_3D,                              "iimage3D",
  GL_INT_IMAGE_2D_RECT,                         "iimage2DRect",
  GL_INT_IMAGE_CUBE,                            "iimageCube",
  GL_INT_IMAGE_BUFFER,                          "iimageBuffer",
  GL_INT_IMAGE_1D_ARRAY,                        "iimage1DArray",
  GL_INT_IMAGE_2D_ARRAY,                        "iimage2DArray",
  GL_INT_IMAGE_2D_MULTISAMPLE,                  "iimage2DMS",
  GL_INT_IMAGE_2D_MULTISAMPLE_ARRAY,            "iimage2DMSArray",
  GL_UNSIGNED_INT_IMAGE_1D,                     "uimage1D",
  GL_UNSIGNED_INT_IMAGE_2D,                     "uimage2D",
  GL_UNSIGNED_INT_IMAGE_3D,                     "uimage3D",
  GL_UNSIGNED_INT_IMAGE_2D_RECT,                "uimage2DRect",
  GL_UNSIGNED_INT_IMAGE_CUBE,                   "uimageCube",
  GL_UNSIGNED_INT_IMAGE_BUFFER,                 "uimageBuffer",
  GL_UNSIGNED_INT_IMAGE_1D_ARRAY,               "uimage1DArray",
  GL_UNSIGNED_INT_IMAGE_2D_ARRAY,               "uimage2DArray",
  GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE,         "uimage2DMS",
  GL_UNSIGNED_INT_IMAGE_2D_MULTISAMPLE_ARRAY,   "uimage2DMSArray",
  GL_UNSIGNED_INT_ATOMIC_COUNTER,               "atomic_uint"
};

namespace scene {
	
	int get_attrib_loc(char const* attrib, int program) {
		return glGetAttribLocation(program, attrib);
	}
	int get_attrib_loc(const char* attrib, GLuint program) {
		return glGetAttribLocation(program, attrib);
	}

	int get_uniform_loc(const char* attrib, GLuint program) {
		return glGetUniformLocation(program, attrib);
	}

	shader_program::shader_program() {
		new_scene();
	}

	void shader_program::new_scene() {
		program_id = glCreateProgram();
	}

	int shader_program::uniform(string identifier) {
		return get_uniform_loc(identifier.c_str(), program_id);
	}

	void shader_program::add_shader(const char* shader, const char* fname, GLenum type) {
		GLuint s = glCreateShader (type);
		glShaderSource(s, 1, &shader, NULL);
		glCompileShader(s);
		int result;
		glGetShaderiv(s, GL_COMPILE_STATUS, &result);
		if (result != GL_TRUE) {
			printf("Error compiling %s\n%s\n",fname,
				get_shader_log(s).c_str());
			
			backtrace();
			exit(1);
		}
		glAttachShader(program_id, s);
		attached.push_back(s);
	}
	void shader_program::link_shaders() {
		glUseProgram(program_id);
		glLinkProgram(program_id);
		for (auto i = 0; i < attached.size(); i++) {
			glDetachShader(program_id, attached[i]);
		}
		attached.clear();
	}

	void shader_program::use_shaders() {
		glUseProgram(program_id);
		current_program = this;
	}

	int shader_program::attrib(string attrib) {
		return get_attrib_loc(attrib.c_str(), program_id);
	}

	string get_shader_log(unsigned int shaderID) {
		std::vector<char> infoLog;
		GLint infoLen;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLen);
		infoLog.resize(infoLen);

		glGetShaderInfoLog(shaderID, infoLen, &infoLen, &infoLog[0]);

		return std::string(infoLog.begin(), infoLog.end());
	}

	void shader_program::draw(vbo& target)
	{
		target.draw();
	}

	void shader_program::uniform(string uni, std::vector<glm::vec3> value) {
		glUniform3fv(uniform(uni),
			value.size(),
			glm_addr(value.data())
		);
	}

	void shader_program::uniform(string uni, glm::vec3 value)
	{
		glUniform3fv(uniform(uni),
			1,
			glm_addr(&value)
		);
	}

	void shader_program::uniform(string uni, std::vector<glm::mat4> values, GLenum transpose) {
		glUniformMatrix4fv(uniform(uni),
			values.size(),
			transpose,
			glm_addr(values.data())
		);
	}

	void shader_program::uniform(string uni, glm::mat4 value, GLenum transpose) {
		glUniformMatrix4fv(uniform(uni),
			1,
			transpose,
			glm_addr(&value)
		);
	}
}

void
eTB_GLSL__print_uniforms (GLuint program)
{
  GLint uniform_count;
  glGetProgramiv (program, GL_ACTIVE_UNIFORMS, &uniform_count);

  GLchar name [256];

  for (GLint i = 0; i < uniform_count; i++) {
    memset (name, '\0', 256);
    GLint  size;
    GLenum type;

    glGetActiveUniform (program, i, 255, NULL, &size, &type, name);

    GLint location = glGetUniformLocation (program, name);

    for (int j = 0; j < sizeof (type_set) / sizeof (glsl_type_set); j++) {
      if (type_set [j].type != type)
        continue;

      const char* type_name = type_set [j].name;

      if (size > 1)
        printf ( "Uniform %d (loc=%d):\t%20s %-20s <Size: %d>\n",
                   i, location, type_name, name, size );
      else
        printf ( "Uniform %d (loc=%d):\t%20s %-20s\n",
                   i, location, type_name, name );

      break;
    }

    if (i == (uniform_count - 1))
      printf ("\n");
  }
}