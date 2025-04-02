#version 330
out vec4 fragColor;
uniform sampler2D tex;
uniform vec4 couleur ;
in vec2 vsoTexCoord;
in float il;

void main() {
  /* on a décidé de mettre du vert à la place du rouge */
  fragColor = il * mix(texture(tex, vsoTexCoord), couleur, 0.5);;
}
