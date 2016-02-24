#version 450 core                           //lower your version if GLSL 4.5 is not supported by your GPU

layout (location = 0) in vec3 VertexPosition;
uniform mat4x4 a;
 
void main()
{
	gl_Position = a * vec4(VertexPosition, 1.0);
}