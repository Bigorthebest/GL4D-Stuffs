#version 330

uniform sampler2D tex;

uniform vec2 vhs_resolution = vec2(320.0, 240.0);

uniform vec3 rgb_shift = vec3(0.015, 0.00, 0.005);
uniform vec3 color_bruit = vec3(0.2) ;

// Basic shader inputs
uniform float time;
uniform float bruit;
in vec2 vsoTexCoord;
#define PI 3.1415926535897932384626433832795
out vec4 fragColor;

uniform int samples = 0;

// Tape crease uniforms
uniform float tape_crease_smear = 1.0;
uniform float tape_crease_jitter = 0.10;
uniform float tape_crease_speed = 0.5;
uniform float tape_crease_discoloration = 1.0;

// Noise uniforms
//uniform vec2 blur_direction = vec2(1.0, 0.0); // Flou horizontal par défaut
//uniform float blur_intensity = 1.0; // Intensité du flou

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
	//return 0.5;// texture(noise_texture, mod(uv, vec2(1.0))).x;
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
	vec2 uvn = vsoTexCoord;
	vec3 col = vec3(0.0, 0.0, 0.0);

	// Tape wave.
	//uvn.x += (v2random(vec2(uvn.y / 10.0, time / 10.0) / 1.0) - 0.5) / vhs_resolution.x * 1.0;
	uvn.x += (v2random(vec2(uvn.y, time * 10.0)) - 0.5) / vhs_resolution.x * 1.0;
	// tape crease
	float tc_phase = smoothstep(0.90, 0.96, sin(uvn.y * 8.0 - (time * tape_crease_speed + tape_crease_jitter * v2random(time * vec2(0.67, 0.59))) * PI * 1.2));
	float tc_noise = smoothstep(0.3, 1.0, v2random(vec2(uvn.y * 4.77, time)));
	float tc = tc_phase * tc_noise;
	uvn.x = uvn.x - tc / vhs_resolution.x * 8.0 * tape_crease_smear;//mettre tc pour le glitch classique 
	//Le glitch de la dupli a l'infinie 
	uvn = vec2(0.5 + time * cos(uvn.x), 0.5 + time * sin(uvn.y));
	//uvn = vec2(uvn.x * time, uvn.y * time) ;
	//Glitch pour aleks :) 
	//uvn.x = uvn.x - tc_phase / vhs_resolution.x * 8.0 * tape_crease_smear;
	// Décalage RGB
	vec2 red_uv = uvn + vec2(rgb_shift.r, 0.0);
	vec2 green_uv = uvn + vec2(rgb_shift.g, 0.0);
	vec2 blue_uv = uvn + vec2(rgb_shift.b, 0.0);
	float r = texture(tex, red_uv).r;
	float g = texture(tex, green_uv).g;
	float b = texture(tex, blue_uv).b;
	col = vec3(r, g, b);
	//Création de bruit à l'écran 
	if(bruit != 0){
		float r = v2random(uvn) ;
		if(r * 2 > bruit){
    		fragColor = vec4(col, 1.0);
		}
  		else{
    		fragColor = vec4(color_bruit,1.0);
		}
	}
	else {
		fragColor = vec4(col, 1.0);
	}
	//fragColor = texture(tex,UV) ;
}
