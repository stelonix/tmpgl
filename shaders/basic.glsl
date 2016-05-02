#version 450 core                           //lower your version if GLSL 4.5 is not supported by your GPU
in vec3 position;
uniform mat4x4 projection;
 
void main()
{
	gl_Position = projection * vec4(position, 1.0);
}