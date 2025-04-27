#version 330

in vec4 mpos;
in vec3 mnormal;
in vec2 tcoord;
out vec4 fragColor;

uniform float zoom;
uniform int est_ciel;
uniform float temps;

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

uniform sampler2D permTexture;
uniform sampler2D gradTexture;

#define ONE 0.00390625
#define ONEHALF 0.001953125

float fade(float t) {
  return t*t*t*(t*(t*6.0-15.0)+10.0);
}

float noise(vec2 P)
{
  vec2 Pi = ONE*floor(P)+ONEHALF; 
  vec2 Pf = fract(P);             

  vec2 grad00 = texture(permTexture, Pi).rg * 4.0 - 1.0;
  float n00 = dot(grad00, Pf);

  vec2 grad10 = texture(permTexture, Pi + vec2(ONE, 0.0)).rg * 4.0 - 1.0;
  float n10 = dot(grad10, Pf - vec2(1.0, 0.0));

  vec2 grad01 = texture(permTexture, Pi + vec2(0.0, ONE)).rg * 4.0 - 1.0;
  float n01 = dot(grad01, Pf - vec2(0.0, 1.0));

  vec2 grad11 = texture(permTexture, Pi + vec2(ONE, ONE)).rg * 4.0 - 1.0;
  float n11 = dot(grad11, Pf - vec2(1.0, 1.0));

  vec2 n_x = mix(vec2(n00, n01), vec2(n10, n11), fade(Pf.x));

  float n_xy = mix(n_x.x, n_x.y, fade(Pf.y));

  return n_xy;
}

float rug2(void) {
  const float zoom = 1.0;
  float no = 0.0;
  for(float freq = 1.0, amp = 1.0; freq < 33.0; freq *= 2.0, amp /= 2.0)
    no += amp * noise((freq + temps/2.0) * zoom * tcoord);
  no = pow(no, 0.5);
  return no;
}

void main(void) {
  float n = 0.0, A = 1.0, mf = 1.0;
  if(est_ciel != 0){
    for(int i = 0; i < 5; ++i) {
      n += A * noise(tcoord * zoom * sin(mf + temps));
      A = A / 2.0;
      mf = mf * 2.0;
    }
    fragColor = vec4(1.0, 0.4, 0.0, 1.0) + vec4(n);
  }
  else{
    float nu = rug2() ;
    fragColor = vec4(nu, 0.5, 0.8, 1.0) + vec4(n);
  }
}
