#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <SDL_image.h>
#include <GL4D/gl4df.h>
#include <GL4D/gl4dh.h>
#include "noise.h"

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);

static GLuint _wW = 1024, _wH = 768;
static GLuint _soleilId = 0;
static GLuint _quadId = 0;
static GLuint _quad_fbo = 0 ;
static GLuint _sphereId = 0;
static GLuint _pIdDawn[3] = { 0 };
static GLfloat bruit = 0.02 ;
//id FBO 
static GLuint _fbo = 0;
//Id texture (pour le fbo)
static GLuint _texId[] = { 0, 0 };
//Pour la programation des glitchs
int prog_glitch[] = {0,0,0,1,2,1,5,4,4,4,4,4,4,4,2,1,5,3,3,3,3,3,3,3,3} ;
int indice_glitch = 0 ;
int taille_prog = 26 ;
int compt = 0 ;
/*!\brief créé la fenêtre, un screen 2D effacé en noir et lance une
 *  boucle infinie.*/
void dawn(int state) {
  /* INITIALISEZ VOS VARIABLES */
 switch(state) {
   case GL4DH_INIT:
     /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
     init();
     return;
   case GL4DH_FREE:
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

void init(void) {
  initNoiseTextures();
  glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
  SDL_GL_SetSwapInterval(1);
  glEnable(GL_DEPTH_TEST);
  /* Attention on change la valeur par défaut et on active la CULL FACE */
  glEnable(GL_CULL_FACE);
  _quad_fbo = gl4dgGenQuadf();
  _soleilId = gl4dgGenSpheref(10,10);
  _quadId = gl4dgGenQuadf();
  _sphereId = gl4dgGenSpheref(10,10);
  _pIdDawn[0] = gl4duCreateProgram("<vs>shaders/dawn.vs", "<fs>shaders/dawn.fs", NULL);
  _pIdDawn[1] = gl4duCreateProgram("<vs>shaders/dawn.vs", "<fs>shaders/perlin.fs", NULL);
  _pIdDawn[2] = gl4duCreateProgram("<vs>shaders/filter.vs", "<fs>shaders/filter.fs", NULL);
  //gestion des textures pour le FBO 
  glGenTextures(2, _texId);
  for(int i = 0; i < 2; ++i) {
    glBindTexture(GL_TEXTURE_2D, _texId[i]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, _wW, _wH, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  }
  glBindTexture(GL_TEXTURE_2D, 0);  

  glGenFramebuffers(1, &_fbo);

  gl4duGenMatrix(GL_FLOAT, "model");
  gl4duGenMatrix(GL_FLOAT, "view");
  gl4duGenMatrix(GL_FLOAT, "proj");
  gl4duBindMatrix("proj");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1.0f, 1.0f, (-1.0f * _wH)  / _wW, (1.0f * _wH)  / _wW, 1.0f, 1000.0f);
  /* glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); */
  /* glLineWidth(7.0f); */
  /* glEnable(GL_LINE_SMOOTH); */
}

static double get_dt(void) {
  static double t0 = 0.0f;
  double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
  t0 = t;
  return dt;
}

void draw(void) {
  static GLfloat a = 0.0f;
  static GLfloat a1 = 0.0f;
  //static double t0 = 0.0;
  double t = gl4dGetElapsedTime() / 1000.0;// dt = (t - t0);
  //t0 = t;

  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  //GLfloat lumpos[] = {-4.0f, 4.0f, 0.0f, 1.0f};
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glClearColor(0.5,0.9,0.1,1.0) ;

  glUseProgram(_pIdDawn[0]);
  glUniform4f(glGetUniformLocation(_pIdDawn[0], "lumpos") ,0.0f, 0.0f, -3.0f, 1.0f);
  //glUniform4f(glGetUniformLocation(_pIdDawn[0], "lumpos") ,0.0f, 0.4f, -20.0f, 1.0f);
  glUniform4f(glGetUniformLocation(_pIdDawn[0], "ldiffus"), 1.0f, 0.1f, 0.1f, 1.0f);
  glUniform4f(glGetUniformLocation(_pIdDawn[0], "lspec"), 1.0f, 0.6f, 0.0f, 1.0f);
  //Lumiére de la scéne 
  if(prog_glitch[indice_glitch] == 3){
    glUniform4f(glGetUniformLocation(_pIdDawn[0], "lambient"), 1.0f, 0.2f, 0.0f, 1.0f);
    glUniform4f(glGetUniformLocation(_pIdDawn[0], "sambient"),  0.80f, 1.0f, 0.0f, 1.0f);
    glUniform4f(glGetUniformLocation(_pIdDawn[0], "sdiffus"),  0.6f, 0.6f, 0.0f, 1.0f);
    glUniform4f(glGetUniformLocation(_pIdDawn[0], "sspeculaire"),1.0f, 0.5f, 0.5f, 1.0f);
  }
  else{
    glUniform4f(glGetUniformLocation(_pIdDawn[0], "lambient"), 0.0f, 0.2f, 1.0f, 1.0f);
    glUniform4f(glGetUniformLocation(_pIdDawn[0], "sambient"),  0.0f, 1.0f, 0.80f, 1.0f);
    glUniform4f(glGetUniformLocation(_pIdDawn[0], "sdiffus"),  0.0f, 0.6f, 0.6f, 1.0f);
    glUniform4f(glGetUniformLocation(_pIdDawn[0], "sspeculaire"),1.0f, 0.5f, 0.5f, 1.0f);
  }


  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(0.0f, 2.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  /* quad */
  gl4duBindMatrix("model");
  gl4duLoadIdentityf();
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  gl4duScalef(15.0f, 10.0f, 15.0f);
  gl4duSendMatrices();
  if(prog_glitch[indice_glitch] == 3){
    glUniform1f(glGetUniformLocation(_pIdDawn[0], "temps"), 0.0);
  }
  else{
    glUniform1f(glGetUniformLocation(_pIdDawn[0], "temps"), t);
  }
  gl4dgDraw(_quadId);


  /* Attention le CULL FACE est sur les faces avant */
  glCullFace(GL_FRONT);
  /* sphere */
  glUseProgram(_pIdDawn[1]);
  gl4duLoadIdentityf();
  gl4duScalef(14.0f, 14.0f, 14.0f);
  gl4duRotatef(180.0f * a1 / M_PI, 0.0f, 0.0f, 1.0f);
  gl4duSendMatrices();
  glUniform1i(glGetUniformLocation(_pIdDawn[1], "est_ciel"), 0);
  glUniform1f(glGetUniformLocation(_pIdDawn[1], "zoom"), 0.9f);
  useNoiseTextures(_pIdDawn[1], 0);
  gl4dgDraw(_sphereId);
  //unuseNoiseTextures(0);

  /* Attention le CULL FACE est sur les faces arrières */
  glCullFace(GL_BACK);
  /* Soleil */
  gl4duLoadIdentityf();
  gl4duTranslatef(0.0f, 0.4f, -14.0f);
  gl4duScalef(2.0f, 2.0f, 2.0f);
  gl4duRotatef(180.0f * a / M_PI, 0.0f, 1.0f, 0.0f);
  gl4duSendMatrices();
  glUniform1i(glGetUniformLocation(_pIdDawn[1], "est_ciel"), 1);
  glUniform1f(glGetUniformLocation(_pIdDawn[1], "zoom"), 1.1f);
  if(prog_glitch[indice_glitch] == 3){
    glUniform1f(glGetUniformLocation(_pIdDawn[0], "temps"), 0.0);
  }
  else{
    glUniform1f(glGetUniformLocation(_pIdDawn[0], "temps"), a);
  } 
  gl4dgDraw(_quadId);
  useNoiseTextures(_pIdDawn[1], 0);
  gl4dgDraw(_sphereId);
  unuseNoiseTextures(0);
 

  //gl4dfScattering(0, 0, 6, 0, 0, GL_FALSE);
  //gl4dfMedian(0, 0, 2, GL_FALSE);
  gl4dgDraw(_soleilId);
  //gl4dfBlur(0, 0, 7, 1, 0, GL_FALSE);

  a += 0.02f * M_PI * get_dt();
  a1 += 1.0f * M_PI *  get_dt();

  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId[0], 0);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, _wW, _wH, 0, 0, _wW, _wH, GL_COLOR_BUFFER_BIT, GL_NEAREST);


  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId[1], 0); /* le output */

  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(_pIdDawn[2]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId[0]); /* le input */
  glUniform1i(glGetUniformLocation(_pIdDawn[2], "tex"), 0); // le 0 correspond à glActiveTexture(GL_TEXTURE0);
  glUniform1f(glGetUniformLocation(_pIdDawn[2], "time"), -(t));
  glUniform3f(glGetUniformLocation(_pIdDawn[2], "rgb_shift"),0.015, 0.00, 0.005);
  glUniform1f(glGetUniformLocation(_pIdDawn[2], "bruit"),bruit);
  glUniform1i(glGetUniformLocation(_pIdDawn[2], "glitch"), prog_glitch[indice_glitch]);
  //glUniform2f(glGetUniformLocation(_pIdDawn[2], "vhs_resolution"),_wW,_wH); //sert a rien apparement, viens du projet original

  gl4dgDraw(_quad_fbo);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
  glBlitFramebuffer(0, 0, _wW, _wH, 0, 0, _wW, _wH, GL_COLOR_BUFFER_BIT, GL_NEAREST);

  //fprintf(stderr,"temps : %f, compt : %d indice : %d\n",t,compt,indice_glitch);
  
  if(t > 17){//On commence la prog après un petit temps 
    if(compt >= 30){
      indice_glitch += 1 ;
      if (indice_glitch == taille_prog){
        indice_glitch = 0 ;
    
      }
      compt = 0 ;
    }
    compt += 1 ;
  }
}
