/*!\file window.c
 * \brief GL4Dummies, exemple simple 3D avec GL4Dummies
 * \author Farès BELHADJ, amsi@up8.edu
 * \date February 03 2022
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

//0 -2 -5
/* on créé une variable pour stocker l'identifiant du programme GPU */
GLuint _pId = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un cube GL4D */
GLuint _cube = 0;
// Variable d'angle 
float angle = 0 ;

GLuint _texId[1] = { 0 };


/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "Je suis une fenetre magique !!!", /* titre */
			 10, 10, 800, 600, /* x,y, largeur, heuteur */
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
  /* placer draw comme fonction à appeler pour dessiner chaque frame */
  //gl4duwPassiveMotionFunc(ecranX,ecranY) ;
  gl4duwDisplayFunc(draw);
  /* boucle infinie pour éviter que le programme ne s'arrête et ferme
   * la fenêtre immédiatement */
  gl4duwMainLoop();
  return 0;
}

/* initialise des paramètres GL et GL4D */
void init(void) {
  /* activer la synchronisation verticale */
  SDL_GL_SetSwapInterval(1);
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.0f, 0.0f, 0.5f, 1.0f);

  
  /* générer un cube en GL4D */
  _cube = gl4dgGenCubef();
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  /* créer dans GL4D une matrice qui s'appelle modview ; matrice de
     modélisation et vue qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "modview");
  /* créer dans GL4D une matrice qui s'appelle proj ; matrice de
     projection qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "proj");
  /* binder (mettre au premier plan, "en courante" ou "en active") la matrice proj */
  gl4duBindMatrix("proj");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* combiner la matrice courante avec une matrice de projection en
     perspective. Voir le support de cours pour les six paramètres :
     left, right, bottom, top, near, far */
  gl4duFrustumf(-1, 1, -0.75, 0.75, 1, 1000); //En gros la camera (ou plutot ça projection su rl'ecran, elle peut deformer les objets)

}

void draw(void) {
  /* une variable pour stocker un ange qui incrémente */
  //static float a = 0;
  /* set une couleur d'effacement random pour OpenGL */
  //glClearColor(gl4dmURand(), gl4dmURand(), gl4dmURand(), 1.0);
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pId);
  //gl4duwKeyDownFunc(handleKeyPress);
  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice modview */

    
  gl4duBindMatrix("modview");
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, 0);
  glUniform1i(glGetUniformLocation(_pId, "screen_texture"), 0);
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf(); //Cela veut dire recommence à ton état initial très important pour reset à chaque boucle
  gl4dgDraw(_cube) ;
  gl4duTranslatef(0,0,-5);
  gl4duRotatef(angle,1,1,0);
  gl4duSendMatrices();
  glUniform1f(glGetUniformLocation(_pId, "time"), (float)gl4dGetTime());
  //glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 0.0f, 0.0f, 1.0f);
  glUseProgram(0);
  angle += 0.7 ;
}


/* appelée lors du exit */
void quit(void) {
  /* nettoyer (libérer) tout objet créé avec GL4D */
  gl4duClean(GL4DU_ALL);
}

