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

vec3 convertRgbToHsl(vec3 c) { 
    vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0); 
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g)); 
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r)); 
    
    float d = q.x - min(q.w, q.y); 
    float e = 1.0e-10; 
    return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x); 
} 

vec3 convertHslToRgb(vec3 c) { 
    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0); 
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www); 
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
} 

void main() {
	vec4 color = texture2D(al_tex, varying_texcoord);
	vec2 pos = (varying_origpos.xy * scale - offset) / size;
	vec4 tex_color = texture2D(tex, vec2(flipX ? 1.0 : 0.0, flipY ? 0.0 : 1.0) - pos * vec2(flipX ? 1.0 : -1.0, flipY ? -1.0 : 1.0));
	tex_color *= color.a;
	vec3 sat = convertRgbToHsl(tex_color.rgb);
	sat.g *= 1.2;
	tex_color = vec4(convertHslToRgb(sat), tex_color.a);
	gl_FragColor = tex_color * varying_color; //mix(color, tex_color, color.r) * varying_color;
}
