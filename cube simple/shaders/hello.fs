#version 330
out vec4 fragColor;
uniform vec4 couleur;

in vec4 color ;

void main() {
  fragColor = color ;//color ; // vec4(1.0, 0.0, 0.0, 1.0);//couleur;
}

