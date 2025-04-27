//Crédit pour ma démo
/* inclusion des entêtes de fonctions de création et de gestion de
 * fenêtres système ouvrant un contexte favorable à GL4dummies. Cette
 * partie est dépendante de la bibliothèque SDL2 */
#include <GL4D/gl4duw_SDL2.h>
#include <GL4D/gl4dm.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4dh.h>
#include <SDL_ttf.h>

static void initText(GLuint * ptId, const char * text);

void credit(int state) {
    /* INITIALISEZ VOS VARIABLES */
    /* on créé une variable pour stocker l'identifiant du programme GPU */
    static GLuint _pId = 0;
    /* on créé une variable pour stocker l'identifiant de la géométrie d'un cube GL4D */
    static GLuint _quad_credit = 0;
    //Texture 
    static GLuint TexTexte = 0 ;
  const char texte[] = "                   Crédit :\nGL4D : Farès BELHADJ\nAvec une grande aide de Romaric sur le filtre VHS\nMusique \'nuit d'angoisse\' de Jean-Paul V \ndisponible sur le site : pixabay.com\n" ;
  switch(state) {
  case GL4DH_INIT:
    /* INITIALISEZ VOTRE ANIMATION (SES VARIABLES <STATIC>s) */
    glEnable(GL_DEPTH_TEST);
    SDL_GL_SetSwapInterval(1);
    _quad_credit = gl4dgGenQuadf();
    _pId = gl4duCreateProgram("<vs>shaders/basic.vs", "<fs>shaders/basic.fs", NULL);
    initText(&TexTexte,texte);
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
    gl4duFrustumf(-1, 1, -1, 1, 1, 1000);
    return;
  case GL4DH_FREE:
    /* LIBERER LA MEMOIRE UTILISEE PAR LES <STATIC>s */
    if(TexTexte) {
        glDeleteTextures(1, &TexTexte);
        TexTexte = 0 ;
      }
    return;
  case GL4DH_UPDATE_WITH_AUDIO:
    /* METTRE A JOUR VOTRE ANIMATION EN FONCTION DU SON */
    return;
  default: /* GL4DH_DRAW */
    gl4duBindMatrix("modview");
    glUseProgram(_pId);
    glActiveTexture(GL_TEXTURE0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBindTexture(GL_TEXTURE_2D, TexTexte);
    glUniform1i(glGetUniformLocation(_pId, "inv"), 1);
    glUniform4f(glGetUniformLocation(_pId, "color"),0.0,0.0,0.0,1.0 );

    gl4duLoadIdentityf();
    gl4duSendMatrices();
    glEnable(GL_DEPTH_TEST) ;
    gl4dgDraw(_quad_credit);
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);

  }
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
  SDL_Color c = {255, 255, 255, 255};
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
  if( !(font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 128)) ) {
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
  SDL_FillRect(s, NULL, SDL_MapRGBA(s->format, 0, 0, 0, 255));
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
