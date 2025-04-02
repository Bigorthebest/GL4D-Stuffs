#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoord;

uniform mat4 proj;
uniform mat4 modview;
uniform int inv;
out vec2 vsoTexCoord;

out vec4 color ;
/* incidence de la lumière pour algorithme Gouraud */
out float il;

void main() {
     //gl_Position = proj * modview * vec4(pos, 1.0);
     //color = vec4(pos,1.0) ;*/
       /* une lumière directionnelle constante */
     const vec3 Ld = vec3(0.0, -0.5, -0.866) ;
     vec4 n = vec4(normal, 0.0);
     n = transpose(inverse(modview)) * n;
     /* le produit scalaire permet de calculer l'incidence de la lumière */
     il = clamp(dot(normalize(n.xyz), -Ld), 0.0, 1.0);
     gl_Position = proj * modview * vec4(pos, 1.0);
     if(inv != 0)
          vsoTexCoord = vec2(texCoord.s, 1.0 - texCoord.t);
     else
          vsoTexCoord = vec2(texCoord.s, texCoord.t);
          //color = vec4(pos,1.0);
}
