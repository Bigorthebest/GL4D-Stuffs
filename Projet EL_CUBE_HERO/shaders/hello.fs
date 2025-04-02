#version 330
uniform sampler2D tex;
in  vec2 vsoTexCoord;
out vec4 fragColor;
uniform vec4 couleur; 

in vec4 color ;
in float il;
uniform int inv;

void main() {
  //if(inv != 0)
    //vec4 c = texture(tex, vsoTexCoord);
    //fragColor = vec4(c.rgb, length(c.rgb) > 0.0 ? 1.0 : 0.0);//color ;//vec4(1.0, 0.0, 1.0, 1.0);//color ;//vec4(1.0, 0.0, 0.0, 0.1); // vec4(1.0, 0.0, 0.0, 1.0);//couleur;
  //else 
    //fragColor = vec4(1.0, 0.0, 1.0, 1.0) ;
    //fragColor =  mix(texture(tex, vsoTexCoord),vec4(1.0, 0.0, 1.0, 1.0) , 0.5);; //cette ligne mets un filtre rose sur le texte / texture 
    fragColor =  il * mix(texture(tex, vsoTexCoord),couleur, 0.5);
}



