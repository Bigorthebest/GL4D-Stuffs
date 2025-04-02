/*!\file light.fs
 * \brief fragment shader progressif sur l'éclairage.
 */
#version 330

in float intensite_diffus ;

uniform vec4 sdiffus ;
uniform vec4 sambient ; 

/* sortie du frament shader : une couleur */
out vec4 fragColor;

void main(void) {
  fragColor = 0.25 * sambient + 0.75 * intensite_diffus * sdiffus; //20 % ambiant 80% de diffue, augmanter l'ambiant pour plus de rouge
}
