#version 130                           //lower your version if GLSL 4.5 is not supported by your GPU
in vec3 position;

uniform mat4x4 projection;
out vec2 texCoord0;
in vec2 tex_coord;
void main()
{
	gl_Position = projection * vec4(position, 1.0);
	texCoord0 = tex_coord;
}