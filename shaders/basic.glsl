uniform float scale;
attribute vec4 color;
attribute vec2 position;
varying vec4 v_color;
void main()
{
	gl_Position = vec4(scale*position, 0.0, 1.0);
	v_color = color;
}