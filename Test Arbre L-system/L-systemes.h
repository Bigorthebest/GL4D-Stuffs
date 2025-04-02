//Protection contre le double appel 
#ifndef L_SYSTEMES_GL4D
#define L_SYSTEMES_GL4D

#include <GL4D/linked_list.h>
#include <string.h>

//Structure générative de notre L-sytémes 
typedef struct l_system {
    int iteration ; 
    float angle ;
    char * axoim ; 
    int taille_axoim ;
    char * regle ;
    int taille_regle ;
} l_system ;

//Je définie directement ma structure 
l_system arbuste = {
    1 , //ité
    60 , //angle
    "F", //axoim
    1 , //taille a
    "F[+F]F[-F]F", //régle
    11 //taille r 
} ;

//Definition des fonctions
linked_list_t * charToLl (char * string, int taille);
//void _printLlChar (linked_list_t * ll);
linked_list_t * genLsystem (l_system * ls);
void print_charLl (linked_list_t * lliste);
void free_char(void * c);
linked_list_t * llFusion (linked_list_t * base, linked_list_t * ajout, int pos);
llnode_t * llLast(linked_list_t * liste);
char * developpeL_system(l_system * ls);


char * developpeL_system(l_system * ls){
    //pré calcule de taille (on pourrait mieux faire)
    int nb_F = 0 ;
    for(int i = 0; i < ls->taille_axoim ;i++){
        if(ls->axoim[i] == 'F'){
            nb_F += 1 ;
        }
    }
    //Travail avec la taille
    int new_taille = (ls->taille_axoim - nb_F) + (nb_F * ls->taille_regle) ;
    if (new_taille <= 0) {
        fprintf(stderr, "Erreur: new_taille invalide (%d)\n", new_taille);
        exit(0);
    }
    //char *new_char = malloc((size_t)new_taille * sizeof(char));
    char *new_char = malloc(new_taille * sizeof(char));
    int compteur = 0 ;
    //Remplissage de la nouvelle chaine
    for(int i = 0; i < ls->taille_axoim; i++){
        if(ls->axoim[i] == 'F'){
            //fprintf(stderr, "on a un F\n") ;
            for(int j = 0 ;j < ls->taille_regle; j++){
                //fprintf(stderr, "on place un F regle %d\n",compteur) ;
                new_char[compteur]  = ls->regle[j] ;
                compteur += 1 ;
            }
        }
        else{
            //fprintf(stderr, "on a autre chose %d\n",compteur) ;
            new_char[compteur] = ls->axoim[i] ;
            compteur += 1 ;
        }
    }
    ls->taille_axoim = new_taille ;
    ls->axoim = new_char ;
    return new_char ;
}

void test_liste(void){
    //linked_list_t * test = llNew() ;
    //char * c = "C" ;
    //llInsertAt(llFirst(test),c) ;
    //fprintf(stderr,"%c",*(char*)test->head->data) ;
    //linked_list_t * teststring = charToLl(arbuste.regle,arbuste.taille_regle) ;
    //fprintf(stderr,"\n Premier char : %c  \n",*(char*)teststring->head->data) ;
    //_printLlChar(teststring) ;
    //print_charLl(teststring) ;
    //void (*ptr_fonction)(void *);
    //ptr_fonction = &free_char ;
    //llFree(teststring,ptr_fonction);
    //linked_list_t * test_l_system = genLsystem(&arbuste) ;
    //fprintf(stderr,"L systeme passer\n") ; 
    //print_charLl(test_l_system) ;
    //llFree(test_l_system,ptr_fonction) ;
    for(int i = 0;i < arbuste.iteration;i++){
        fprintf(stderr,"Taille axiom %d \n",arbuste.taille_axoim) ;
        char * dev = developpeL_system(&arbuste) ;
        fprintf(stderr,"Test remplace : %s \n",dev) ;
        //*arbuste.axoim = *dev ;
        fprintf(stderr,"Taille axiom %d \n",arbuste.taille_axoim) ;
    }
}

linked_list_t * charToLl (char * string, int taille){
    linked_list_t * lliste = llNew() ;
    //llnode_t ** currant = llFirst(lliste) ;
    for(int i = 0 ;i < taille; i++){
        //fprintf(stderr,"%c ->",string[i]) ;
        char * data = malloc(sizeof(char)) ;
        assert(data) ;
        *data = string[i] ;
        //fprintf(stderr,"deivent : %c |",*(char*)data) ;
        llPushBack(lliste,data) ;
    }
    return lliste ;
}

void print_charLl (linked_list_t * lliste){
    llnode_t * courrant = *llFirst(lliste) ;
    assert(courrant) ;
    while (courrant->next != NULL){
        fprintf(stderr,"%c ->",*(char*)courrant->data) ;  
        courrant = *llNext(&courrant) ;
    }   
    fprintf(stderr,"FIN \n") ;
}

void free_char(void * c){
    free(c);
}

linked_list_t * genLsystem (l_system * ls){
        // Initialisation des listes
        linked_list_t * axiom = charToLl(ls->axoim, ls->taille_axoim);
        linked_list_t * regle = charToLl(ls->regle, ls->taille_regle);
        linked_list_t * developer = llNew(); // Développer commence vide
    
        llnode_t *courrant = *llFirst(axiom);
        int compt = 0;
    
        // Parcours de l'axiome
        while (courrant != NULL) {
            char data = *(char *)(courrant->data);
            fprintf(stderr, "char %c -->", data);
    
            if (data == 'F') {
                // Fusionner la règle au bon endroit
                developer = llFusion(developer, regle, compt);
            } else {
                // Sinon, on ajoute le caractère courant
                llPushBack(developer, courrant->data);
            }
    
            // Passer au prochain nœud
            courrant = courrant->next;
            compt++;
        }
    
        // Nettoyer la mémoire
        void (*ptr_fonction)(void *);
        ptr_fonction = &free_char ;
        llFree(axiom,ptr_fonction);
        return developer;
    
    //Préparation
    /*
    linked_list_t * axiom = charToLl(ls->axoim,ls->taille_axoim) ;
    linked_list_t * regle = charToLl(ls->regle,ls->taille_regle) ;
    linked_list_t * developer = charToLl(ls->axoim,ls->taille_axoim) ;
    //print_charLl(axiom) ;
    //linked_list_t * test = llFusion(axiom,regle,0) ;
    //Developpement
    //llnode_t * courrant = *llFirst(axiom) ;
    llnode_t * courrant = *llFirst(axiom) ;
    int compt = 0 ;
    int compt_regle = 0;
    while (compt < ls->taille_axoim){
        //assert(courrant) ;
        void * data = llPop(axiom) ;
        fprintf(stderr,"char %c -->",*(char*)data) ;
        if(*(char*)data == 'F' ){
            //llDelete(axiom,&courrant,NULL) ;
            //fprintf(stderr,"char %c\n",*(char*)data) ;
            //llPushBack(developer,charToLl(ls->regle,ls->taille_regle)) ;
            //courrant = *llNext(&courrant) ;
            developer = llFusion(developer,regle,compt+compt_regle);
            //compt = 99 ;
            compt_regle += ls->taille_regle;
        }
        compt ++ ; 

        //for(int i = 0 ;i < ls->taille_regle; i++){
        //    courrant = *llNext(&courrant) ;
        //}
        //courrant = *llNext(&courrant) ;
    }
    //print_charLl(axiom) ;
    return developer ;*/
}

linked_list_t * llFusion (linked_list_t * base, linked_list_t * ajout, int pos){
    assert(base) ;

    if (!ajout || *llFirst(ajout) == NULL) {
        return base;
    }

    if (pos == 0) {
        llnode_t *dernier_ajout = llLast(ajout);
        assert(dernier_ajout) ;
        dernier_ajout->next = *llFirst(base);
        base = ajout;
        return base;
    }
    
    llnode_t * courrant = *llFirst(base) ;
    for(int i = 0 ;i < pos; i++){
        if(courrant->next == NULL){
            fprintf(stderr,"Erreur pos plus grand que ll, indice : %d \n",i) ;
            exit(1) ;
        }
        courrant = *llNext(&courrant) ;
    }
    //llnode_t ** pnt_next = &(courrant->next) ;
    llnode_t * dernier = llLast(ajout) ;
    dernier->next = courrant->next ;
    courrant->next = *llFirst(ajout) ;
    /*
    while (courrant->next != NULL){
        courrant = *llNext(&courrant) ;
        //fprintf(stderr,"Test fusion suite : %c ->",*(char*)courrant->data) ;  
    }   
    *llNext(&courrant) = *pnt_next ;*/
    return base ;
}

llnode_t * llLast(linked_list_t * liste){
    llnode_t * courrant = *llFirst(liste) ;
    while (courrant->next != NULL){
        courrant = *llNext(&courrant) ;
    }   
    return courrant ;
}


/*
void _printLlChar (linked_list_t * ll){
    llnode_t * noeud = *llFirst(ll) ;
    while (llNext(&noeud) != NULL){
        fprintf(stderr,"%c ->",*(char*)noeud->data) ;
    }
}*/

/*
char * genLsystem (l_system system){
    int i = system.iteration ;
    char * chaine_courante = system.axoim ;
    int taille_chaine = system.taille_axoim ;
    for(;i > 0; i--){
        for(int j = 0 ;j < taille_chaine; j++){
            if (chaine_courante[j] == 'F'){
                char * temp = realloc(chaine_courante,sizeof(char) * taille_chaine + system.taille_regle ) ;
                
            }
        }
    }
}*/



























#endif