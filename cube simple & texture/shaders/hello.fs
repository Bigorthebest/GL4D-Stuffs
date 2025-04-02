#version 330
out vec4 fragColor;
uniform vec4 couleur;
uniform sampler2D tex;
in  vec2 vsoTexCoord;
in vec4 color ;

void main() {
  //fragColor = vec4(1.0, 1.0, 0.0, 1.0) ; //color ;//color ; // vec4(1.0, 0.0, 0.0, 1.0);//couleur;
  fragColor = (texture(tex, vsoTexCoord) );
}

