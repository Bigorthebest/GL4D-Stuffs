// https://www.shadertoy.com/view/MdffD7
// Fork of FMS_Cat's VCR distortion shader
// TODO: Add uniforms for tape crease discoloration and image jiggle

//uniform sampler2D tex: hint_tex, filter_linear_mipmap, repeat_disable;
#version 330

uniform vec2 vhs_resolution = vec2(320.0, 240.0);

uniform vec3 rgb_shift = vec3(0.0, 0.0, 0.0);

in vec2 UV ;
uniform float temps ;
out vec4 fragColor;
uniform sampler2D tex;

uniform int samples = 2;
uniform float crease_noise = 1.0;
uniform float crease_opacity = 0.5;
uniform float filter_intensity = 0.1;

uniform float tape_crease_smear = 0.2;
uniform float tape_crease_intensity = 0.2;
uniform float tape_crease_jitter = 0.10;
uniform float tape_crease_speed = 0.5;
uniform float tape_crease_discoloration = 1.0;

uniform float bottom_border_thickness = 6.0;
uniform float bottom_border_jitter = 6.0;

uniform float noise_intensity = 0.1;
//uniform sampler2D noise_texture: filter_linear_mipmap, repeat_enable;

uniform vec2 blur_direction = vec2(1.0, 0.0); // Flou horizontal par défaut
uniform float blur_intensity = 1.0; // Intensité du flou

vec3 apply_directional_blur(vec2 uv, sampler2D tex, vec2 direction, float intensity) {
	vec3 color = vec3(0.0);
	float total_weight = 0.0;
	for (int i = -samples; i <= samples; i++) {
		float weight = 1.0 - abs(float(i)) / float(samples); // Poids dégressif
		vec2 offset = direction * float(i) * intensity / vhs_resolution;
		color += texture(tex, uv + offset).rgb * weight;
		total_weight += weight;
	}
	return color / total_weight;
}


float v2random(vec2 uv) {
	//return texture(noise_texture, mod(uv, vec2(1.0))).x;
    return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

mat2 rotate2D(float t) {
	return mat2(vec2(cos(t), sin(t)), vec2(-sin(t), cos(t)));
}

vec3 rgb2yiq(vec3 rgb) {
	return mat3(vec3(0.299, 0.596, 0.211), vec3(0.587, -0.274, -0.523), vec3(0.114, -0.322, 0.312)) * rgb;
}

vec3 yiq2rgb(vec3 yiq) {
	return mat3(vec3(1.0, 1.0, 1.0), vec3(0.956, -0.272, -1.106), vec3(0.621, -0.647, 1.703)) * yiq;
}

vec3 vhx_tex_2D(sampler2D tex, vec2 uv, float rot) {
	vec3 yiq = vec3(0.0);
	for (int i = 0; i < samples; i++) {
		yiq += rgb2yiq(texture(tex, uv - vec2(float(i), 0.0) / vhs_resolution).xyz) *
				vec2(float(i), float(samples - 1 - i)).yxx / float(samples - 1)
				/ float(samples) * 2.0;
	}
	if (rot != 0.0) {
		yiq.yz *= rotate2D(rot * tape_crease_discoloration);
	}
	return yiq2rgb(yiq);
}

void main() {
	vec2 uvn = UV;
	vec3 col = vec3(0.0, 0.0, 0.0);
	
	// Tape wave.
	uvn.x += (v2random(vec2(uvn.y / 10.0, temps / 10.0) / 1.0) - 0.5) / vhs_resolution.x * 1.0;
	uvn.x += (v2random(vec2(uvn.y, temps * 10.0)) - 0.5) / vhs_resolution.x * 1.0;
	// tape crease
	float tc_phase = smoothstep(0.9, 0.96, sin(uvn.y * 8.0 - (temps * tape_crease_speed + tape_crease_jitter * v2random(temps * vec2(0.67, 0.59))) * 3.1415926 * 1.2));
	float tc_noise = smoothstep(0.3, 1.0, v2random(vec2(uvn.y * 4.77, temps)));
	float tc = tc_phase * tc_noise;
	uvn.x = uvn.x - tc / vhs_resolution.x * 8.0 * tape_crease_smear;
	// switching noise
	float sn_phase = smoothstep(1.0 - bottom_border_thickness / vhs_resolution.y, 1.0, uvn.y);
	uvn.x += sn_phase * (v2random(vec2(UV.y * 100.0, temps * 10.0)) - 0.5) / vhs_resolution.x * bottom_border_jitter;
	// fetch
	col = vhx_tex_2D(tex, uvn, tc_phase * 0.2 + sn_phase * 2.0);
	// crease noise
	float cn = tc_noise * crease_noise * (0.7 * tc_phase * tape_crease_intensity + 0.3);
	if (0.29 < cn) {
		vec2 V = vec2(0.0, crease_opacity);
		vec2 uvt = (uvn + V.yx * v2random(vec2(uvn.y, temps))) * vec2(0.1, 1.0);
		float n0 = v2random(uvt);
		float n1 = v2random(uvt + V.yx / vhs_resolution.x);
		if (n1 < n0) {
			col = mix(col, 2.0 * V.yyy, pow(n0, 10.0));
		}
	}
	
	// ac beat
	col *= 1.0 + 0.1 * smoothstep(0.4, 0.6, v2random(vec2(0.0, 0.1 * (UV.y + temps * 0.2)) / 10.0));
	
	// color noise
	col *= 1.0 - noise_intensity * 0.5 + noise_intensity * 2.0 * vec3(v2random(mod(uvn * vec2(1.0, 1.0) + temps * vec2(5.97, 4.45), vec2(1.0)))).xxx;
	col = clamp(col, 0.0, 1.0);
	// yiq
	col = rgb2yiq(col);
	col = vec3(0.9, 1.1, 1.5) * col + vec3(0.1, -0.1, 0.0) * filter_intensity;
	col = yiq2rgb(col);
	
	// Décalage RGB
	vec2 red_uv = uvn + vec2(rgb_shift.r, 0.0);   // Décalage du canal rouge
	vec2 green_uv = uvn + vec2(rgb_shift.g, 0.0); // Décalage du canal vert
	vec2 blue_uv = uvn + vec2(rgb_shift.b, 0.0);  // Décalage du canal bleu
	float r = texture(tex, red_uv).r;
	float g = texture(tex, green_uv).g;
	float b = texture(tex, blue_uv).b;
	col = vec3(r, g, b);
	// Application du flou directionnel
	//vec3 blurred_col = apply_directional_blur(uvn, tex, blur_direction, blur_intensity);
	//col = mix(col, blurred_col, 0.5); // Mélange des couleurs décalées avec le flou
	fragColor = texture(tex,UV);//vec4(col, 1.0);
    //fragColor = vec4(0.0,1.0,0.0,1.0);
	//COLOR = vec4(r, g, b, 1.0);
	//COLOR = vec4(col, 1.0);
	//COLOR = vec4(col.r + r, col.g + g, col.b + b, 0.5);
	//COLOR = vec4(mix(col.r, r, 0.5), mix(col.g, g, 0.5), mix(col.b, b, 0.5), 1.0);
}
