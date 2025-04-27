#version 330
in vec4 mpos;
in vec3 mnormal;
in vec2 tcoord;
out vec4 fragColor;

uniform mat4 proj, model, view;

uniform vec4 ldiffus, lspec ;
uniform vec4 sdiffus, sambient, sspeculaire, lambient ;
uniform vec4 lumpos;
uniform float temps ;

float v2random(vec2 uv) {
	//return 0.5;// texture(noise_texture, mod(uv, vec2(1.0))).x;
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main() {
  //gestion de la lumiére
  const vec3 V = vec3(0.0, 0.0, -1.0);
  vec4 vlum = view * lumpos ;
  //const vec4 acouleur = vec4(0.0, 0.5, 0.796, 1.0);
  vec3 Ld = normalize(mpos.xyz - vlum.xyz);
  vec3 N = mnormal;
  //Simulation de normal map 
  vec3 T = vec3(1.0, 0.0, 0.0);
  vec3 B = cross(T, N);
  vec2 wave = (1.0 - length(tcoord) / sqrt(2.0)) * (sin( tcoord * 6.283 - temps * 2) + 1.0) / 2.0;//6.283
  //vec2 wave = ((1.0 - length(tcoord) / sqrt(2.0)) * (sin(vec2(v2random(tcoord)) * 6.283 - temps) + 1.0)) ;//6.283
  //wave = pow(wave,vec2(9.0));
  //wave = smoothstep((sin( tcoord * 6.283 - temps * 2)), 0.8, wave);
  //vec2 wave = (1.0 - length(tcoord) / sqrt(2.0)) * (sin( tcoord * 6.283 - temps) + 1.0) *(sin(vec2(v2random(tcoord)) / 10 - temps) + 1.0) ;
  N = normalize(4.0 * N + (wave.x * T + wave.y * B));
  //Fin simulation normal map 
  float intensite_diffus = clamp(dot(N, -Ld), 0.0, 1.0);
  vec3 R = reflect(Ld, N);
  float intensite_speculaire = pow(clamp(dot(R, -V), 0.0, 1.0), 200.0);
  vec4 diffus = intensite_diffus * ldiffus * sdiffus ;
  vec4 ambient = sambient * lambient ;
  //fragColor = diffus + intensite_speculaire * lspec ;
  fragColor = mix(ambient, diffus, 0.75) + intensite_speculaire * lspec * sspeculaire ;

  /*
  float ild = clamp(dot(N, -Ld), 0.0, 1.0);
  //vec3 R = reflect(Ld, N);
  //R = normalize((transpose(inverse(view)) * vec4(R, 0.0)).xyz);
  float ils = clamp(dot(R, -V), 0.0, 1.0);
  //vec4 tmp = ils * vec4(1.0, 0.0, 0.0, 1.0) + 0.30 * acouleur + 0.70 * ild * couleur;  //ratio 70 rouge 30 bleu pour un effet rosée sur les vagues
  //test autres gestion des couleurs 
  vec4 bleu = vec4(0.0, 0.2, 0.8,0.8);
  vec4 orange = vec4(1.0, 0.5, 0.0, 1.0);

  vec4 tmp = ils * bleu + 0.90 * ild * bleu + 0.10;
  fragColor = tmp;*/
}
