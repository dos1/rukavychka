float3 convertRgbToHsl(float3 c) {
    float4 K = float4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
    float4 p = lerp(float4(c.bg, K.wz), float4(c.gb, K.xy), step(c.b, c.g));
    float4 q = lerp(float4(p.xyw, c.r), float4(c.r, p.yzx), step(p.x, c.r));

    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return float3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

float3 convertHslToRgb(float3 c) {
    float4 K = float4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
    float3 p = abs(frac(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * lerp(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float4 main(
  uniform sampler2D al_tex : TEXUNIT0,
  uniform sampler2D tex : TEXUNIT1,
  uniform float2 offset,
  uniform float2 scale,
  uniform float saturation,
  uniform float brightness,
  float4 varying_color : COLOR,
  float2 varying_texcoord : TEXCOORD0,
  float4 varying_pos : TEXCOORD1)
{
	float4 color = float4(1.0) - tex2D(al_tex, varying_texcoord);
	float4 tex_color = tex2D(tex, (varying_pos.xy / scale + 1.0) / 2.0 - offset * float2(1.0, -1.0));
	float3 sat = convertRgbToHsl(tex_color.rgb * color.a);
	sat.y *= saturation;
	sat.z *= brightness;
	return mul(float4(convertHslToRgb(sat), tex_color.a * color.a), varying_color);
}
