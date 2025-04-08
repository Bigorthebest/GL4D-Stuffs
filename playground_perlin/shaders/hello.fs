#version 330
in vec4 mpos;
in vec3 mnormal;
in vec2 tcoord;
out vec4 fragColor;

uniform vec4 couleur;
uniform vec4 lumpos;
uniform mat4 view;
uniform float temps ;

void main() {
  const vec3 V = vec3(0.0, 0.0, -1.0);
  const vec4 acouleur = vec4(0.0, 0.5, 0.796, 1.0);
  vec3 Ld = normalize(mpos.xyz - lumpos.xyz);
  vec3 N = mnormal;
  //Simulation de normal map 
  vec3 T = vec3(1.0, 0.0, 0.0);
  vec3 B = cross(T, N);
  vec2 wave = (1.0 - length(tcoord) / sqrt(2.0)) * (sin( tcoord * 6.283 - temps) + 1.0) / 2.0;//6.283
  N = normalize(4.0 * N + (wave.x * T + wave.y * B));
  //Fin simulation normal map 
  float ild = clamp(dot(N, -Ld), 0.0, 1.0);
  vec3 R = reflect(Ld, N);
  R = normalize((transpose(inverse(view)) * vec4(R, 0.0)).xyz);
  float ils = clamp(dot(R, -V), 0.0, 1.0);
  //vec4 tmp = ils * vec4(1.0, 0.0, 0.0, 1.0) + 0.30 * acouleur + 0.70 * ild * couleur;  //ratio 70 rouge 30 bleu pour un effet rosée sur les vagues
  //test autres gestion des couleurs 
  vec4 bleu = vec4(0.0, 0.2, 0.8,0.8);
  vec4 orange = vec4(1.0, 0.5, 0.0, 1.0);

  vec4 tmp = ils * bleu + 0.90 * ild * bleu + 0.10;
  fragColor = tmp;
}
