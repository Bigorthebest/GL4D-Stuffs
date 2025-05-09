/*!\file window.c
 * \brief GL4Dummies, exemple progressif d'éclairage
 * \author Farès BELHADJ, amsi@up8.edu
 * \date February 02 2025
 */

#include <GL4D/gl4du.h>
#include <GL4D/gl4duw_SDL2.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void draw(void);
static void resize(int w, int h);
static void quit(void);

/*!\brief largeur et hauteur de la fenêtre */
static int _ww = 800, _wh = 600;
/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId = 0;
/*!\brief identifiant pour une géométrie GL4D */
static GLuint _cubeId = 0;

/*!\brief créé la fenêtre d'affichage, initialise GL et les données,
 * affecte les fonctions d'événements et lance la boucle principale
 * d'affichage.
 */
int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, _ww, _wh, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwResizeFunc(resize);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}
/*!\brief initialise les paramètres OpenGL et les données. */
void init(void) {
  /* Création du programme shader (voir le dossier shader) */
  _pId = gl4duCreateProgram("<vs>shaders/light.vs", "<fs>shaders/light.fs", NULL);
  /* Créer un cube */
  _cubeId = gl4dgGenCubef();
  /* Set de la couleur (RGBA) d'effacement OpenGL */
  glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
  /* activation du test de profondeur afin de prendre en compte la
   * notion de devant-derrière. */
  glEnable(GL_DEPTH_TEST);
  /* Création des matrices GL4Dummies, une pour la projection, une
   * pour la modélisation et une pour la vue */
  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelMatrix");
  gl4duGenMatrix(GL_FLOAT, "viewMatrix");
  resize(_ww, _wh);
}
/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
void draw(void) {
  static GLfloat angle = 0.0f;
  static double t0 = 0.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = (t - t0);
  t0 = t;
  /* effacement du buffer de couleur, nous rajoutons aussi le buffer
   * de profondeur afin de bien rendre les fragments qui sont devant
   * au dessus de ceux qui sont derrière. Commenter le
   * "|GL_DEPTH_BUFFER_BIT" pour voir la différence. Nous avons ajouté
   * l'activation du test de profondeur dans la fonction init via
   * l'appel glEnable(GL_DEPTH_TEST). */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* activation du programme _pId */
  glUseProgram(_pId);
  /* lier (mettre en avant ou "courante") la matrice vue créée dans
   * init */
  gl4duBindMatrix("viewMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* Composer la matrice vue courante en simulant une "caméra" à
   * l'aide de la fonction LookAt(xyz_position_cam,
   * xyz_ou_elle_regarde, xyz_son_vecteur_haut) */
  gl4duLookAtf(0.0f, 3.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  //gl4duLookAtf(0.0f, 0.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  /* lier (mettre en avant ou "courante") la matrice modèle créée dans
   * init */
  gl4duBindMatrix("modelMatrix");
  /* Charger la matrice identité dans la matrice courante (liée) */
  gl4duLoadIdentityf();
  /* composer la matrice modèle avec une rotation */
  gl4duRotatef(angle, 0.0f, 1.0f, 0.0f);
  /* Envoyer, au shader courant, toutes les matrices connues dans
   * GL4Dummies, ici on intègre pas la rotation qui vient après */
  gl4duSendMatrices();

  glUniform1f(glGetUniformLocation(_pId, "temps"), t / 1000.0f);

  /* lumière 0 et ses caractéristiques */
  glUniform4f(glGetUniformLocation(_pId, "Lp0"), -1.3f, -2.0f, 3.5f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "l0diffus"), 0.5f, 0.5f, 1.0f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "l0speculaire"), 0.8f, 0.8f, 1.0f, 1.0f);

  /* lumière 1 et ses caractéristiques */
  glUniform4f(glGetUniformLocation(_pId, "Lp1"), 0.0f, sin(t) * 8.0f, -3.0f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "l1diffus"), 1.0f, 0.1f, 0.1f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "l1speculaire"), 1.0f, 1.0f, 0.0f, 1.0f);

  /* lumière ambiente de toute la scène */
  glUniform4f(glGetUniformLocation(_pId, "lambient"), 1.0f, 1.0f, 0.4f, 1.0f);

  glUniform4f(glGetUniformLocation(_pId, "sambient"), 0.85f, 1.0f, 0.85f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "sdiffus"), 1.0f, 0.6f, 0.6f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId, "sspeculaire"), 0.2f, 0.2f, 0.2f, 1.0f);
  
  /* dessiner la géométrie */
  gl4dgDraw(_cubeId);
  /* désactiver le programme shader */
  glUseProgram(0);
  /* un demi-tour par seconde */
  angle += 18.0f * dt;
}
/*!\brief appelée au moment du resize de la fenêtre.*/
void resize(int w, int h) {
  GLfloat ratio = h / (GLfloat)w;
  _ww = w; _wh = h;
  /* on active la matrice de projection créée précédemment */
  gl4duBindMatrix("projectionMatrix");
  /* la matrice en cours reçoit identité (matrice avec des 1 sur la
   * diagonale et que des 0 sur le reste) */
  gl4duLoadIdentityf();
  /* calcul du ratio écran */
  /* on multiplie la matrice en cours par une matrice de projection
   * orthographique ou perspective */
  /* décommenter pour orthographique gl4duOrthof(-1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 100.0f); */
  gl4duFrustumf(-1.0f, 1.0f, -1.0f * ratio, 1.0f * ratio, 2.0f, 100.0f);
  /* dans quelle partie de l'écran on effectue le rendu */
  glViewport(0, 0, _ww, _wh);
}
/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés.*/
void quit(void) {
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  gl4duClean(GL4DU_ALL);
}
