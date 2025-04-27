#version 330
uniform sampler2D myTexture;
in  vec2 vsoTexCoord;
out vec4 fragColor;
uniform vec4 color ;

void main(void) {
  fragColor = texture(myTexture, vec2(vsoTexCoord.x, vsoTexCoord.y));
}
