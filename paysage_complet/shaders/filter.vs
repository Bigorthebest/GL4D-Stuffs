#version 330 core

layout (location = 0) in vec2 vsiPosition;
layout (location = 1) in vec3 vsiNormal;
layout (location = 2) in vec2 vsiTexCoord;

out vec2 UV;
out vec2 vsoTexCoord;

void main(void) {
  /* gl_Position, nom réservé produisant un sommet GL */
  /* l'entrée vsiPosition est complétée en vecteur 4D (x, y, z, w) où
     z = 0.0 et w = 1.0 */
  gl_Position = vec4(vsiPosition, 0.0, 1.0);
  vsoTexCoord = vec2(vsiTexCoord.x, vsiTexCoord.y);
}