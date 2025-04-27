#version 330
out vec4 fragColor;

uniform vec4 couleur;

void main() {
  fragColor = vec4(couleur); //couleur;
}

