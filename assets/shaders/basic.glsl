#version 130                           //lower your version if GLSL 4.5 is not supported by your GPU
in vec3 position;

uniform mat4x4 projection;
uniform mat4x4 model;
uniform vec3 v_trans;

out vec2 texCoord0;
in vec2 tex_coord;
void main()
{
	vec3 real_pos = vec3(position.x, position.y, position.z) + v_trans;
	gl_Position = projection * model * vec4(real_pos, 1.0);
	texCoord0 = tex_coord;
}