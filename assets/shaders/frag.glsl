#version 130  
//varying vec4 v_color;
uniform sampler2D tex;
in vec2 texCoord0;
void main() {
	//vec4 color = texture2D(tex, gl_TexCoord[0].st);
	//gl_FragColor = vec4(1.0,0.0,1.0,1.0);
	gl_FragColor = texture2D(tex, texCoord0);//vec4(1.0,1.0,1.0,1.0);
}