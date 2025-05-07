#version 330
uniform float dt;
uniform sampler2D tex0;
uniform sampler2D tex1;
in  vec2 vsoTexCoord;
out vec4 fragColor;
uniform float bruit ;

float v2random(vec2 uv) {
	//return 0.5;// texture(noise_texture, mod(uv, vec2(1.0))).x;
	return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);
}

void main(void) {
  float r = v2random(uvn) ;
  if(r * 2 > bruit){
    fragColor =  fragColor = texture(tex1, vsoTexCoord);
  }
  else{
    fragColor = texture(tex1, vsoTexCoord);
  }
}
