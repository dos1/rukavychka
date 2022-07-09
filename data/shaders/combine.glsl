#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D al_tex;
uniform sampler2D tex;
uniform vec2 size;
uniform vec2 offset;
varying vec2 varying_texcoord;
varying vec4 varying_color;
varying vec4 varying_origpos;

void main() {
	vec4 color = texture2D(al_tex, varying_texcoord);
	vec4 tex_color = texture2D(tex, (varying_origpos.xy - offset) / size);
	tex_color *= 1.0 - color.r;
	gl_FragColor = tex_color * varying_color; //mix(color, tex_color, color.r) * varying_color;
}
