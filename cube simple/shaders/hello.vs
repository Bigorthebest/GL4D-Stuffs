#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 modview;

out vec4 color ;

void main() {
     gl_Position = proj * modview * vec4(pos, 1.0) ;
     //vec4(pos, 1.0) ; //proj * modview * vec4(pos, 1.0) ;
	//if (texCoord.y > 0.90){
		//color = vec4(0.64, 0.77, 0.22, 1.0);
	//}
	//else{
     color = ( (vec4(pos,1.0) + vec4(1)) / 2.0) * vec4(0.64, 0.77, 0.22, 1.0);
	//}
}
