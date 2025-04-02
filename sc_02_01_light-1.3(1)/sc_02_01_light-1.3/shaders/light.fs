/*!\file light.fs
 * \brief fragment shader progressif sur l'éclairage. */
#version 330
/* caractéristiques diffus et ambient de la surface et des lumières */
uniform vec4 sdiffus, sambient, sspeculaire, l0diffus, l0speculaire, l1diffus, l1speculaire, lambient;
uniform mat4 projectionMatrix, viewMatrix, modelMatrix;
uniform vec4 Lp0, Lp1;
in vec4 vmPos;
in vec3 vmNormal;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
  const vec3 vue = vec3(0.0, 0.0, -1.0);
  vec4 vLp0 = viewMatrix * Lp0;
  vec3 Ld0  = normalize((vmPos - vLp0).xyz);
  vec4 vLp1 = viewMatrix * Lp1;
  vec3 Ld1  = normalize((vmPos - vLp1).xyz);
  vec3 n = normalize(vmNormal);
  float intensite_diffus0 = clamp(dot(n, -Ld0), 0.0, 1.0);
  float intensite_diffus1 = clamp(dot(n, -Ld1), 0.0, 1.0);
  vec3 R0 = reflect(Ld0, n);
  vec3 R1 = reflect(Ld1, n);
  float intensite_speculaire0 = pow(clamp(dot(R0, -vue), 0.0, 1.0), 200.0);
  float intensite_speculaire1 = pow(clamp(dot(R1, -vue), 0.0, 1.0), 200.0);
  vec4 diffus = intensite_diffus0 * sdiffus * l0diffus + intensite_diffus1 * sdiffus * l1diffus;
  vec4 ambient = sambient * lambient;
  fragColor = mix(ambient, diffus, 0.75) + intensite_speculaire0 * l0speculaire * sspeculaire + intensite_speculaire1 * l1speculaire * sspeculaire;
}
