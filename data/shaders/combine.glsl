#ifdef GL_ES
precision highp float;
#endif

uniform sampler2D al_tex;
uniform sampler2D tex;
uniform vec2 size;
uniform vec2 offset;
uniform float scale;
uniform bool flipX;
uniform bool flipY;
varying vec2 varying_texcoord;
varying vec4 varying_color;
varying vec4 varying_origpos;

void main() {
	vec4 color = texture2D(al_tex, varying_texcoord);
	vec2 pos = (varying_origpos.xy * scale - offset) / size;
	vec4 tex_color = texture2D(tex, vec2(flipX ? 1.0 : 0.0, flipY ? 0.0 : 1.0) - pos * vec2(flipX ? 1.0 : -1.0, flipY ? -1.0 : 1.0));
	tex_color *= color.a;
	gl_FragColor = tex_color * varying_color; //mix(color, tex_color, color.r) * varying_color;
}
