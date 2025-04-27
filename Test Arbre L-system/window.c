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
#include "L-systemes.h"

static void init(void);
static void draw(void);
static void quit(void);
void drawL_system(l_system ls);
void drawTronc(float x,float  y,float z,float angle);
void drawFeuille(float x,float y,float z,float angle);
void drawLine3d(float x1, float y1, float z1, float x2, float y2, float z2, float thickness, float r, float g, float b);
/* on créé une variable pour stocker l'identifiant du programme GPU */
GLuint _pId = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un cube GL4D */
GLuint _cube = 0;
//Pareil pour un cone 
GLuint _cone = 0 ;
// Variable d'angle 
float angle = 0 ;
//Variable max pour les test de l systeme 
int max = 1 ; 
//Variable pour les tests d'angle de caméra 
static float anglevuZ = 2.0f ;
static float anglevuY = 0.0f ;

//Test vao 
static GLuint _vao = 0;
static GLuint _buffer[2] = { 0 };


//Fonction pour l'interaction clavier
void handleKeyPress(SDL_Keycode key) {
  switch (key) {
      case SDLK_LEFT:
          anglevuZ += 1.0f ;
          break ;
      case SDLK_RIGHT:
          anglevuZ -= 1.0f ;
          break ;
      case SDLK_UP:
          anglevuY += 1.0f ;
          break ;
      case SDLK_DOWN:
          anglevuY -= 1.0f ;
          break ;
  }
}

//OUi


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
  //Le cone 
  _cone = gl4dgGenFanConef(10,10);
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
  gl4duLookAtf(0.0f, 15.0f, 0.0f, 0.0f, -5.0f, 0.0f, 0.0f, anglevuY, anglevuZ);

  glGenVertexArrays(1, &_vao);
  glBindVertexArray(_vao);
  glEnableVertexAttribArray(0);
  glEnableVertexAttribArray(1);

  glGenBuffers(2, _buffer);

  test_liste() ;
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
  //gl4duwKeyDownFunc(handleKeyPress);

  gl4duBindMatrix("modview");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
 
  gl4duLoadIdentityf(); //Cela veut dire recommence à ton état initial très important pour reset à chaque boucle
  //gl4dgDraw(_cone) ;
  //gl4duTranslatef(0,-5,0);
  drawL_system(arbuste) ;
  //gl4duSendMatrices();
  glUseProgram(0);
  angle += 0.7 ;
  //gl4duLookAtf(angle, 10.0f, 0.0f, 0.0f, -5.0f, 0.0f, 0.0f, anglevuY, anglevuZ);

}

void drawL_system(l_system ls){ //Jpense c mieux si je donne direct le L_systeme 
  //fprintf(stderr,"Charactére l-systeme %s, %d\n",ls.axoim,ls.taille_axoim) ;
  //gl4duLoadIdentityf();
  //gl4dgDraw(_cube) ;
  //gl4duTranslatef(0,avancement,0);
  //gl4duSendMatrices();

  float turtle[3] = {0.0,0.0,0.0} ;
  float temp_turtle[3] = {0.0,0.0,0.0} ;
  float dist = 1.5 ;
  int boolRotateGauche = 1 ;
  int boolRotateDroite = 1 ;
  

  for(int i = 0;i < ls.taille_axoim && i < max;i++){
    if(ls.axoim[i] == 'F'){
      if (i + 1 < ls.taille_axoim && ls.axoim[i + 1] != '['){
        //drawFeuille(turtle[0],turtle[1],turtle[2],90);
        drawLine3d(turtle[0],turtle[1],turtle[2],turtle[0],turtle[1],turtle[2]+dist,1.0,0.0,1.0,0.2);
      }
      else{
        //drawTronc(turtle[0],turtle[1],turtle[2],0);
        drawLine3d(turtle[0],turtle[1],turtle[2],turtle[0],turtle[1],turtle[2]+dist,10.0,0.85,0.2,0.2);

      }
    //drawL_system(arbuste) ; 
      turtle[2] += dist ;
    }
    else if(ls.axoim[i] == 'f'){
      turtle[2] += dist ;
    }
    else if(ls.axoim[i] == '+'){
      boolRotateGauche = 0 ;
      //gl4duRotatef(ls.angle,0,1,0) ;
    }
    else if(ls.axoim[i] == '-'){
      boolRotateDroite = 0 ;
      //gl4duRotatef(ls.angle,0,-1,0) ;
    }
    else if(ls.axoim[i] == '['){
      temp_turtle[0] = turtle[0] ; temp_turtle[1] = turtle[1] ; temp_turtle[2] = turtle[2] ;
    }
    else if(ls.axoim[i] == ']'){
      turtle[0] = temp_turtle[0] ; turtle[1] = temp_turtle[1] ; turtle[2] = temp_turtle[2] ;
    }

    //else{
      //fprintf(stderr,"Charactére l-systeme %c non reconue\n",ls.axoim[i]) ;
  //}
    //*/
  }
  max += 1 ;
}

void drawFeuille(float x,float y,float z,float angle){
  gl4duLoadIdentityf(); 
  
  gl4duTranslatef(x+1,y+1,z);
  gl4duRotatef(angle,1,1,0) ;
  glUniform4f(glGetUniformLocation(_pId, "couleur"),0.0,1.0,0.2,1.0);
  gl4dgDraw(_cube);

  gl4duSendMatrices();
}

void drawTronc(float x,float y,float z,float angle){
  gl4duLoadIdentityf(); 

  gl4duTranslatef(x,y,z);
  //gl4duRotatef(angle,1,1,0) ;
  gl4duScalef(0.5,1,1);
  glUniform4f(glGetUniformLocation(_pId, "couleur"),0.85,0.2,0.2,1.0);
  gl4dgDraw(_cube);

  gl4duSendMatrices();
}

void drawLine3d(float x1, float y1, float z1, float x2, float y2, float z2, float thickness, float r, float g, float b)
{
    GLfloat vertices[6] = {
        x1, y1, z1,
        x2, y2, z2};

    GLfloat colors[8] = {
        r, g, b, 1.0f,
        r, g, b, 1.0f};

    glBindVertexArray(_vao);

    /* Position */
    glBindBuffer(GL_ARRAY_BUFFER, _buffer[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
           
    /* Couleur */
    glBindBuffer(GL_ARRAY_BUFFER, _buffer[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);

    /* l'épaisseur de la ligne */
    glLineWidth(thickness);

    /* Dessiner la ligne */
    gl4duLoadIdentityf(); 
    gl4duTranslatef(0,0,-4) ;
    //gl4duRotatef(-angle, 0, 1, 0);
    gl4duSendMatrices();
    glDrawArrays(GL_LINES, 0, 2);

    /* Nettoyer */
    glBindVertexArray(0);
}


/* appelée lors du exit */
void quit(void) {
  /* nettoyer (libérer) tout objet créé avec GL4D */
  gl4duClean(GL4DU_ALL);
  if(_vao)
    glDeleteVertexArrays(1, &_vao);
  if(_buffer[0])
    glDeleteBuffers(2, _buffer);
}

