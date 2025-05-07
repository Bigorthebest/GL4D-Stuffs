#include <GL4D/gl4du.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>
#include <SDL_image.h>
#include <assert.h>
#include <GL4D/gl4df.h>

/* Prototypes des fonctions statiques contenues dans ce fichier C */
static void init(void);
static void resize(int w, int h);
static void draw(void);
static void quit(void);

/*!\brief largeur et hauteur de la fenêtre */
static int _wW = 1280, _wH = 1024;
/*!\brief identifiant du (futur) GLSL program */
static GLuint _pId[2] = { 0 };
/*!\brief identifiant de framebuffer */
static GLuint _fbo = 0;
/*!\brief identifiant du plan */
static GLuint _quad = 0;
static GLuint _tore = 0;
/*!\brief identifiant de textures */
static GLuint _texId[] = { 0, 0 };

int main(int argc, char ** argv) {
  if(!gl4duwCreateWindow(argc, argv, "GL4Dummies", 20, 20, 
			 _wW, _wH, GL4DW_RESIZABLE | GL4DW_SHOWN))
    return 1;
  init();
  atexit(quit);
  gl4duwDisplayFunc(draw);
  gl4duwMainLoop();
  return 0;
}

static void init(void) {
  _tore = gl4dgGenTorusf(20, 20, 0.33f);
  _pId[0] = gl4duCreateProgram("<vs>shaders/light.vs", "<fs>shaders/light.fs", NULL);
  //Ancienne maniére par l'include d'un fichier shaders
  //_pId[1] = gl4duCreateProgram("<vs>shaders/filter.vs", "<fs>shaders/filter.fs", NULL);

  //Debut INIT VHS
  _quad = gl4dgGenQuadf();
  //Nouvelle maniére include direct avec imfs
  if(!_pId[1]){
    const char * imfs = "<imfs>vhs-filter.fs</imfs>\n"
#ifdef __GLES4D__
    "#version 300 es\n"
#else
    "#version 330\n"
#endif
      "uniform sampler2D tex; \n"
      "uniform vec2 vhs_resolution = vec2(320.0, 240.0); \n"
      "uniform vec3 rgb_shift = vec3(0.015, 0.00, 0.005); \n"
      "uniform vec3 color_bruit = vec3(0.2); \n"
      "uniform float time; \n"
      "uniform float bruit; \n"
      "in vec2 vsoTexCoord; \n"
      "#define PI 3.1415926535897932384626433832795 \n"
      "out vec4 fragColor; \n"
      "uniform int samples = 0;\n"
      "uniform float tape_crease_smear = 1.0;"
      "uniform float tape_crease_jitter = 0.10;"
      "uniform float tape_crease_speed = 0.5;"
      "uniform float tape_crease_discoloration = 1.0;"
      "float v2random(vec2 uv) {"
      "  return fract(sin(dot(uv, vec2(12.9898, 78.233))) * 43758.5453);"
      "}"
      "void main() {"
      "  vec2 uvn = vsoTexCoord;"
      "  vec3 col = vec3(0.0, 0.0, 0.0);"
      "  uvn.x += (v2random(vec2(uvn.y, time * 10.0)) - 0.5) / vhs_resolution.x * 1.0;"
      "  float tc_phase = smoothstep(0.90, 0.96, sin(uvn.y * 8.0 - (time * tape_crease_speed + tape_crease_jitter * v2random(time * vec2(0.67, 0.59))) * PI * 1.2));"
      "float tc_noise = smoothstep(0.3, 1.0, v2random(vec2(uvn.y * 4.77, time)));"
      "float tc = tc_phase * tc_noise;"
      "uvn.x = uvn.x - tc / vhs_resolution.x * 8.0 * tape_crease_smear;"
      "vec2 red_uv = uvn + vec2(rgb_shift.r, 0.0);"
      "vec2 green_uv = uvn + vec2(rgb_shift.g, 0.0);"
      "vec2 blue_uv = uvn + vec2(rgb_shift.b, 0.0);"
      "float r = texture(tex, red_uv).r;"
      "float g = texture(tex, green_uv).g;"
      "float b = texture(tex, blue_uv).b;"
      "col = vec3(r, g, b);"
      "if(bruit != 0){"
      "  float r = v2random(uvn) ;"
      "  if(r * 2 > bruit){"
      "      fragColor = vec4(col, 1.0);"
      "  }"
      "    else{"
      "      fragColor = vec4(color_bruit,1.0);"
      "  }"
      "}"
      "else {"
      "  fragColor = vec4(col, 1.0);"
      "}"
    "}" ;
    const char * imvs =
    "<imvs>vhs-filter.vs</imvs>\n"
    "#version 330\n"
    "layout (location = 0) in vec2 vsiPosition;\n"
    "layout (location = 1) in vec3 vsiNormal;\n"
    "layout (location = 2) in vec2 vsiTexCoord;\n"
    "out vec2 vsoTexCoord;\n"
    "void main(void) {\n"
    "gl_Position = vec4(vsiPosition, 0.0, 1.0);\n"
    "vsoTexCoord = vec2(vsiTexCoord.x, vsiTexCoord.y);\n"
    "}" ;
    _pId[1] = gl4duCreateProgram(imvs, imfs, NULL); //j'utilise toujours mon fichier vs mais je pourrais le remplacer
    gl4duAtExit(quit);
}
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
  //FIN init VHS

  gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
  gl4duGenMatrix(GL_FLOAT, "modelMatrix");
  gl4duGenMatrix(GL_FLOAT, "viewMatrix");
  resize(_wW, _wH);
}

void resize(int w, int h) {
  GLfloat ratio = h / (GLfloat)w;
  _wW = w; _wH = h;
  gl4duBindMatrix("projectionMatrix");
  gl4duLoadIdentityf();
  gl4duFrustumf(-1.0f, 1.0f, -1.0f * ratio, 1.0f * ratio, 2.0f, 100.0f);
  glViewport(0, 0, _wW, _wH);
}



/*!\brief Cette fonction dessine dans le contexte OpenGL actif. */
static void draw(void) {
  static GLfloat angle = 0.0f;
  static double t0 = 0.0;
  double t = gl4dGetElapsedTime(), dt = (t - t0) / 1000.0;
  t0 = t;



  glEnable(GL_DEPTH_TEST);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(_pId[0]);

  gl4duBindMatrix("viewMatrix");
  gl4duLoadIdentityf();
  gl4duLookAtf(1.0f, 1.0f, 3.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
  gl4duBindMatrix("modelMatrix");
  gl4duLoadIdentityf();
  gl4duRotatef(angle, 1.0f, 0.0f, 0.0f);
  gl4duSendMatrices();

  glUniform1f(glGetUniformLocation(_pId[0], "temps"), t / 1000.0f);
  glUniform4f(glGetUniformLocation(_pId[0], "Lp"), 0.0f, 0.0f, 1.5f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId[0], "sdiffus"), 0.8f, 0.0f, 0.0f, 1.0f);
  glUniform4f(glGetUniformLocation(_pId[0], "sambient"), 0.85f, 1.0f, 0.85f, 1.0f);

  gl4dgDraw(_tore);

  angle += 18.0f * dt;

  //début DRAW VHS

  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId[0], 0);

  glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
  glBlitFramebuffer(0, 0, _wW, _wH, 0, 0, _wW, _wH, GL_COLOR_BUFFER_BIT, GL_NEAREST);


  glBindFramebuffer(GL_FRAMEBUFFER, _fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texId[1], 0); /* le output */

  glClear(GL_COLOR_BUFFER_BIT);
  glUseProgram(_pId[1]);
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, _texId[0]); /* le input */
  glUniform1i(glGetUniformLocation(_pId[1], "tex"), 0); // le 0 correspond à glActiveTexture(GL_TEXTURE0);
  glUniform1f(glGetUniformLocation(_pId[1], "time"), -(t/1000));
  glUniform3f(glGetUniformLocation(_pId[1], "rgb_shift"),0.090, 0.010, 0.070);
  glUniform1f(glGetUniformLocation(_pId[1], "bruit"),0.1);
  gl4dgDraw(_quad);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindFramebuffer(GL_READ_FRAMEBUFFER, _fbo);
  glBlitFramebuffer(0, 0, _wW, _wH, 0, 0, _wW, _wH, GL_COLOR_BUFFER_BIT, GL_NEAREST);
  //FIN DRAW VHS
}

/*!\brief appelée au moment de sortir du programme (atexit), elle
 *  libère les éléments OpenGL utilisés.*/
static void quit(void) {
  /* suppression de la texture */
  if(_texId[0])
    glDeleteTextures(2, _texId);
  /* suppression du fbo */
  if(_fbo)
    glDeleteFramebuffers(1, &_fbo);
  /* nettoyage des éléments utilisés par la bibliothèque GL4Dummies */
  _pId[1] = 0 ;
  gl4duClean(GL4DU_ALL);
}
