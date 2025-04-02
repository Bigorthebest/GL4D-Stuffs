
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dp.h>

#define WW 800
#define WH 600


static void init(void);
static void draw(void);
static void quit(void);

/* on créé une variable pour stocker l'identifiant du programme GPU */
GLuint _pId = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un cube GL4D */
GLuint _cube = 0;
/* on se créé 3 identifiants de texture */
GLuint _texId[1] = { 0 };

/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
int main(int argc, char ** argv) {
  /* tentative de création d'une fenêtre pour GL4Dummies */
  if(!gl4duwCreateWindow(argc, argv, /* args du programme */
			 "GL4Dummies' Hello World", /* titre */
			 10, 10, WW, WH, /* x,y, largeur, heuteur */
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
  /* activer le BACKFACE_CULLING */
  glCullFace(GL_BACK);
  glEnable(GL_CULL_FACE);
  /* on active le test de profondeur */
  glEnable(GL_DEPTH_TEST);
  /* set la couleur d'effacement OpenGL */
  glClearColor(0.7f, 0.7f, 0.0f, 1.0f);
  /* générer un cube en GL4D */
  _cube = gl4dgGenCubef();
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  /* créer dans GL4D une matrice qui s'appelle model ; matrice de
     modélisation qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "model");
  /* créer dans GL4D une matrice qui s'appelle view ; matrice de
     vue qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "view");
  /* créer dans GL4D une matrice qui s'appelle proj ; matrice de
     projection qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "proj");

  glGenTextures(1, _texId);

  /* on bind la texture _texId[2] */
  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  /* paramétrage de l'interpolation pour rapetisser ou agrandir la texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  /* on récupère une image depuis SDL/BMP */
  
  SDL_Surface * s = SDL_LoadBMP("A.bmp");
  assert(s);
  /* je sais que mon images est RGBA */
  assert(s->format->BytesPerPixel == 3);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
  SDL_FreeSurface(s);

  /* on dé-bind la texture 2D */
  glBindTexture(GL_TEXTURE_2D, 0);  
}


void draw(void) {
  /* une variable pour stocker un ange qui incrémente */
  static float a = 0;
  /* set une couleur d'effacement random pour OpenGL */
  //glClearColor(gl4dmURand(), gl4dmURand(), gl4dmURand(), 1.0);
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pId);

/* on précise qu'on utilise l'étage de texture 0 */
  glActiveTexture(GL_TEXTURE0);
  /* on bind la texture _texId[0] */
  //glBindTexture(GL_TEXTURE_2D, _texId[0]);
  /* on donne un nom à notre texture dans GLSL et on y met 0 car c'est l'étage où elle se trouve */
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);


  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice model */
  gl4duBindMatrix("model");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* composer (multiplication à droite) avec une translation dans les z négatifs (-5) */ 
  gl4duTranslatef(-2, 0, -5);
  /* composer (multiplication à droite) avec une rotation d'angle a et
     d'axe (autour de l'axe) <0, 1, 0> */
  gl4duRotatef(a, 0, 1, 0);
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  gl4duSendMatrices();
  /* set la variable "uniform" couleur pour mettre du rouge */
  glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 0.0f, 0.0f, 1.0f);
  /* demander le dessin d'un objet GL4D */
  gl4dgDraw(_cube);
  

  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);
  /* augmenter l'ange a de 1 */
  a += 1;
}

/* appelée lors du exit */
void quit(void) {
  if(_texId[0]) {
    /* on supprime la texture côté OpenGL */
    glDeleteTextures(1, _texId);
    _texId[0] = 0;
  }
  /* nettoyer (libérer) tout objet créé avec GL4D */
  gl4duClean(GL4DU_ALL);
}
