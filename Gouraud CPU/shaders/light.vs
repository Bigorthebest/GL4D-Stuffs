/*!\file light.vs
 * \brief vertex shader progressif sur l'éclairage */
#version 330

layout (location = 0) in vec3 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

uniform mat4 projectionMatrix, viewMatrix, modelMatrix;

uniform vec4 Lp ; //Envoyer depuis le CPU 

out float intensite_diffus ;

void main(void) {
  vec4 vmPos = viewMatrix * modelMatrix * vec4(vsiPosition, 1.0);
  vec3 n = normalize(transpose(inverse(viewMatrix * modelMatrix)) * vec4(vsiNormal, 0.0)).xyz;
  vec4 vLp = viewMatrix * Lp ; //Position de la lumiére (on lui ajoute la matrice view) 
  vec3 Ld = normalize((vmPos  - vLp ).xyz) ; //.xyz pour prendre seulement les 3 premiére valeur 
  intensite_diffus = clamp(dot(n , -Ld), 0.0, 1.0) ;
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
   * w = 1.0 ; elle est multipliée à gauche par une matrice de
   * modélisation puis vue puis projection (lire de droite à gauche à
   * partir du sommet) */
  gl_Position = projectionMatrix * vmPos ;
}
