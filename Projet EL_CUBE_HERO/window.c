/*!\file window.c
 * \brief GL4Dummies, exemple simple 3D avec GL4Dummies
 * \author Farès BELHADJ, amsi@up8.edu
 * \date February 03 2022
 */

/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <stdio.h>
#include <assert.h>
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4du.h>
#include <GL4D/gl4dp.h>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

static void init(void);
static void draw(void);
static void quit(void);
static void initText(GLuint * ptId, const char * text);
void scene_credit_creditant(GLuint _pId);
static void initAudio(const char * filename);

static int horizon = -5 ;
static int vertical = -4 ;
static int espace = -7 ;
//-5 -4 -7
//Boolen pour credit 
static int bool_credit = 0 ;

//Gestion des couleurs de chaque cube celon les touches
static int cube0 = 0 ;
static int cube1 = 0 ;
static int cube2 = 0 ;
static int cube3 = 0 ;


/* on créé une variable pour stocker l'identifiant du programme GPU */
GLuint _pId = 0;
/* on créé une variable pour stocker l'identifiant de la géométrie d'un cube GL4D */
GLuint _cube = 0;
//Mon cube à moua
GLuint El_quad = 0 ;
//La grande sphére
GLuint EL_sphere = 0 ;
/*!\brief pointeur vers la musique chargée par SDL_Mixer */
static Mix_Music * _mmusic = NULL;
//Truc pour la zic 
/*!\brief nombre d'échantillons du signal sonore */
#define ECHANTILLONS 4
/*!\brief amplitude des échantillons du signal sonore */
static Sint16 _hauteurs[ECHANTILLONS];

//id de texture pour le texte 
//static GLuint _textTexId = 0;
//On va essayer de faire 2 texture, une couleur et l'autre sera l'id du texte
GLuint _texId[3] = { 0 };
//Les 3 texture pour le compteur 
GLuint comptId[3] = {0} ;
//Texture pour les sphere 
GLuint sphereId[1] = { 0} ;

int tableau_des_cube[10][4] = {
  {1,0,0,0},
  {0,1,0,0},
  {0,0,1,0},
  {0,0,0,1},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
  {0,0,0,0},
};

//Variable de test 
int compteur = 0 ;
int compteur_point = 0 ;

//Variable de score 
//char string_score[7] = {'S','c','o','r','e',' ',':'};
int score_total[3] = {0,0,0} ;
char string_1[1] = {'0'}  ;
char string_2[1] = {'0'} ;
char string_3[1] = {'0'} ;

int compt3 = 0 ;

void handleKeyPress(SDL_Keycode key) {
    switch (key) {
        case SDLK_ESCAPE: 
            //gl4duClean(GL4DU_ALL);
            break ;
        case SDLK_z:
            vertical += 1 ;
            break;
        case SDLK_s:
            vertical -= 1 ;
            break;
        case SDLK_q:
            horizon -= 1 ;
            break;
        case SDLK_d:
            horizon += 1 ;
            break;
        case SDLK_a:
            horizon = -2 ;
            vertical = 0 ;
            break ;
        case SDLK_w:
            espace += 1 ;
            break ;
        case SDLK_x:
            if (bool_credit == 0 ){
              bool_credit = 1 ;
              Mix_PauseMusic(); 
            }
            else{
              bool_credit = 0 ;
              Mix_ResumeMusic();
            }
            break ;
        case SDLK_g:
            cube0 = 10 ;
            break ;
        case SDLK_h:
            cube1 = 10 ;
            break ;
        case SDLK_j:
            cube2 = 10 ;
            break ;
        case SDLK_k:
            cube3 = 10 ;
            break ;
    }
}


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
  El_quad = gl4dgGenQuadf() ;
  EL_sphere = gl4dgGenSpheref(10,10) ;
  //gen un texte 
  //initText(&_texId[1], "Over the fallen leave, do you believe, do you believe now ?");
  initText(&_texId[1], "Credit :\n\nGL4D : Fares Belhadj\nEl cube hero : Julien BOUTICOURT\nMusic : Julien BOUTICOURT via Beat-box\nPolice : Disfrutadlo\nTexture : Julien BOUTICOURT");
  initText(&_texId[2], "Score :");
  initText(&comptId[0],string_1);
  glBindTexture(GL_TEXTURE_2D, 0);
  initText(&comptId[1],string_2);
  glBindTexture(GL_TEXTURE_2D, 0);
  initText(&comptId[0],string_3);
  glBindTexture(GL_TEXTURE_2D, 0);
  
  /* créer un programme GPU pour OpenGL (en GL4D) */
  _pId = gl4duCreateProgram("<vs>shaders/hello.vs", "<fs>shaders/hello.fs", NULL);
  /* créer dans GL4D une matrice qui s'appelle modview ; matrice de
     modélisation et vue qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "modview");
  /* créer dans GL4D une matrice qui s'appelle proj ; matrice de
     projection qu'on retrouvera dans le vertex shader */
  gl4duGenMatrix(GL_FLOAT, "proj");


  //Creation de la texture d'une sphére 
  glGenTextures(1, sphereId);
  /* on bind la texture _texId[1] */
  glBindTexture(GL_TEXTURE_2D, sphereId[0]);
  /* paramétrage de l'interpolation pour rapetisser ou agrandir la texture */
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_LINEAR
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  /* on récupère une image depuis SDL/BMP */
  SDL_Surface * s = SDL_LoadBMP("image/note.bmp");
  assert(s);
  /* je sais que mon images est RGB */
  assert(s->format->BytesPerPixel == 3);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGB, GL_UNSIGNED_BYTE, s->pixels);
  SDL_FreeSurface(s);


  /* binder (mettre au premier plan, "en courante" ou "en active") la matrice proj */
  gl4duBindMatrix("proj");
  /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
  gl4duLoadIdentityf();
  /* combiner la matrice courante avec une matrice de projection en
     perspective. Voir le support de cours pour les six paramètres :
     left, right, bottom, top, near, far */
  gl4duFrustumf(-1, 1, -0.75, 0.75, 1, 1000); //En gros la camera (ou plutot ça projection su rl'ecran, elle peut deformer les objets)

  //gl4duLookAtf(0.0f, yvue, zvue, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -2.0f);//angle camera a mettre dans le draw
  initAudio("music/royaume.mp3");

  /*
  glGenTextures(1, &_texId[3]);
  //GLuint pixels[] = { RGBA(255, 0, 0, 255), RGBA(255, 255, 255, 255), RGBA(255, 255, 255, 255), 0 };
  //initialisation de la texture pour nos notes de musique 
  glBindTexture(GL_TEXTURE_2D, _texId[3]);
  // paramétrage de l'interpolation pour rapetisser ou agrandir la texture */
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  /* on récupère une image depuis SDL/BMP */
  //SDL_Surface * s = SDL_LoadBMP("images/note.bmp");
  //assert(s);  
  //assert(s->format->BytesPerPixel == 4);
  //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  //SDL_FreeSurface(s);
  //glBindTexture(GL_TEXTURE_2D, 0);  

}

void draw_el_cube(int x , int y){
  gl4duLoadIdentityf();
  gl4duTranslatef(horizon +x, vertical , espace + y);
  gl4duTranslatef(x, 0, y);
  gl4duTranslatef(x, 0, 0);
  gl4duTranslatef(0.5, 0, 0);
  gl4duSendMatrices();
  //glEnable(GL_DEPTH_TEST) ;
  gl4dgDraw(_cube);
}

void draw_el_sphere(int x, int y){
  gl4duLoadIdentityf();
  gl4duTranslatef(horizon +x, vertical , espace + y);
  gl4duTranslatef(x, 0, y);
  gl4duTranslatef(x, 0, 0);
  gl4duTranslatef(0.5, 0, 0);
  gl4duRotatef(90,0,1,0);
  gl4duSendMatrices();
  //glEnable(GL_DEPTH_TEST) ;
  gl4dgDraw(EL_sphere);
}

void marcher_sphere(void){
  for (int i = 0; i < 10 - 1; i++) {
    for (int j = 0; j < 4; j++) {
      if (tableau_des_cube[i][j] == 1) {
        tableau_des_cube[i][j] = 0;
        tableau_des_cube[i - 1][j] = 1;
        }
      }
    }
}

void incrementScore(void){
  //printf("TOP");
  if(compteur_point > 0){
    return;
  }
  else{
    compteur_point = 10 ;
    if(string_1[0] == '9'){
      string_1[0] -= 8 ;
        if(string_2[0] == '9'){
          string_2[0] -= 8 ;
          if(string_3[0] == '9'){
            return;
          }
          else{
            string_3[0] += 1;
          }
          initText(&comptId[2], string_3);
          initText(&comptId[1], string_2);
        }
      else{
        string_2[0] += 1;
      }
      initText(&comptId[1], string_2);
    }
    else{
      string_1[0] += 1;
    }
    initText(&comptId[0], string_1);
  }
}

void draw(void) {
  //on active la magie des texture c'est important je crois 
  glActiveTexture(GL_TEXTURE0);

  /* on bind la texture _texId[0] */
  glBindTexture(GL_TEXTURE_2D, _texId[0]);
  /* on donne un nom à notre texture dans GLSL et on y met 0 car c'est l'étage où elle se trouve */

  /* une variable pour stocker un ange qui incrémente */
  //static float a = 0;
  /* set une couleur d'effacement random pour OpenGL */
  //glClearColor(gl4dmURand(), gl4dmURand(), gl4dmURand(), 1.0);
  /* effacer le buffer de couleur (image) et le buffer de profondeur d'OpenGL */
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  /* utiliser le programme GPU "_pId" */
  glUseProgram(_pId);
  glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  gl4duwKeyDownFunc(handleKeyPress);
  /* binder (mettre au premier plan, "en courante" ou "en active") la
     matrice modview */
  gl4duBindMatrix("modview");

  if(bool_credit == 0){
    /*
    if(_hauteurs[0]  > 0){
      glUniform4f(glGetUniformLocation(_pId, "couleur"), 0.0f, 1.0f, 0.0f, 1.0f);
    }
    else{
      glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 0.0f, 0.0f, 1.0f);
    }
    */
    /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
    //gl4duLoadIdentityf(); //Cela veut dire recommence à ton état initial très important pour reset à chaque boucle
    /* composer (multiplication à droite) avec une rotation d'angle a et
      d'axe (autour de l'axe) <0, 1, 0> */
      /* composer (multiplication à droite) avec une translation dans les z négatifs (-5) */ 
    //gl4duTranslatef(horizon, truc, -5); //-5 pour le mettre devant moi comme avec ellule
    //gl4duRotatef(a, 0, 1, 0); //Tourne autour de l'axe translater
    glUniform4f(glGetUniformLocation(_pId, "couleur"), 0.0f, 1.0f, 0.0f, 5.0f);
    for (int j = 0; j < 1; j++){
      for (int i = 0;i < 4;i++){
        switch (i)
        {
        
        case 0 :
          if(cube0 > 0){
            glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 0.0f, 0.0f, 1.0f);
            cube0 -= 1 ;
            if(tableau_des_cube[0][0] == 1){
              incrementScore();
            }
          }
          break;
        case 1 :
          if(cube1 > 0){
            glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 0.0f, 0.0f, 1.0f);
            cube1 -= 1 ;
            if(tableau_des_cube[0][1] == 1){
              incrementScore();
            }
          }
          break;
        case 2 :
          if(cube2 > 0){
            glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 0.0f, 0.0f, 1.0f);
            cube2 -= 1 ;
            if(tableau_des_cube[0][2] == 1){
              incrementScore();
            }
          }
          break;
        case 3 :
          if(cube3 > 0){
            glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 0.0f, 0.0f, 1.0f);
            cube3 -= 1 ;
            if(tableau_des_cube[0][3] == 1){
              incrementScore();
            }
          }
          break;
        }

        draw_el_cube(i,j);
        glUniform4f(glGetUniformLocation(_pId, "couleur"), 0.0f, 1.0f, 0.0f, 1.0f);
      }
    }
    //Les sphere jaune
    glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 1.0f, 0.0f, 1.0f);
    //glBindTexture(GL_TEXTURE_2D, _texId[2]);
    glBindTexture(GL_TEXTURE_2D, sphereId[0]);
    /* mettre la matrice identité (celle qui ne change rien) dans la matrice courante */
    for (int j = 0; j < 10; j++){
      for (int i = 0;i < 4;i++){
        if(tableau_des_cube[j][i] == 1){
          draw_el_sphere(i,-j);
        }
        //draw_el_sphere(i,-j);
      }}
    //glBindTexture(GL_TEXTURE_2D, 0);

    if(_hauteurs[0] > 8000){
      tableau_des_cube[8][0] = 1 ;
    }
    if(_hauteurs[1] < -6000){
      tableau_des_cube[8][1] = 1 ;
    }
    if(_hauteurs[2] < -6000){
      tableau_des_cube[8][2] = 1 ;
    }
    if(_hauteurs[3] > 8000){
      tableau_des_cube[8][3] = 1 ;
    }

      //Avancement des boules 
    if(compteur == 60){
      //incrementScore();
      marcher_sphere();
      compteur = 0 ;
    }
    compteur ++ ;
    compteur_point -= 1 ;

    //le score 
    
    glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 1.0f, 1.0f, 1.0f);
    //glBindTexture(GL_TEXTURE_2D, _texId[2]);
    glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
    gl4duLoadIdentityf();
    gl4duTranslatef(horizon , vertical +8 , espace );
    gl4duScalef(2.0,0.9,1.0);
    gl4duSendMatrices();
    glEnable(GL_DEPTH_TEST) ;
    //gl4dgDraw(El_quad);
    glBindTexture(GL_TEXTURE_2D, _texId[2]);
    gl4dgDraw(El_quad);

    glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 1.0f, 1.0f, 1.0f);
    //glBindTexture(GL_TEXTURE_2D, _texId[2]);
    glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
    gl4duLoadIdentityf();
    gl4duTranslatef(horizon +4, vertical +8 , espace );
    gl4duScalef(0.7,0.7,0.7);
    gl4duSendMatrices();
    glEnable(GL_DEPTH_TEST) ;
    //gl4dgDraw(El_quad);
    glBindTexture(GL_TEXTURE_2D, comptId[0]);
    gl4duTranslatef(horizon +1, vertical +8 , espace );
    gl4dgDraw(El_quad);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 1.0f, 1.0f, 1.0f);
    //glBindTexture(GL_TEXTURE_2D, _texId[2]);
    glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
    gl4duLoadIdentityf();
    gl4duTranslatef(horizon +6 , vertical +8 , espace );
    //gl4duScalef(2.0,1.0,1.0);
    gl4duScalef(0.7,0.7,0.7);
    gl4duSendMatrices();
    glEnable(GL_DEPTH_TEST) ;
    
    glBindTexture(GL_TEXTURE_2D, comptId[1]);
    //gl4duTranslatef(horizon -2, vertical +8 , espace );
    gl4dgDraw(El_quad);
    glBindTexture(GL_TEXTURE_2D, 0);

    glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 1.0f, 1.0f, 1.0f);
    //glBindTexture(GL_TEXTURE_2D, _texId[2]);
    glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
    gl4duLoadIdentityf();
    gl4duTranslatef(horizon +8 , vertical +8 , espace );
    //gl4duScalef(2.0,1.0,1.0);
    gl4duScalef(0.7,0.7,0.7);
    gl4duSendMatrices();
    glEnable(GL_DEPTH_TEST) ;
    
    glBindTexture(GL_TEXTURE_2D, comptId[2]);
    //gl4duTranslatef(horizon -2, vertical +8 , espace );
    gl4dgDraw(El_quad);
    glBindTexture(GL_TEXTURE_2D, 0);
    //glBindTexture(GL_TEXTURE_2D, comptId[2]);
    //gl4duTranslatef(horizon -3, vertical +8 , espace );
    //gl4dgDraw(El_quad);


  }
  else{
    scene_credit_creditant(_pId) ;
  }

  /*
  //Avancement des boules 
  if(compteur == 60){
    incrementScore();
    marcher_sphere();
    compteur = 0 ;
  }
  compteur ++ ;*/
  
  //Petit if d'analyse de son
  /*
  if (compteur == 60){
    compteur = 0 ;
    for(int i = 0 ;i < ECHANTILLONS;i++){
      fprintf(stderr,"Echantillion à : %d",_hauteurs[i]);
    }
  }
  compteur += 1 ;*/

  

  /* !!! Ceci est passer dans la fonction scene credit creditant
  glBindTexture(GL_TEXTURE_2D, _texId[1]);
  glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
  //glUniform1i(glGetUniformLocation(_pId, "tex"), 0);
  gl4duLoadIdentityf();
  gl4duTranslatef(horizon +2, vertical , espace + 2);
  gl4duTranslatef(2, 0, 2);
  gl4duTranslatef(2, 0, 0);
  gl4duTranslatef(0.5, 0, 0);
  gl4duSendMatrices();
  glEnable(GL_DEPTH_TEST) ;
  gl4dgDraw(El_quad);
  */

  //Je peut mettre ça pour bien séparer les texture (toujours à la fin)
  //glBindTexture(GL_TEXTURE_2D, 0); 
  
  /////printf("coord : %d , %d , %d",horizon,espace,vertical);


  //printf("x : %d, y : %d, z : %d\n",horizon,truc,espace) ;
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  //gl4duSendMatrices();
  /* demander le dessin d'un objet GL4D */
  //glEnable(GL_DEPTH_TEST) ;
  //gl4dgDraw(_cube);
  //Autre matrice pour le deuxiéme cube
  //On repart de 0 
  //gl4duLoadIdentityf() ;
    /* composer (multiplication à droite) avec une translation dans les z négatifs (-5) */ 
  //gl4duTranslatef(2, 0, -5); //-5 pour le mettre devant moi comme avec ellule
  //gl4duRotatef(-a, 0, 1, 0); //Tourne autour de l'axe translater
  /* envoyer les matrice GL4D au programme GPU OpenGL (en cours) */
  //gl4duSendMatrices();
  //glDisable(GL_DEPTH_TEST) ;
  //gl4dgDraw(_cube);
  /* n'utiliser aucun programme GPU (pas nécessaire) */
  //glUseProgram(0);
  /* augmenter l'ange a de 1 */
  //a -= 1;
  //*/
}

void scene_credit_creditant(GLuint _pId){
  glBindTexture(GL_TEXTURE_2D, 0);
  glUniform4f(glGetUniformLocation(_pId, "couleur"), 1.0f, 1.0f, 1.0f, 1.0f);
  //Affichage du texte et gestion du quad 
  glBindTexture(GL_TEXTURE_2D, _texId[1]);
  glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
  gl4duLoadIdentityf();

  //gl4duTranslatef(-5,-2,-7);
  gl4duTranslatef(horizon + 5, vertical +4 , espace );
  //-5 -4 -7
  gl4duScalef(6.0,4.0,1.0);
  gl4duSendMatrices();
  glEnable(GL_DEPTH_TEST) ;
  gl4dgDraw(El_quad);

}

/*!\brief Cette fonction est appelée quand l'audio est joué et met 
 * dans \a stream les données audio de longueur \a len.
 * \param udata pour user data, données passées par l'utilisateur, ici NULL.
 * \param stream flux de données audio.
 * \param len longueur de \a stream. */
static void mixCallback(void *udata, Uint8 *stream, int len) {
  int i;
  Sint16 *s = (Sint16 *)stream;
  if(len >= 2 * ECHANTILLONS)
    for(i = 0; i < ECHANTILLONS; i++)
      _hauteurs[i] =  s[i] ;/// ((1 << 15) - 1.0);
  return;
}

static void initAudio(const char * filename) {
  int mixFlags = MIX_INIT_OGG | MIX_INIT_MP3 | MIX_INIT_MOD, res;
  res = Mix_Init(mixFlags);
  if( (res & mixFlags) != mixFlags ) {
    fprintf(stderr, "Mix_Init: Erreur lors de l'initialisation de la bibliotheque SDL_Mixer\n");
    fprintf(stderr, "Mix_Init: %s\n", Mix_GetError());
    //exit(3); commenté car ne réagit correctement sur toutes les architectures
  }
  if(Mix_OpenAudio(44100, AUDIO_S16LSB, 2, 1024) < 0)
    exit(4);
  if(!(_mmusic = Mix_LoadMUS(filename))) {
    fprintf(stderr, "Erreur lors du Mix_LoadMUS: %s\n", Mix_GetError());
    exit(5);
  }
  Mix_SetPostMix(mixCallback, NULL);
  if(!Mix_PlayingMusic())
    Mix_PlayMusic(_mmusic, 1);
  //Mix_VolumeChunk(_mmusic, 0);
}


/* appelée lors du exit */
void quit(void) {
  //Supression des fichier music 
  if(_mmusic) {
    if(Mix_PlayingMusic())
      Mix_HaltMusic();
    Mix_FreeMusic(_mmusic);
    _mmusic = NULL;
  }
  Mix_CloseAudio();
  Mix_Quit();
  /* nettoyer (libérer) tout objet créé avec GL4D */
  /*Ca ct seulement qu'en y avait qu'un seul id de texte 
  if(_textTexId) {
    glDeleteTextures(1, &_textTexId);
    _textTexId = 0;
  }*/
  if(_texId[0]) {
    /* on supprime la texture côté OpenGL */
    glDeleteTextures(3, _texId);
    _texId[0] = 0;
  }
  if(sphereId[0]){
    glDeleteTextures(1, sphereId);
    sphereId[0] = 0 ;
  }
  if(comptId[0]){
    glDeleteTextures(3, comptId);
    comptId[0] = 0 ;
  }
  gl4duClean(GL4DU_ALL);
}

/*!\brief création d'une texture avec du texte. 
 *
 * \param ptId pointeur vers la variable stockant l'identifiant de
 * texture OpenGL. Si l'identifiant existe déjà (!= 0) l'utiliser
 * sinon (== 0) en générer un nouveau.
 *
 * \param text pointeur vers la chaine de caractères contenant le
 * texte.
 */
static void initText(GLuint * ptId, const char * text) {
  static int firstTime = 1;
  SDL_Color c = {255, 255, 0, 255};
  SDL_Surface * d, * s;
  TTF_Font * font = NULL;
  if(firstTime) {
    /* initialisation de la bibliothèque SDL2 ttf */
    if(TTF_Init() == -1) {
      fprintf(stderr, "TTF_Init: %s\n", TTF_GetError());
      exit(2);
    }
    firstTime = 0;
  }
  if(*ptId == 0) {
    /* initialisation de la texture côté OpenGL */
    glGenTextures(1, ptId);
    glBindTexture(GL_TEXTURE_2D, *ptId);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  }
  /* chargement de la font */
  if( !(font = TTF_OpenFont("police/MonsterFriendFore.ttf", 128)) ) {
    fprintf(stderr, "TTF_OpenFont: %s\n", TTF_GetError());
    return;
  }
  /* création d'une surface SDL avec le texte */
  d = TTF_RenderUTF8_Blended_Wrapped(font, text, c, 2048);
  if(d == NULL) {
    TTF_CloseFont(font);
    fprintf(stderr, "Erreur lors du TTF_RenderText\n");
    return;
  }
  /* copie de la surface SDL vers une seconde aux spécifications qui correspondent au format OpenGL */
  s = SDL_CreateRGBSurface(0, d->w, d->h, 32, R_MASK, G_MASK, B_MASK, A_MASK);
  assert(s);
  SDL_BlitSurface(d, NULL, s, NULL);
  SDL_FreeSurface(d);
  /* transfert vers la texture OpenGL */
  glBindTexture(GL_TEXTURE_2D, *ptId);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, s->w, s->h, 0, GL_RGBA, GL_UNSIGNED_BYTE, s->pixels);
  fprintf(stderr, "Dimensions de la texture : %d %d\n", s->w, s->h);
  SDL_FreeSurface(s);
  TTF_CloseFont(font);
  glBindTexture(GL_TEXTURE_2D, 0);
}
