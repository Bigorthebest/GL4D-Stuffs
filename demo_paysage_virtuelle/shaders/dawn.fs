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

void main() {
  //gestion de la lumiére
  const vec3 V = vec3(0.0, 0.0, -1.0);
  vec4 vlum = view * lumpos ;
  vec3 Ld = normalize(mpos.xyz - vlum.xyz);
  vec3 N = mnormal;
  //Simulation de normal map 
  vec3 T = vec3(1.0, 0.0, 0.0);
  vec3 B = cross(T, N);
  vec2 wave = (1.0 - length(tcoord) / sqrt(2.0)) * (sin( tcoord * 6.283 - temps * 2) + 1.0) / 2.0;
  N = normalize(4.0 * N + (wave.x * T + wave.y * B));
  //Fin simulation normal map 
  float intensite_diffus = clamp(dot(N, -Ld), 0.0, 1.0);
  vec3 R = reflect(Ld, N);
  float intensite_speculaire = pow(clamp(dot(R, -V), 0.0, 1.0), 200.0);
  vec4 diffus = intensite_diffus * ldiffus * sdiffus ;
  vec4 ambient = sambient * lambient ;
  fragColor = mix(ambient, diffus, 0.75) + intensite_speculaire * lspec * sspeculaire ;
}
