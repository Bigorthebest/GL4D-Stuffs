/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>


static void init(void);
static void draw(void);
static void filtreVHS(GLint originFBO,double t);

/* on créé une variable pour stocker l'identifiant du programme GPU */
GLuint _pIdFractale = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un sphere GL4D */
GLuint _quad = 0;

static int _wW = 1920, _wH = 1080;//1920, 1080

//Gestion du filtre 
//id FBO 
static GLuint _fbo = 0;
//Id texture (pour le fbo)
static GLuint _texId[] = { 0, 0 };
static GLfloat bruit = 0.02 ;
static GLuint _pIdFiltre = 0 ;

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
      if(_texId[0]) {
        glDeleteTextures(2, _texId);
        _texId[0] = _texId[1] = 0;
      }
      if(_fbo){
        glDeleteFramebuffers(1, &_fbo);
      }
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
  _pIdFiltre = gl4duCreateProgram("<vs>shaders/filter.vs", "<fs>shaders/filter-bar.fs", NULL);
    //gestion des textures pour le FBO 
    //fbo de post process
  glGenFramebuffers(1, &_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glGenTextures(2, _texId);
  for(int i = 0; i < 2; ++i) {
    glBindTexture(GL_TEXTURE_2D, _texId[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _wW, _wH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  //glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId[i], 0);
  }
}

void draw(void) {
  static GLfloat angle = 0.0 ;
  static double t0 = 0.0 ;
  double t = gl4dGetElapsedTime() / 1000.0, dt = (t - t0);
  t0 = t;

  //Sauvegarder le framebuffer original
  GLint originFBO;
  glGetIntegerv(GL_FRAMEBUFFER_BINDING, &originFBO);

  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pIdFractale);
  glUniform1f(glGetUniformLocation(_pIdFractale,"angle"),angle) ;
  gl4dgDraw(_quad);
  /* n'utiliser aucun programme GPU (pas nécessaire) */
  glUseProgram(0);
  angle += dt * M_PI ;
  filtreVHS(originFBO, t); 
}

static void filtreVHS(GLint originFBO,double t){
  /*Ici on récupére notre buffer de dessin (avec originFBO) qu'on lie et envoie dans _texId[0] qui sera notre input pour le filtre
  Ensuite on reprend la main avec notre fbo custom auquel on bind la seconde texture (de sortie) sur laquelle on applique le filtre
  la premiére ligne est correct cars on comunique avec notre fbo custom*/
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId[0], 0);
  
  glBindFramebuffer(GL_READ_FRAMEBUFFER, originFBO);
  glBlitFramebuffer(0, 0, _wW, _wH, 0, 0, _wW, _wH, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  
  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId[1], 0); //Notre texture output
  
  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(_pIdFiltre);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId[0]); //on mets la texture de l'input ici
  glUniform1i(glGetUniformLocation(_pIdFiltre, "tex"), 0); // le 0 correspond à glActiveTexture(GL_TEXTURE0);
  glUniform1f(glGetUniformLocation(_pIdFiltre, "time"), -(t));
  glUniform3f(glGetUniformLocation(_pIdFiltre, "rgb_shift"),0.00, 0.00, 0.00);
  glUniform1f(glGetUniformLocation(_pIdFiltre, "bruit"),bruit);
  //glUniform1i(glGetUniformLocation(_pIdShip[1], "glitch"), prog_glitch[indice_glitch]);
  gl4dgDraw(_quad);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  glBindFramebuffer(GL_FRAMEBUFFER, originFBO); //On remet notre buffer de dessin classique
  glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo); //On lit dans notre fbo (mais on ne le bind pas vraiment)
  glBlitFramebuffer(0, 0, _wW, _wH, 0, 0, _wW, _wH, GL_COLOR_BUFFER_BIT, GL_NEAREST); //Concretement c'est afficher l'output
  
  //Restaurer le framebuffer original
  glBindFramebuffer(GL_FRAMEBUFFER, originFBO); //c'est étrange mais important de tout de même le re-bind ici
}


