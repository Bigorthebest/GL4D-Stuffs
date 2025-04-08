#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dg.h>
#include <stdio.h>
#include <SDL_image.h>
#include <GL4D/gl4df.h>
#include "noise.h"

static void init(void);
/* TODO : gérer le retaillage de la fenêtre */
/* static void resize(int width, int height); */
static void draw(void);
static void sortie(void);

static GLuint _wW = 1024, _wH = 768;
static GLuint _soleilId = 0;
static GLuint _quadId = 0;
static GLuint _sphereId = 0;
static GLuint _pId[2] = { 0 };

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "Test coucher de soleil", GL4DW_POS_CENTERED, GL4DW_POS_CENTERED,
			 _wW, _wH, GL4DW_OPENGL | GL4DW_RESIZABLE | GL4DW_SHOWN)) {
    fprintf(stderr, "Erreur lors de la création de la fenêtre\n");
    return 1;
  }
  init();
  atexit(sortie);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

void init(void) {
  initNoiseTextures();
  glClearColor(0.75f, 0.75f, 0.75f, 1.0f);
  SDL_GL_SetSwapInterval(1);
  glEnable(GL_DEPTH_TEST);
  /* Attention on change la valeur par défaut et on active la CULL FACE */
  glEnable(GL_CULL_FACE);
  _soleilId = gl4dgGenSpheref(10,10);
  _quadId = gl4dgGenQuadf();
  _sphereId = gl4dgGenSpheref(10,10);
  _pId[0] = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  _pId[1] = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/perlin.fs", NULL);
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
  static const GLfloat rouge[] = {0.6f, 0.0f, 0.0f, 1.0f}, bleu[] = {0.0f, 0.0f, 0.6f, 1.0f};
  static GLfloat a = 0.0f;
  static GLfloat a1 = 0.0f;
  static double t0 = 0.0;
  double t = gl4dGetElapsedTime() / 1000.0, dt = (t - t0);
  t0 = t;


  //GLfloat lumpos[] = {-4.0f, 4.0f, 0.0f, 1.0f};
  GLfloat lumpos[] = {0.0f, 1.5f, -10.0f, 1.0f};
  lumpos[1] = 2.0f + 1.9f * sin(a);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId[1]);
  glUniform4fv(glGetUniformLocation(_pId[0], "lumpos") , 1, lumpos);

  gl4duBindMatrix("view");
  gl4duLoadIdentityf();
  gl4duLookAtf(0.0f, 2.0f, 6.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

  /* quad */
  gl4duBindMatrix("model");
  gl4duLoadIdentityf();
  gl4duRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
  gl4duScalef(15.0f, 10.0f, 15.0f);
  gl4duSendMatrices();
  glUniform1f(glGetUniformLocation(_pId[1], "zoom"), 1.0f);
  glUniform1f(glGetUniformLocation(_pId[1], "temps"), t);
  glUniform4fv(glGetUniformLocation(_pId[1], "couleur") , 1, bleu);

  gl4dgDraw(_quadId);

  //gl4dfScattering(0, 0, 6, 0, 0, GL_FALSE);
  //gl4dfMedian(0, 0, 2, GL_FALSE);  //gl4dfBlur(0, 0, 7, 1, 0, GL_FALSE);

  glUseProgram(0);

  a += 0.02f * M_PI * get_dt();
  a1 += 1.0f * M_PI *  get_dt();
}

void sortie(void) {
  freeNoiseTextures();
  gl4duClean(GL4DU_ALL);
}
