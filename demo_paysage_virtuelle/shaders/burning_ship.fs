#version 330
out vec4 fragColor;
in vec2 tc;

uniform float angle ; 
uniform vec2 resolution ;
uniform float temps ; 
uniform float gauche ; 
uniform float droite ;
uniform float coord ;
uniform vec2 move ;

void main(){
  vec2 n = -(vec2(tc.x, 1.0 - tc.y) - 0.5) * 2.5 * coord;

  vec2 pos_depart = vec2(-0.3, -0.6);
  vec2 pos_arriver = vec2(-1.78003, -0.05);

  float zoom = cos(0.1 * temps);
  zoom = pow(zoom,2.0);
  zoom = max(zoom, 0.05); //On stop à la bonne pos

  vec2 pos = mix(pos_depart, pos_arriver, 1.0 - zoom);
  n = n * zoom + pos;

  int iteration = 0 , max_iteration = 64 ;
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

