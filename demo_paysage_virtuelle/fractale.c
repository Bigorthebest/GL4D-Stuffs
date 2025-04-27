/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>


static void init(void);
static void draw(void);

/* on créé une variable pour stocker l'identifiant du programme GPU */
GLuint _pIdFractale = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un sphere GL4D */
GLuint _quad = 0;

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
void fractale(int state) {
   /* INITIALISEZ VOS VARIABLES */
  /* ... */
  switch(state) {
    case GL4DH_INIT:
      /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
      init();
      return;
    case GL4DH_FREE:
      /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
      return;
    case GL4DH_UPDATE_WITH_AUDIO:
      /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
      return;
    default: /* GL4DH_DRAW */
      /* JOUER L'ANIMATION */
      draw();
      return;
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
  _pIdFractale = gl4duCreateProgram("<vs>shaders/fractale.vs", "<fs>shaders/fractale.fs", NULL);

}

void draw(void) {
  static GLfloat angle = 0.0 ;
  static double t0 = 0.0 ;
  double t = gl4dGetElapsedTime() / 1000.0, dt = (t - t0);
  t0 = t;
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pIdFractale);
  glUniform1f(glGetUniformLocation(_pIdFractale,"angle"),angle) ;
  gl4dgDraw(_quad);
  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);
  angle += dt * M_PI ; 
}

