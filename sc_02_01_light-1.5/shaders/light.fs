/*!\file light.fs
 * \brief fragment shader progressif sur l'éclairage. */
#version 330
/* caractéristiques diffus et ambient de la surface et des lumières */
uniform vec4 sdiffus, sambient, sspeculaire, l0diffus, l0speculaire, l1diffus, l1speculaire, lambient;
uniform mat4 projectionMatrix, viewMatrix, modelMatrix;
uniform vec4 Lp0, Lp1;
uniform float temps;

uniform sampler2D tex;
uniform sampler2D tex_nm;

uniform vec2 dim;

in vec4 vmPos;
in vec3 vmNormal;
in vec2 tc;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
  const vec3 vue = vec3(0.0, 0.0, -1.0);
  vec4 vLp0 = viewMatrix * Lp0;
  vec3 Ld0  = normalize((vmPos - vLp0).xyz);
  vec4 vLp1 = viewMatrix * Lp1;
  vec3 Ld1  = normalize((vmPos - vLp1).xyz);

  /* AVANT CHARGEMENT DE TEXTURE AVEC SA NORMAL MAP */
  /* vec3 n = normalize(vmNormal); */
  /* /\* simulation de normal map *\/ */
  /* vec3 T = vec3(1.0, 0.0, 0.0); */
  /* vec3 B = cross(T, n); */
  /* /\* fait en cours, donne un rond de vagues *\/ */
  /* /\* vec2 co = 2.0 * (tc - vec2(0.5)); *\/ */
  /* /\* float d = length(co) / 1.4142; *\/ */
  /* /\* float wave = (1.0 - d) * (cos(10.0 * (d - temps) * 6.283) + 1.0) / 2.0; *\/ */
  /* /\* n = normalize(n + wave * T + wave * B); *\/ */
  /* /\* autre effet *\/ */
  /* vec2 wave = (1.0 - length(tc) / sqrt(2.0)) * (cos(15.0 * tc * 6.283) + 1.0) / 2.0; */
  /* n = normalize(4.0 * n + wave.x * T + wave.y * B); */
  /* /\* fin simulation *\/ */

  vec3 n = normalize((transpose(inverse(viewMatrix * modelMatrix)) * vec4(texture(tex_nm, tc).rgb, 0.0)).xyz);

  
  float intensite_diffus0 = clamp(dot(n, -Ld0), 0.0, 1.0);
  float intensite_diffus1 = clamp(dot(n, -Ld1), 0.0, 1.0);
  vec3 R0 = reflect(Ld0, n);
  vec3 R1 = reflect(Ld1, n);
  float intensite_speculaire0 = pow(clamp(dot(R0, -vue), 0.0, 1.0), 200.0);
  float intensite_speculaire1 = pow(clamp(dot(R1, -vue), 0.0, 1.0), 200.0);
  vec4 tdiffus = texture(tex, tc);
  /* vec4 diffus = intensite_diffus0 * sdiffus * l0diffus + intensite_diffus1 * sdiffus * l1diffus; */
  vec4 diffus = intensite_diffus0 * tdiffus * l0diffus + intensite_diffus1 * tdiffus * l1diffus;
  vec4 ambient = sambient * lambient;
  if(gl_FragCoord.x < 0.5 * dim.x)
    fragColor = mix(ambient, diffus, 0.75) + intensite_speculaire0 * l0speculaire * sspeculaire + intensite_speculaire1 * l1speculaire * sspeculaire;
  else
    fragColor = tdiffus;
}
