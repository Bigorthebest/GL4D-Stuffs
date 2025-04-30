#version 330
in  vec2 vsoTexCoord;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

uniform sampler2D tex;


/*
 * 2D, 3D and 4D Perlin noise, classic and simplex, in a GLSL fragment shader.
 *
 * Classic noise is implemented by the functions:
 * float noise(vec2 P)
 * float noise(vec3 P)
 * float noise(vec4 P)
 *
 * Simplex noise is implemented by the functions:
 * float snoise(vec2 P)
 * float snoise(vec3 P)
 * float snoise(vec4 P)
 *
 * Author: Stefan Gustavson ITN-LiTH (stegu@itn.liu.se) 2004-12-05
 * Simplex indexing functions by Bill Licea-Kane, ATI (bill@ati.com)
 *
 * You may use, modify and redistribute this code free of charge,
 * provided that the author's names and this notice appear intact.
 */

/*
 * NOTE: there is a formal problem with the dependent texture lookups.
 * A texture coordinate of exactly 1.0 will wrap to 0.0, so strictly speaking,
 * an error occurs every 256 units of the texture domain, and the same gradient
 * is used for two adjacent noise cells. One solution is to set the texture
 * wrap mode to "CLAMP" and do the wrapping explicitly in GLSL with the "mod"
 * operator. This could also give you noise with repetition intervals other
 * than 256 without any extra cost.
 * This error is not even noticeable to the eye even if you isolate the exact
 * position in the domain where it occurs and know exactly what to look for.
 * The noise pattern is still visually correct, so I left the bug in there.
 * 
 * The value of classic 4D noise goes above 1.0 and below -1.0 at some
 * points. Not much and only very sparsely, but it happens. This is a
 * bug from the original software implementation, so I left it untouched.
 */


/*
 * "permTexture" is a 256x256 texture that is used for both the permutations
 * and the 2D and 3D gradient lookup. For details, see the main C program.
 * "gradTexture" is a 256x256 texture with 4D gradients, similar to
 * "permTexture" but with the permutation index in the alpha component
 * replaced by the w component of the 4D gradient.
 * 2D classic noise uses only permTexture.
 * 2D simplex noise uses only permTexture.
 * 3D classic noise uses only permTexture.
 * 3D simplex noise uses only permTexture.
 * 4D classic noise uses permTexture and gradTexture.
 * 4D simplex noise uses permTexture and gradTexture.
 */
//#extension GL_ARB_explicit_attrib_location : enable

uniform sampler2D permTexture;
uniform sampler2D gradTexture;


/*
 * To create offsets of one texel and one half texel in the
 * texture lookup, we need to know the texture image size.
 */
#define ONE 0.00390625
#define ONEHALF 0.001953125
// The numbers above are 1/256 and 0.5/256, change accordingly
// if you change the code to use another texture size.


/*
 * The interpolation function. This could be a 1D texture lookup
 * to get some more speed, but it's not the main part of the algorithm.
 */
float fade(float t) {
  // return t*t*(3.0-2.0*t); // Old fade, yields discontinuous second derivative
  return t*t*t*(t*(t*6.0-15.0)+10.0); // Improved fade, yields C2-continuous noise
}

/*
 * Efficient simplex indexing functions by Bill Licea-Kane, ATI. Thanks!
 * (This was originally implemented as a texture lookup. Nice to avoid that.)
 */
void simplex( const in vec3 P, out vec3 offset1, out vec3 offset2 )
{
  vec3 offset0;
 
  vec2 isX = step( P.yz, P.xx );         // P.x >= P.y ? 1.0 : 0.0;  P.x >= P.z ? 1.0 : 0.0;
  offset0.x  = dot( isX, vec2( 1.0 ) );  // Accumulate all P.x >= other channels in offset.x
  offset0.yz = 1.0 - isX;                // Accumulate all P.x <  other channels in offset.yz

  float isY = step( P.z, P.y );          // P.y >= P.z ? 1.0 : 0.0;
  offset0.y += isY;                      // Accumulate P.y >= P.z in offset.y
  offset0.z += 1.0 - isY;                // Accumulate P.y <  P.z in offset.z
 
  // offset0 now contains the unique values 0,1,2 in each channel
  // 2 for the channel greater than other channels
  // 1 for the channel that is less than one but greater than another
  // 0 for the channel less than other channels
  // Equality ties are broken in favor of first x, then y
  // (z always loses ties)

  offset2 = clamp(   offset0, 0.0, 1.0 );
  // offset2 contains 1 in each channel that was 1 or 2
  offset1 = clamp( --offset0, 0.0, 1.0 );
  // offset1 contains 1 in the single channel that was 1
}

void simplex( const in vec4 P, out vec4 offset1, out vec4 offset2, out vec4 offset3 )
{
  vec4 offset0;
 
  vec3 isX = step( P.yzw, P.xxx );        // See comments in 3D simplex function
  offset0.x = dot( isX, vec3( 1.0 ) );
  offset0.yzw = 1.0 - isX;

  vec2 isY = step( P.zw, P.yy );
  offset0.y += dot( isY, vec2( 1.0 ) );
  offset0.zw += 1.0 - isY;
 
  float isZ = step( P.w, P.z );
  offset0.z += isZ;
  offset0.w += 1.0 - isZ;

  // offset0 now contains the unique values 0,1,2,3 in each channel

  offset3 = clamp(   offset0, 0.0, 1.0 );
  offset2 = clamp( --offset0, 0.0, 1.0 );
  offset1 = clamp( --offset0, 0.0, 1.0 );
}


/*
 * 2D classic Perlin noise. Fast, but less useful than 3D noise.
 */
float noise(vec2 P)
{
  vec2 Pi = ONE*floor(P)+ONEHALF; // Integer part, scaled and offset for texture lookup
  vec2 Pf = fract(P);             // Fractional part for interpolation

  // Noise contribution from lower left corner
  vec2 grad00 = texture(permTexture, Pi).rg * 4.0 - 1.0;
  float n00 = dot(grad00, Pf);

  // Noise contribution from lower right corner
  vec2 grad10 = texture(permTexture, Pi + vec2(ONE, 0.0)).rg * 4.0 - 1.0;
  float n10 = dot(grad10, Pf - vec2(1.0, 0.0));

  // Noise contribution from upper left corner
  vec2 grad01 = texture(permTexture, Pi + vec2(0.0, ONE)).rg * 4.0 - 1.0;
  float n01 = dot(grad01, Pf - vec2(0.0, 1.0));

  // Noise contribution from upper right corner
  vec2 grad11 = texture(permTexture, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float n11 = dot(grad11, Pf - vec2(1.0, 1.0));

  // Blend contributions along x
  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(Pf.x));

  // Blend contributions along y
  float n_xy = mix(n_x.x, n_x.y, fade(Pf.y));

  // We're done, return the final noise value.
  return n_xy;
}

// https://www.shadertoy.com/view/MdffD7
// Fork of FMS_Cat's VCR distortion shader

shader_type canvas_item;

// TODO: Add uniforms for tape crease discoloration and image jiggle

uniform sampler2D screen_texture: hint_screen_texture, filter_linear_mipmap, repeat_disable;

uniform vec2 vhs_resolution = vec2(320.0, 240.0);

uniform vec3 rgb_shift = vec3(0.0, 0.0, 0.0);

uniform int samples = 2;
uniform float crease_noise: hint_range(0.0, 2.0, 0.1) = 1.0;
uniform float crease_opacity: hint_range(0.0, 1.0, 0.1) = 0.5;
uniform float filter_intensity: hint_range(0.0, 1.0, 0.1) = 0.1;

group_uniforms tape_crease;
uniform float tape_crease_smear: hint_range(0.0, 2.0, 0.1) = 0.2;
uniform float tape_crease_intensity: hint_range(0.0, 1.0, 0.1) = 0.2;
uniform float tape_crease_jitter: hint_range(0.0, 1.0, 0.01) = 0.10;
uniform float tape_crease_speed: hint_range(-2.0, 2.0, 0.1) = 0.5;
uniform float tape_crease_discoloration: hint_range(0.0, 2.0, 0.1) = 1.0;

group_uniforms bottom_border;
uniform float bottom_border_thickness: hint_range(0.0,32.0, 0.1) = 6.0;
uniform float bottom_border_jitter: hint_range(0.0, 24.0, 0.5) = 6.0;

group_uniforms noise;
uniform float : hint_range(0.0, 1.0, 0.1) = 0.1;
uniform sampler2D noise_texture: filter_linear_mipmap, repeat_enable;

uniform vec2 blur_direction = vec2(1.0, 0.0); // Flou horizontal par défaut
uniform float blur_intensity: hint_range(0.0, 10.0, 0.1) = 1.0; // Intensité du flou

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
	return texture(noise_texture, mod(uv, vec2(1.0))).x;
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

void fragment() {
	vec2 uvn = UV;
	vec3 col = vec3(0.0, 0.0, 0.0);
	
	// Tape wave.
	uvn.x += (v2random(vec2(uvn.y / 10.0, TIME / 10.0) / 1.0) - 0.5) / vhs_resolution.x * 1.0;
	uvn.x += (v2random(vec2(uvn.y, TIME * 10.0)) - 0.5) / vhs_resolution.x * 1.0;
	// tape crease
	float tc_phase = smoothstep(0.9, 0.96, sin(uvn.y * 8.0 - (TIME * tape_crease_speed + tape_crease_jitter * v2random(TIME * vec2(0.67, 0.59))) * PI * 1.2));
	float tc_noise = smoothstep(0.3, 1.0, v2random(vec2(uvn.y * 4.77, TIME)));
	float tc = tc_phase * tc_noise;
	uvn.x = uvn.x - tc / vhs_resolution.x * 8.0 * tape_crease_smear;
	// switching noise
	float sn_phase = smoothstep(1.0 - bottom_border_thickness / vhs_resolution.y, 1.0, uvn.y);
	uvn.x += sn_phase * (v2random(vec2(UV.y * 100.0, TIME * 10.0)) - 0.5) / vhs_resolution.x * bottom_border_jitter;
	// fetch
	col = vhx_tex_2D(screen_texture, uvn, tc_phase * 0.2 + sn_phase * 2.0);
	// crease noise
	float cn = tc_noise * crease_noise * (0.7 * tc_phase * tape_crease_intensity + 0.3);
	if (0.29 < cn) {
		vec2 V = vec2(0.0, crease_opacity);
		vec2 uvt = (uvn + V.yx * v2random(vec2(uvn.y, TIME))) * vec2(0.1, 1.0);
		float n0 = v2random(uvt);
		float n1 = v2random(uvt + V.yx / vhs_resolution.x);
		if (n1 < n0) {
			col = mix(col, 2.0 * V.yyy, pow(n0, 10.0));
		}
	}
	
	// ac beat
	col *= 1.0 + 0.1 * smoothstep(0.4, 0.6, v2random(vec2(0.0, 0.1 * (UV.y + TIME * 0.2)) / 10.0));
	
	// color noise
	col *= 1.0 - noise_intensity * 0.5 + noise_intensity * 2.0 * texture(noise_texture, mod(uvn * vec2(1.0, 1.0) + TIME * vec2(5.97, 4.45), vec2(1.0))).xyz;
	col = clamp(col, 0.0, 1.0);
	// yiq
	col = rgb2yiq(col);
	col = vec3(0.9, 1.1, 1.5) * col + vec3(0.1, -0.1, 0.0) * filter_intensity;
	col = yiq2rgb(col);
	
	// Décalage RGB
	vec2 red_uv = uvn + vec2(rgb_shift.r, 0.0);   // Décalage du canal rouge
	vec2 green_uv = uvn + vec2(rgb_shift.g, 0.0); // Décalage du canal vert
	vec2 blue_uv = uvn + vec2(rgb_shift.b, 0.0);  // Décalage du canal bleu
	float r = texture(screen_texture, red_uv).r;
	float g = texture(screen_texture, green_uv).g;
	float b = texture(screen_texture, blue_uv).b;
	col = vec3(r, g, b);
	// Application du flou directionnel
	//vec3 blurred_col = apply_directional_blur(uvn, screen_texture, blur_direction, blur_intensity);
	//col = mix(col, blurred_col, 0.5); // Mélange des couleurs décalées avec le flou
	COLOR = vec4(col, 1.0);
	//COLOR = vec4(r, g, b, 1.0);
	//COLOR = vec4(col, 1.0);
	//COLOR = vec4(col.r + r, col.g + g, col.b + b, 0.5);
	//COLOR = vec4(mix(col.r, r, 0.5), mix(col.g, g, 0.5), mix(col.b, b, 0.5), 1.0);
}

void main(void) {
  int y = int(vsoTexCoord.y * 300.0);
  float n_map = noise(vsoTexCoord) ;
  if((vsoTexCoord.x * n_map) > 0.5 && (vsoTexCoord.y * n_map) > 0.5)
    fragColor = texture(tex, vsoTexCoord);
  else
    fragColor = vec4(0.5);
}
