#version 330
out vec4 fragColor;
in vec2 tc;

uniform float angle ; 

/*
void main() {
  vec2 n = (tc * 2.5 - vec2(1.25)) ; //gére le zoom de la fractale 
  vec2 z = n ;
  vec2 c = vec2(cos(angle) + 0.25,sin(angle) + 0.5) ;
  float d = 0.0 ;
  int i , imax = 15 ;
  for(i = 0 ;i < 20 ; ++i){
    vec2 oz = vec2(z) ;
    z.x = oz.x * oz.x - oz.y * oz.y + c.x; 
    z.y = 2.0 * oz.x * oz.y + c.y; 
    d = length(z) ; d = d * d ;
    if (d > 4.0){//Juste d * d pour un effet stylé 
      break ; 
    }
  }
  if(i < imax){
      fragColor = vec4(float(i) / float(imax));

  }
  else{
      fragColor = vec4(1.0);
  }
}
*/

void main() {
  vec2 n = (tc * 4.0 - vec2(2.0)) ; //gére le zoom de la fractale 
  vec2 z = n ;
  vec2 c = vec2(cos(angle) + 0.25,sin(angle) + 0.5) ;
  float d = 0.0 ;
  int i , imax = 15 ;
  for(i = 0 ;i < 20 ; ++i){
    vec2 oz = vec2(z) ;
    z.x = oz.x * oz.x - oz.y * oz.y + c.x; 
    z.y = 2.0 * oz.x * oz.y + c.y; 
    d = length(z) ; d = d * d ;
    if (d * d > 4.0){ 
      break ; 
    }
  }
  if(d < 4.0){
      fragColor = vec4(c,z.x,1.0);//vec4(float(i) / float(imax));

  }
  else{
      fragColor = vec4(1.0);
  }
}

