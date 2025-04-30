/*!\file window.c
 * \brief GL4Dummies, exemple de physique
 * \author Farès BELHADJ, amsi@up8.edu
 * \date jannuary 20, 2025
 */

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>

static void init(void);
static void draw(void);
static void quit(void);
static void keydown(int keycode);

/* on créé une variable pour stocker l'identifiant du programme GPU */
GLuint _pId = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un sphere GL4D */
GLuint _quad = 0;
//Pour le keydown
float coord = -1.3 ;
//Direction de la fractale 
float mouvement[2] = {0.4,0.8} ;
int compt = 0 ;
int bateau = 0 ;

static int _ww = 600, _wh = 600;

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "SuperFractaleProg", /* titre */
			 10, 10, _ww, _wh, /* x,y, largeur, heuteur */
			 GL4DW_SHOWN) /* état visible */) {
    /* ici si échec de la création souvent lié à un problème d'absence
     * de contexte graphique ou d'impossibilité d'ouverture d'un
     * contexte OpenGL (au moins 3.2) */
    return 1;
  }
  /* appeler init pour initialiser des paramètres GL et GL4D */
  init();
  /* placer quit comme fonction à appeler au moment du exit */
  atexit(quit);
  gl4duwKeyDownFunc(keydown);
  /* placer draw comme fonction à appeler pour dessiner chaque frame */
  gl4duwDisplayFunc(draw);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}

static void keydown(int keycode) {
  switch(keycode) {
  case SDLK_z:
    coord += 0.1 ;
    break;
  case SDLK_UP :
    mouvement[0] += 0.1 ;
    break ;
  case SDLK_DOWN :
    mouvement[0] -= 0.1 ;
    break ;
  case SDLK_LEFT :
    mouvement[1] += 0.1 ;
    break ;
  case SDLK_RIGHT :
    mouvement[1] -= 0.1 ;
    break ;
  case SDLK_s :
    coord -= 0.1 ;
    break ;
  case SDLK_SPACE :
    fprintf(stderr,"Zoom : %f | Coord %f,%f \n",coord,mouvement[0],mouvement[1]);
    break ;
  case SDLK_ESCAPE:
  case 'q':
    exit(0);
  default:
    break;
  }
}

/* initialise des paramètres GL et GL4D */
void init(void) {
  /* activer la synchronisation verticale */
  SDL_GL_SetSwapInterval(1);
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.0f, 0.0f, 0.5f, 1.0f);
  /* générer un quad en GL4D */
  _quad = gl4dgGenQuadf();
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/id.vs", "<fs>shaders/fractale.fs", NULL);

}

void draw(void) {
  static GLfloat angle = 0.0 ;
  static double t0 = 0.0 ;
  double t = gl4dGetElapsedTime() / 1000.0, dt = (t - t0);
  t0 = t;
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pId);
  glUniform1f(glGetUniformLocation(_pId,"angle"),angle) ; //"resolution"
  glUniform2f(glGetUniformLocation(_pId,"resolution"),_ww,_wh) ;
  glUniform1f(glGetUniformLocation(_pId,"temps"),t) ; //"resolution"
  glUniform1f(glGetUniformLocation(_pId,"zoom"),coord) ;
  glUniform2f(glGetUniformLocation(_pId,"move"),mouvement[0],mouvement[1]) ; //"direction"

  
  if(compt > 120){
    
    /*
    if(bateau == 0){
      compt = 0 ;
      coord = -1.3 ;
      mouvement[0] = 0.4 ; 
      mouvement[1] = 0.8 ;
      bateau = 1 ;
    }
    else{
      compt = 0 ;
      coord = -0.1 ;
      mouvement[0] = 0.4 ;
      mouvement[1] = 17.400030 ;
      bateau = 0;
    }*/
  }
  else{
    compt += 1 ;
  }

  
  gl4dgDraw(_quad);
  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);
  angle += dt * M_PI ; 
}

/* appelée lors du exit */
void quit(void) {
  gl4duClean(GL4DU_ALL);
}

