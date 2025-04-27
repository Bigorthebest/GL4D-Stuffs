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
#include <GL4D/gl4dh.h>


static void init(void);
static void draw(void);
static void keydown(int keycode);

/* on créé une variable pour stocker l'identifiant du programme GPU */
GLuint _pIdShip[2] = {0};
/* on créé une variable pour stocker l'identifiant de la géométrie d'un sphere GL4D */
GLuint _quad_ecran = 0;
//Pour le keydown
float coord = -1.3 ;
//Direction de la fractale 
float mouvement[2] = {0.4,0.8} ;
int compt_dt = 0 ;
int bateau = 0 ;

static int _wW = 600, _wH = 600;

static GLuint _quad_fbo_ship = 0 ;
//id FBO 
static GLuint _fbo_ship = 0;
//Id texture (pour le fbo)
static GLuint _texIdShip[] = { 0, 0 };

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
void burning_ship(int state) {
 switch(state) {
   case GL4DH_INIT:
     /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
     init();
     return;
   case GL4DH_FREE:
    if(_texIdShip[0]){
      glDeleteTextures(2, _texIdShip);
      _texIdShip[0] = _texIdShip[1] = 0;
      }
    if(_fbo_ship){
      glDeleteFramebuffers(1, &_fbo_ship);
    }
     /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
     return;
   case GL4DH_UPDATE_WITH_AUDIO:
     /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
     return;
   default: /* GL4DH_DRAW */
     /* JOUER L'ANIMATION */
     draw();
     gl4duwKeyDownFunc(keydown);
     return;
   }
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
  _quad_fbo_ship = gl4dgGenQuadf();
  /* générer un quad en GL4D */
  _quad_ecran = gl4dgGenQuadf();
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pIdShip[0] = gl4duCreateProgram("<vs>shaders/fractale.vs", "<fs>shaders/burning_ship.fs", NULL);

    //gestion des textures pour le FBO 
    glGenTextures(2, _texIdShip);
    for(int i = 0; i < 2; ++i) {
      glBindTexture(GL_TEXTURE_2D, _texIdShip[i]);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
      glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
      glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _wW, _wH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }
    glBindTexture(GL_TEXTURE_2D, 0);  
  
    glGenFramebuffers(1, &_fbo_ship);

}

void draw(void) {
  static GLfloat angle = 0.0 ;
  static double t0 = 0.0 ;
  double t = gl4dGetElapsedTime() / 1000.0, dt = (t - t0);
  t0 = t;
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glEnable(GL_DEPTH_TEST);

  /* utiliser le programme GPU "_pIdShip" */
  glUseProgram(_pIdShip[0]);
  glUniform1f(glGetUniformLocation(_pIdShip[0],"angle"),angle) ; //"resolution"
  glUniform2f(glGetUniformLocation(_pIdShip[0],"resolution"),_wW,_wH) ;
  glUniform1f(glGetUniformLocation(_pIdShip[0],"temps"),t) ; //"resolution"
  glUniform1f(glGetUniformLocation(_pIdShip[0],"zoom"),coord) ;
  glUniform2f(glGetUniformLocation(_pIdShip[0],"move"),mouvement[0],mouvement[1]) ; //"direction"
  gl4dgDraw(_quad_ecran);
  
  if(compt_dt > 120){
    if(bateau == 0){
      compt_dt = 0 ;
      coord = -1.3 ;
      mouvement[0] = 0.4 ; 
      mouvement[1] = 0.8 ;
      bateau = 1 ;
    }
    else{
      compt_dt = 0 ;
      coord = -0.1 ;
      mouvement[0] = 0.4 ;
      mouvement[1] = 17.400030 ;
      bateau = 0;
    }
  }
  else{
    compt_dt += 1 ;
  }

  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);
  angle += dt * M_PI ; 
}

