#version 330
out vec4 fragColor;
in vec2 tc;

uniform float angle ; 
uniform vec2 resolution ;
uniform float temps ; 
uniform float zoom ;
uniform vec2 move ;


void main(){
  //vec2 n = - (vec2(tc.x, 1.0 - tc.y) * (2.5 * zoom) - (vec2(0.75 + move.y, 1.25 + move.x) * zoom)) ;
  /*
  float modifY = 17.400030 ;
  float my = 0.1 ;
  my += temps;  
  if (my > modifY) {
    my = modifY;
  }*/
  vec2 n = - (vec2(tc.x, 1.0 - tc.y) * (2.5 * zoom) - (vec2(0.75 + move.y, 1.25 + move.x) * zoom)) ;

  int iteration = 0 , max_iteration = 100 ;
  vec2 z = vec2(0.0);

  while(iteration < max_iteration){
    float xtemp = z.x * z.x - z.y * z.y + n.x ;
    z.y =  2.0 * abs(z.x * z.y) + n.y ;
    z.x = xtemp ;
    iteration += 1 ;
    if (length(z) > 4.0)
          break;
  }
    
  float norm = float(iteration) / float(max_iteration);
  if(iteration == max_iteration){
    fragColor = vec4(1.0,1.0,0.0,1.0) ; //intérieur
  }
  else {
    fragColor = pow(norm, 0.6) * vec4(1.0,0.2,0.0,1.0); //extrérieur
  }
}
