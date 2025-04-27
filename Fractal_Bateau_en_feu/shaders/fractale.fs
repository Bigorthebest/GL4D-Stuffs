#version 330
out vec4 fragColor;
in vec2 tc;

uniform float angle ; 
uniform vec2 resolution ;
uniform float temps ; 
uniform int zoom ;
uniform float gauche ; 
uniform float droite ;
uniform float coord ;
uniform vec2 move ;


void main(){
  //float x = (tc.x * -2.5 - 1.0) ;
  //float y = (tc.y * -1.0 - 1.0) ;
  //float x = tc.x ;
  //float y = tc.y ;
  //vec2 z = tc ;
  //vec2 n = (vec2(tc.x, 1.0 - tc.y) * (2.5 / temps) - (vec2(0.75, 1.25) / temps)) ; //gére le zoom de la fractale 
  vec2 n ;
  n = - (vec2(tc.x, 1.0 - tc.y) * (2.5 * coord) - (vec2(0.75 + move.y, 1.25 + move.x) * coord)) ;
  /*
  if (zoom == 0 ){
    n = (vec2(tc.x, 1.0 - tc.y) * (2.5) - (vec2(0.75, 1.25))) ;
  }
  if (zoom == 1){
    n = (vec2(tc.x, 1.0 - tc.y) * (2.5 * temps) - (vec2(0.75, 1.25) * temps)) ;
  }
  if (zoom == 2){
    n = (vec2(tc.x, 1.0 - tc.y) * (2.5 / temps) - (vec2(0.75, 1.25) / temps)) ;
  }*/
  //vec2 z = n ;
  //vec2 c = vec2(cos(angle) + 0.25,sin(angle) + 0.5) ;
  vec2 ecran = vec2(0.25,0.5) ;
  //float d = 0.0 ;
  int iteration = 0 , max_iteration = 64 ;

  float aspect = resolution.x / resolution.y;
  ecran.x *= aspect;
  ecran.y *= aspect;

  vec2 c = ecran.x + (ecran.y - ecran.x) * n;

  /*
  vec2 uv = gl_FragCoord.xy / resolution;

  vec2 mini = vec2(-2.5, -1.0);
  vec2 maxi = vec2( 1.0,  1.0);
    
  float zoom = 0.695 + 0.405 * cos(0.1 * temps);  
  //float zoom = 1.0 + cos(temps) ;
  vec2 z = mini + (maxi - mini) * uv;
  z = z * zoom ;
  float x = z.x ; 
  float y = z.y ;
  int iteration = 0 ;
  int max_iteration = 120 ; */
  /*
  while(z.x * z.x + z.y * z.y < 4.0 && iteration < max_iteration){
    float xtemp = z.x * z.x - z.y * z.y + c.x ; 
    z.x = abs(z.x) * abs(z.y) + c.y ;
    //abs(2.0 * z.x * z.y) + c.y ; 
    //abs(z.x) + c.y  * abs(z.y) ;
    z.y = xtemp ; 
    iteration += 1 ;
  }*/

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
