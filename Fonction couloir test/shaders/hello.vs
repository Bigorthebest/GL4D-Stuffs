#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 modview;
out vec2 vsoTexCoord;
out vec4 color ;

void main() {
     vsoTexCoord = vec2(texCoord.s, - texCoord.t); // je met le - pour inverser la texture (mettre le  ? du bas vers le haut 
     gl_Position = proj * modview * vec4(pos, 1.0) ;
}
