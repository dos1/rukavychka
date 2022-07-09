#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D al_tex;
varying vec2 varying_texcoord;
varying vec4 varying_color;
varying vec4 varying_origpos;

void main() {
	vec4 color = texture2D(al_tex, varying_texcoord);
	color *= (pow(1.0 - (color.r - 0.075), 0.75) * 0.04) + 0.075;
	gl_FragColor = color * varying_color;
}
