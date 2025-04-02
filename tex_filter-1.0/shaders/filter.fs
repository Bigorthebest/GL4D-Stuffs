#version 330
in  vec2 vsoTexCoord;
/* sortie du frament shader : une couleur */
out vec4 fragColor;

uniform sampler2D tex;

void main(void) {
  //int y = int(vsoTexCoord.y * 300.0);
  //if(y % 4 != 0)
  //  fragColor = texture(tex, vsoTexCoord);
  //else
  //  fragColor = vec4(0.5);
  fragColor = texture(tex, vsoTexCoord);
}
