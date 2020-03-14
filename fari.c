#include <stdio.h>
#include <stdlib.h>
#include<errno.h>
#include<string.h>
#include "fari.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <glob.h>
#include <jansson.h>


Liste *initialisation()
{
    Liste *liste = malloc(sizeof(*liste));

    if (liste == NULL)
    {
        exit(EXIT_FAILURE);
    }


    liste->premier = NULL;

    return liste;
}



void insertion(Liste *liste, char* token)
{
    /* Création du nouvel élément */
    Element *nouveau = malloc(sizeof(Element));
    if (liste == NULL || nouveau == NULL)
    {
        exit(EXIT_FAILURE);
    }
    if(token!=NULL){
   
    nouveau->mot=(char *)malloc(sizeof(char)*strlen(token));
    strcpy(nouveau->mot,token);}

    /* Insertion de l'élément au début de la liste */

    nouveau->suivant = liste->premier;
    liste->premier = nouveau;


}





void suppression(Liste *liste)
{
    if (liste == NULL)
    {
        exit(EXIT_FAILURE);
    }

    if (liste->premier != NULL)
    {
        Element *aSupprimer = liste->premier;
        liste->premier = liste->premier->suivant;
        free(aSupprimer);
    }
}





// PARTIE JSON

Liste* inversion_liste(Liste* liste_inverse){
     Liste* liste_conforme = initialisation(); 
      if (liste_inverse == NULL)
    {
        exit(EXIT_FAILURE);
    }

    Element *actuel_inverse = liste_inverse->premier;

    while (actuel_inverse != NULL)
    {
        insertion(liste_conforme, actuel_inverse->mot);
        actuel_inverse = actuel_inverse->suivant;
    }
    return liste_conforme;
}




void ajout_array_json(json_t * array, Liste* liste){

      if (liste == NULL)
    {
        exit(EXIT_FAILURE);
    }

    Element *actuel_json = liste->premier;

    while (actuel_json != NULL)
    {
        json_array_append( array, json_string(actuel_json->mot) );
        actuel_json = actuel_json->suivant;
    }
}



void ajout_array_json2(json_t * array, Liste* liste){

      if (liste == NULL)
    {
        exit(EXIT_FAILURE);
    }

    Element *actuel_json2 = liste->premier;
        
    int sat = 0;
    while (actuel_json2 != NULL)
    {   
        sat =strlen(actuel_json2->mot);
        for(int lufy = 0; lufy<sat; lufy++){
          if(lufy==sat-2){
            if((actuel_json2->mot[lufy]=='o')&&(actuel_json2->mot[lufy-1]=='.')){
                      actuel_json2->mot[lufy+1]='!';  

            }
          }
        }
        actuel_json2->mot[sat]='!';  
        char* tokenoos;
        tokenoos = strtok(actuel_json2->mot,"!");
       // strcpy(actuel_json2->mot,tokenoos); 
        json_array_append( array, json_string(tokenoos) );
        actuel_json2 = actuel_json2->suivant;
    }
}



    void generate_json(Liste* headers, Liste *source, Liste *libraries, Liste *flags, char * executable_name, char* fari_error_msg, Liste* command,Liste *liste_command, char * error_msg0,char* error_msg1, char* fari_msg, int valeur){
     
    if ((executable_name == NULL)){
         exit(EXIT_FAILURE);
    }
    if (command == NULL){
        command=initialisation();
    }
    if (command->premier == NULL){
        insertion(command,"");
    }
    if (liste_command==NULL){
        liste_command=initialisation();
        insertion(liste_command,"");
    }

    if (liste_command->premier == NULL){
        liste_command=initialisation();
        insertion(liste_command,"");
    }

    json_t *root = json_object();
    
    json_t *root_compilation = json_object();
    json_t *root_linking = json_object();
    


    json_t *json_arr_s = json_array();
    json_t *json_arr_h = json_array();
    json_t *json_arr_l = json_array();
    json_t *json_arr_f = json_array();
    json_t *json_arr_command = json_array();
  
  //link des array dans root
    json_object_set_new( root, "sources", json_arr_s );
    json_object_set_new( root, "headers", json_arr_h );
    json_object_set_new( root, "libraries", json_arr_l );
    json_object_set_new( root, "flags", json_arr_f );
    json_object_set_new( root, "executable_name", json_string(executable_name));



    if((valeur != 0) && (valeur < 3000)){
    json_object_set_new( root, "fari_error_msg",json_string("erreur lors de l'execution de fari") );
    }else{
    json_object_set_new( root, "fari_error_msg",json_string(fari_error_msg) );
    }


    json_object_set_new( root, "compilation", root_compilation);
         json_object_set_new( root_compilation, "commands",json_arr_command);
         if((valeur != 0) && (valeur < 3000)){
         json_object_set_new( root_compilation, "error_msg",json_string("erreur lors de la compilation"));    
         }else{
         json_object_set_new( root_compilation, "error_msg",json_string(error_msg0));              
         }

    char tableau_commande[1000]="";
      if (command == NULL)
    {
        exit(EXIT_FAILURE);
    }else{
       Element* actuel_commande_execution = command->premier;
       strcpy(tableau_commande,actuel_commande_execution->mot);

    }
    



int countcounter = 0 ;


Element *actuel_source_json = source->premier;

    while (actuel_source_json != NULL)
    {
      char delimiterc[]=".";

      char *test_is_objectfile;
      test_is_objectfile = strstr(actuel_source_json->mot, ".o");
      if (test_is_objectfile != NULL)
      {
        actuel_source_json->mot = strtok(actuel_source_json->mot, delimiterc);
        strcat(actuel_source_json->mot,".c");
      }
      countcounter++;
      actuel_source_json= actuel_source_json->suivant;
    }




    json_object_set_new( root, "linking", root_linking );
        json_object_set_new( root_linking, "command",json_string(tableau_commande));

    if((valeur != 0) && (valeur < 3000)){
              json_object_set_new( root_linking, "error_msg",json_string("erreur lors de l'édition des liens"));
    }else{
              json_object_set_new( root_linking, "error_msg",json_string(error_msg1));      
    }    


    if((valeur != 0) && (valeur < 3000)){
         json_object_set_new( root, "fari_msg", json_string("Compilation failed."));  
   }else{
        json_object_set_new( root, "fari_msg", json_string("Compilation terminée."));  
   }


liste_command = inversion_liste(liste_command);



ajout_array_json(json_arr_s, source);
ajout_array_json(json_arr_h, headers);
ajout_array_json(json_arr_l, libraries);
ajout_array_json(json_arr_f, flags);
ajout_array_json2(json_arr_command, liste_command);



  


  json_dump_file(root,"logs.json",JSON_INDENT(1));  


  json_decref(root);


}










void afficherListe(Liste *liste)
{
    if (liste == NULL)
    {
        exit(EXIT_FAILURE);
    }

    Element *actuel = liste->premier;

    while (actuel != NULL)
    {
        printf("%s-> ", actuel->mot);
        actuel = actuel->suivant;
    }
    printf("fin liste\n");
}

/*
void traitement_lignes_CH2(Liste * liste_traitement, char* token)
{

  token = strtok(NULL," ");

  if (strstr(token, "*")!=NULL){
    while( token != NULL ) {
      glob_t paths;
      int retval;
    
      paths.gl_pathc = 0;
      paths.gl_pathv = NULL;
      paths.gl_offs = 0;
   

      retval = glob( token, GLOB_NOCHECK | GLOB_NOSORT,
                   NULL, &paths );
      if( retval == 0 ) {
        int idx;
        
        for( idx = 0; idx < paths.gl_pathc; idx++ ) {
          //   printf( "[%d]: %s\n", idx, paths.gl_pathv[idx] );          
          insertion(liste_traitement,paths.gl_pathv[idx]);
        }
        
        globfree( &paths );

      } 

        token = strtok(NULL," "); 
        //traitement_lignes_CH2(liste_traitement,token);
    }
  }else{
      while( token != NULL ) {
      insertion(liste_traitement,token);
      token = strtok(NULL," "); 
      //traitement_lignes_CH2(liste_traitement,token);
   
      }
    }
    
}

*/


void traitement_lignes_CH2(Liste * liste_traitement, char* token)
{

  token = strtok(NULL," ");
    while( token != NULL ) {
      if (strstr(token, "*")!=NULL){
       glob_t paths;
       int retval;
    
        paths.gl_pathc = 0;
        paths.gl_pathv = NULL;
        paths.gl_offs = 0;
   

      retval = glob( token, GLOB_NOCHECK | GLOB_NOSORT,
                   NULL, &paths );
      if( retval == 0 ) {
        int idx;
        
        for( idx = 0; idx < paths.gl_pathc; idx++ ) {
          //   printf( "[%d]: %s\n", idx, paths.gl_pathv[idx] );          
          insertion(liste_traitement,paths.gl_pathv[idx]);
        }
        
        globfree( &paths );
 
      } 

      }else{
        insertion(liste_traitement,token);        
      }
           
      token = strtok(NULL," "); 

    }
      

}




void traitement_lignes_BF(Liste * liste_traitement, char* token){
token = strtok(NULL," ");
while( token != NULL ) {
      insertion(liste_traitement,token);
      token = strtok(NULL," ");    

   }

}




void traitement_lignes_BCFH(Liste * liste_traitement, char* token){
token = strtok(NULL," ");
while( token != NULL ) {
  //printf( "tok %s tok \n", token );
      insertion(liste_traitement,token);
      token = strtok(NULL," ");    // parse chaque ligne

   }

}





void traitement_lignes_CH(Liste* liste_chsource, Liste* liste_chheaders,char* tokench){
  tokench = strtok(NULL," ");
  while( tokench != NULL ) {

    char *Chaine;
    Chaine = strstr(tokench, ".c");
      if (Chaine != NULL){
        insertion(liste_chsource,tokench);

      }else{
         insertion(liste_chheaders,tokench);
      }
      tokench = strtok(NULL," ");    // parse chaque ligne
    }
}



void traitement_lignes_CH_v2(Liste** liste_chsource, Liste** liste_chheaders,char* token,int indice){
  token = strtok(NULL," ");
  while( token != NULL ) {

    char *Chaine;
    Chaine = strstr(token, ".c");
      if (Chaine != NULL){
        insertion(liste_chsource[indice],token);

      }else{
         insertion(liste_chheaders[indice],token);
      }
      token = strtok(NULL," ");   
    }
 

}

/* 
 fonction qui permet de verifier si un fichier (que son nom est passé en paramètre) existe 
 et retourne sa date de dernière modification (st_mtime).
*/

time_t date_fichier(char* nom_headers){
    struct stat sb;
    time_t etatfichier = 0 ; 
    if (stat(nom_headers, &sb) == -1) {
        etatfichier = 123;
        perror("This file not found");
    }else{
      etatfichier =  sb.st_mtime;
    }

    return etatfichier;
}


time_t date_fichier2(char* nom_headers){
    struct stat sb;
    time_t etatfichier = 0 ; 
    if (stat(nom_headers, &sb) == -1) {
        etatfichier = 123;
       /// perror("The file not found");
    }else{
      etatfichier =  sb.st_mtime;
    }

    return etatfichier;
}




/* 
  une fonction qui permet de tester si chaque fichier d'entete dans le farile
  et rend par valeur de retour le maximum des st_mtime de ces fichiers
*/
time_t traitement_datef(Liste *liste_headers){



    if (liste_headers == NULL)
    {
        exit(EXIT_FAILURE);
    }

    Element *actuel = liste_headers->premier;
    time_t max=0;
    time_t a;


    while (actuel != NULL)
    {
      a = date_fichier(actuel->mot);
       if(a != 123){  // on teste si le fichier d'entete existe
        if (a>max){
          max=a;
        }
       }
        actuel = actuel->suivant;


    }

    return max;

}


tableau* traitement_datec(Liste *liste_sources, Liste* liste_h,Liste** liste_chheader, Liste** liste_chsource, int aa){
   //  int testruiz = 0;

    int ligneCH= 0;

    if ((liste_chheader[0] != NULL)  && (liste_chsource[0] != NULL)){
        if ((liste_chheader[0]->premier != NULL) && (liste_chsource[0]->premier != NULL) ) {
          ligneCH++;
        }

    }
 
    if (liste_sources == NULL)
    {
        exit(EXIT_FAILURE);
    }

    Element *actuelc = liste_sources->premier;
    Element *actuelcc1 = liste_sources->premier;
    time_t maxo=0;
    time_t haha; // varible qui va servir à calculer le  temps d'un fichier source
    int nbr = 0; 

    tableau* liste_etat_maxo = malloc(sizeof(tableau));
    while (actuelcc1 != NULL){
      nbr++;
       actuelcc1= actuelcc1->suivant;
    }

    liste_etat_maxo->a = nbr;
  //  printf(" nb vaut cette valeur : %d\n", nbr );

    liste_etat_maxo->tableauint = malloc(sizeof(int)*(nbr+1));
    liste_etat_maxo->x=0;
    int walid[nbr+1];
  
    //int resultats=550;
    int gg=0;
    while (actuelc != NULL)
    {
      haha = date_fichier(actuelc->mot); 
      if (haha != 123){    // on teste si le fichier source existe
        char delim[] = ".";
        char testsource[1000];
        strcpy(testsource, actuelc->mot);
        char* ptrtoken = strtok(testsource, delim);
        strcat(ptrtoken,".o");
        time_t o;
        o = date_fichier(ptrtoken);
        if (o != 123){  // on teste si le fichier objet existe
          if (o<haha){
             walid[gg]=1;
          }else{
            time_t h;
            h=traitement_datef(liste_h);
            if(o>h){
          //   testruiz++;
            //  printf("je viens de rentrer dans cette section : %d \n", testruiz);
             walid[gg]=0;
            }else{
              walid[gg]=1;
            }
          }
          if (o>maxo){
            maxo=o;
          }
        }else{
          walid[gg]=2;
        }
      }  
      actuelc = actuelc->suivant;
      gg++;      
    }
    for(int i=0; i<nbr; i++){
      liste_etat_maxo->tableauint[i]=walid[i];
    }
    liste_etat_maxo->x = maxo;
    return liste_etat_maxo;

}









int compilation_totale(Liste* liste_c, Liste* liste_f,Liste* liste_command){
  Element *actuel = liste_c->premier;
  Element *actuel2 = liste_f->premier;
  Element *actuel3 = liste_f->premier;
  int status;
  int etat_compilation = 0;
  int f=0;
  while(actuel2 != NULL){
    actuel2 = actuel2->suivant;
        f++;
  }


    char* arg[6+f];
    arg[5+f]=NULL;
    arg[0]= "gcc"; 
    arg[1]= "-c";
    arg[3]="-o";


    for (int s=1; s<f+1; s++){
      arg[4+s]=actuel3->mot;
      actuel3= actuel3->suivant;
    }
    
    while (actuel != NULL){
        arg[2]= actuel->mot;
      
        char delimiterc[] = ".";
        char* tokentoken = malloc(sizeof(char)*100);
        strcpy(tokentoken,actuel->mot);
        tokentoken = strtok(tokentoken, delimiterc);
        strcat(tokentoken,".o");
        arg[4]=tokentoken;

        //for(int l=0;l<(f+6);l++){printf("LES ARGUMENTS ZFJBSJDJLSF%d : %s\n",l,arg[l]);}

      if(fork()==0){
          const char* commande="gcc";
          execvp(commande,arg);
          perror("Erreur de compilation");
          exit(0);
      } else{

          waitpid(-1,&status,0);
          
          if(!WIFEXITED(status) && (WEXITSTATUS(status) != 0) ){
             //printf("gcc:  erreur de compilation \n");
            etat_compilation++;
          }


            char tokencommande[10000] = "gcc ";

            for(int ll1 =1 ;ll1<f+1;ll1++){
                strcat(tokencommande,arg[4+ll1]);
                strcat(tokencommande," ");

            }
            strcat(tokencommande,"-c ");
            strcat(tokencommande, arg[2]);
            strcat(tokencommande," ");
            strcat(tokencommande,"-o ");
            strcat(tokencommande, arg[4]);
         //   printf("Le voilà  :  %s\n", arg[4]);
           // strcat(tokencommande,"\0");

          //  printf("Voici la commande totale à stocker : %s\n", tokencommande);
            insertion(liste_command,tokencommande);
          
      }   
        
            actuel = actuel->suivant;
    }
    return etat_compilation;
}


int compilation_simple(char* fichier_source, Liste* liste_f,Liste* liste_command){
    int f=0;
    int statussimple;
    int etat_compilation_simple = 0;
    Element *actuel2 = liste_f->premier;
    Element *actuel1 = liste_f->premier;

    while(actuel2 != NULL){
      actuel2 = actuel2->suivant;
        f++;
      }
      //printf("The value of f is : %d\n",f );
      char* arg[6+f];
      arg[0]= "gcc"; 

     int kk=f;

      while((kk!=0) && (actuel1 != NULL)){
        arg[kk]=actuel1->mot;
        kk--;
        actuel1 = actuel1->suivant;
      }

      arg[1+f]= "-c";
      char delimiter0[] = ".";
      char *suiteChainesource;
    // On cherche la première occurrence de ".o" dans fichier_source :
    suiteChainesource = strstr(fichier_source, ".o");
    if (suiteChainesource != NULL)
    {
      char* tok = strtok(fichier_source, delimiter0);
      strcat(tok,".c");
      strcpy(fichier_source,tok);
    }
      char tableauchar[strlen(fichier_source)];
      strcpy(tableauchar,fichier_source);
      arg[2+f]= tableauchar;
      arg[3+f]= "-o";
      char  delimiter[] = ".";
      char* tokeno = strtok(fichier_source, delimiter);
      strcat(tokeno,".o");
      arg[4+f]=tokeno;
      arg[5+f]=NULL;
      
      
    //  for(int l=0;l<(f+6);l++){printf("LES ARGUEMENTS DE MON TABEAUX %d : %s\n",l,arg[l]);}


      if(fork()==0){
          const char*commande="gcc";
         // printf("Le problème est ici \n");
          execvp(commande,arg);
          perror("Erreur de compilation");
          exit(0);
      }
          waitpid(-1,&statussimple,0);
          
          if(!WIFEXITED(statussimple) && (WEXITSTATUS(statussimple) != 0) ){
             etat_compilation_simple++;
          }

            char tokcom[10000] = "gcc ";

            for(int slsl =f ;slsl>0;slsl--){
                strcat(tokcom,arg[slsl]);
                strcat(tokcom," ");

            }
            strcat(tokcom,"-c ");
            strcat(tokcom, arg[2+f]);
            strcat(tokcom," ");
            strcat(tokcom,"-o ");
            strcat(tokcom, arg[4+f]);
       //     strcat(tokcom,"\0");
//printf("Voici la commande simple à stocker %s\n", tokcom);
            insertion(liste_command,tokcom);
            return etat_compilation_simple;

}


int compilation_executable(Liste* liste_c, char* nom_exec, Liste* liste_b, Liste* liste_f,Liste* command){
  
  int etat_edition = 0 ;
  int status0;
  Element *actuelb = liste_b->premier;
  Element *actuelf = liste_f->premier;
  Element *actuelo1 = liste_c->premier;
  Element *actuelb1 = liste_b->premier;
  Element *actuelf1 = liste_f->premier;

  int a=0; // compteur pour calculer la taille de liste_o
  int f=0; // compteur pour calculer la taille de liste_f
  int b=0; // compteur pour calculer la taille de liste_b
  int io=0;
    while (actuelo1 != NULL){
      actuelo1 = actuelo1->suivant;
      a++;
    }
    free(actuelo1);
    Element *actuelo = liste_c->premier;

    while (actuelb1 != NULL){
      actuelb1 = actuelb1->suivant;
      b++;
    }

    while (actuelf1 != NULL){
      actuelf1 = actuelf1->suivant;
      f++;
    }


    char* wal[a+f+b+4];
    wal[0]="gcc";
    wal[1]="-o";
    wal[2]=nom_exec;
    wal[a+f+b+3]=NULL;
 
   
    while ((actuelo != NULL) && (io<a)){
         io++;
        char delim[] = ".";
        char* ptr = strtok(actuelo->mot, delim);
        strcat(ptr,".o");
      //  printf("Good preperation : %s\n", ptr);
        wal[io+2]= ptr;
        actuelo = actuelo->suivant;

    }

    int ib=0;
  
    while ((actuelb != NULL) && (ib<b)){
        ib++;
        wal[ib+io+2]= actuelb->mot;
        actuelb = actuelb->suivant;

    }

    int iff=0;
    while ((actuelf != NULL) && (iff<f)){
        iff++;
        wal[iff+ib+io+2]= actuelf->mot;
        actuelf = actuelf->suivant;

    }
   
    if(fork()==0){
      const char*c="gcc";
      execvp(c,wal);
      perror("Erreur de compilation");

    }else{
          waitpid(-1,&status0,0);
         if(!WIFEXITED(status0) || (WEXITSTATUS(status0) != 0) ){
                 etat_edition++;
          }      
         

        char tokencommande_exec[10000] = "";
        strcat(tokencommande_exec, "gcc ");
        for (int iff1=1;iff1<iff+1;iff1++){
          strcat (tokencommande_exec, wal[iff1+ib+io+2]);
          strcat(tokencommande_exec, " ");
        }
        strcat(tokencommande_exec, "-o ");
        strcat(tokencommande_exec,nom_exec);
        for (int iff2=1;iff2<io+1;iff2++){
          strcat(tokencommande_exec, " ");
          strcat (tokencommande_exec, wal[iff2+2]);
        }
        for (int ibb2=1;ibb2<ib+1;ibb2++){
          strcat(tokencommande_exec, " ");
          strcat (tokencommande_exec, wal[io+ibb2+2]);
        }
        //strcat (tokencommande_exec, "\0");
        insertion (command, tokencommande_exec);

     //   printf("Voici la commande de l'édition des liens:  %s\n", tokencommande_exec);

    }
        return etat_edition;
         
}



int traitement_compilation_et_recompilation(tableau* liste_etat_maxo, Liste* liste_h, Liste* liste_c, Liste* liste_f, Liste* liste_b, char* nom_exec,Liste * liste_command, Liste* command, Liste** liste_chsource, Liste** liste_chheader, int aa){
  Liste* liste_objets = initialisation();

  Element* actuel_objet = liste_c->premier;
  while(actuel_objet != NULL){ 
      char* tokenoo;
      char  delimitertokeno[] = ".";
      tokenoo = strtok(actuel_objet->mot, delimitertokeno);
      strcat(tokenoo,".o");
      insertion(liste_objets,tokenoo);
      //printf(" il est comme ça :  %s\n", liste_objets->premier->mot);
      actuel_objet = actuel_objet->suivant;
  }
  


 // printf("Je suis dans cette fonction\n");
  int etatfari=0;
  int nbrelt=0;
  //nbrelt=liste_etat_maxo->a;

  Element *actuelnbrelt = liste_c->premier;
  while(actuelnbrelt != NULL){
    nbrelt++;
    actuelnbrelt = actuelnbrelt->suivant;
  }

//  printf("voici la valeur de a : %d\n", nbrelt);
 // time_t lionelmessi = 0;
  struct stat sb;

  Element *actuel = liste_c->premier;
  
   stat(nom_exec, &sb);
   time_t tpsexec =sb.st_mtime;
   //printf("Test 1 \n");
   time_t maxh=traitement_datef(liste_h);
 // printf("Test 2 \n");

    int ligneCHindice= 0;

    if ((liste_chheader[0] != NULL)  && (liste_chsource[0] != NULL)){
        if ((liste_chheader[0]->premier != NULL) && (liste_chsource[0]->premier != NULL) ) {
          ligneCHindice++;
        }

    }

      int hchange = 0 ;
      
        if(ligneCHindice!=0){        
              for(int cx0=0;cx0<aa;cx0++){
                time_t maxhch0 = 0; 
                maxhch0 = traitement_datef(liste_chheader[cx0]);
                if (maxhch0>tpsexec){
                  hchange++;
                }
              }
        }  

   if((maxh<tpsexec)&&(hchange==0)){
       // printf("rentrerwalid \n" );

      // printf("Je suis dans cette fonction2\n");
    for(int j=0; j<nbrelt; j++){
      //printf(" liste_etat_maxo->tableauint[j]   :  %d\n", liste_etat_maxo->tableauint[j]);
      if(liste_etat_maxo->tableauint[j]>=1){   // on cherche le fichier source à recompiler
        for(int k=0; k<j;k++){
          actuel=actuel->suivant;
        //  printf("le fichier à recompiler : %s\n", actuel->mot);
        }

        char *suiteChaine;
        suiteChaine = strstr(actuel->mot, ".o");
        if(suiteChaine != NULL){
          char delim[] = ".";
          char* ptr = strtok(actuel->mot, delim);
          strcat(ptr,".c");
          strcpy(actuel->mot,ptr);
             
        }
     //   printf("Compilation 1  : %s \n", actuel->mot);
        int yx=0;

        yx = compilation_simple(actuel->mot, liste_f,liste_command); //printf("liste_command WALID : %s\n",  liste_command->premier->mot);
  // recompilation d'un fichier source en particulier si le .o est plus récent qu'un .h ou du .c correspondant
        if(yx!=0){
            etatfari++;
        }
      //  printf("La valeur de la compilation simple %d\n", yx);
      }

      time_t maximum_objets = 0;
       
     // for (int kks=0; kks<nbrelt; kks++){
       //   printf(" les éléments objets :  %ld\n", liste_objets->);
      //}

      maximum_objets = traitement_datef(liste_objets);
     // printf(" this is the value  :  %ld\n", tpsexec);
      //printf("Entrée là \n %s", command->premier->mot);
     // command = initialisation();
     // printf("C est fait lala\n");
      if(maximum_objets>tpsexec){
      //   printf("anthony joshua \n");
        int alphabeta=0;

        alphabeta =compilation_executable(liste_c, nom_exec, liste_b, liste_f, command);

        if(alphabeta != 0){
          etatfari++;
        }
      //  printf("La valeur prévue de sortie est : %d\n", etatfari);
      }


        //free(examsystem);

     //walid  }
    }
      // printf("Compilation 34 \n");

  }else{   

  // le cas où un fichier .h a été modifié on relance toute les compilations et on refait l'édition des liens avec le  -o et on réappel la fonction 
            //traitement mais on prend on considération les fichiers CH.
     // printf("Compilation 33 \n");

     Element *actuel13 = liste_c->premier;
     for(int data=0;data<nbrelt;data++){


     char *suiteChaine;
        suiteChaine = strstr(actuel13->mot, ".o");
        if(suiteChaine != NULL){
          char delim[] = ".";
          char* ptr = strtok(actuel13->mot, delim);
          strcat(ptr,".c");
          strcpy(actuel13->mot,ptr);
             
        }
        actuel13=actuel13->suivant;
     }       
    // printf("Compilation 3  \n");


    if (maxh<tpsexec){
      if((ligneCHindice!=0)&& (hchange!=0)){
         for(int cx=0;cx<aa;cx++){
              time_t maxhch = 0; 
              maxhch = traitement_datef(liste_chheader[cx]);
              if (maxhch > tpsexec ){
                Element * actuelaa = liste_chsource[cx]->premier;
                while(actuelaa != NULL){
                   //printf("c'est un fichier CH : %s\n", actuelaa->mot);
                   int x=0;
                  x = compilation_simple(actuelaa->mot, liste_f,liste_command);
                  if (x!=0){
                    etatfari++;
                  }
                  actuelaa= actuelaa->suivant;
                }
             }
           }
         }

    }else{//((hchange == 0){
       int y=0;
       y=compilation_totale(liste_c,liste_f,liste_command);
       if(y!=0){
        etatfari++;
       }
    }




    Element * actuel_listesource = liste_c->premier;
    while(actuel_listesource != NULL){
      // printf("la liste des sources corompue : %s\n", actuel_listesource->mot );
       actuel_listesource = actuel_listesource->suivant;
    }

    int xy = 0;
    xy =compilation_executable(liste_c, nom_exec, liste_b, liste_f,command);
    if(xy != 0){
      etatfari++;
    }
  }
 // printf("Cette valeur de etatfari est : %d\n", etatfari );
  //printf("liste_command WALID : %s\n",  liste_command->premier->mot);

  return etatfari;

}





int parseur(FILE *fichier){
  char* line = NULL;
  size_t len =0;
  ssize_t read;
  char* token;
 // char* token1;

  int etatfarifinal = 0;

  //ligne E
  char* nom_exec =(char*)malloc(100);
  int cmp=0;
  int existjava=0;

  //initialisation liste
  Liste * liste_c = initialisation();
  Liste * liste_b = initialisation();
  Liste * liste_h = initialisation();
  Liste * liste_f = initialisation();
  Liste * liste_j = initialisation();
  Liste *liste_command=initialisation();



char* fari_error_msg="";
Liste * command=initialisation();
char * error_msg0="";
char* error_msg1="";
char* fari_msg="";




  
  if (fichier==NULL){
    etatfarifinal++;
  }
    /*

   int counter_CH=0;
   
   while ((read = getline(&line0, &len0, fichier)) != -1) {
    token1=strtok(line,"\n");
    token1= strtok(line," ");
    if(strcmp(token1,"CH")==0){
          counter_CH++;
    }

   }*/



  Liste* liste_chsource[10000]; //[counter_CH];
  Liste* liste_chheader[10000]; //[counter_CH];
  int indiceCH=0;

   
   for (int ch=0;ch<10000;ch++){
      liste_chsource[ch]=initialisation();
      liste_chheader[ch]=initialisation();
   }
  
  while ((read = getline(&line, &len, fichier)) != -1) {   // récupère chaque ligne
    token=strtok(line,"\n");

    token= strtok(line," ");

    //reconnaissance du type de ligne

    if(strcmp(token,"C")==0){
      //traitement_lignes_BCFH(liste_c,token);
      traitement_lignes_CH2(liste_c,token);
      // printf("Sort de C\n");
    }

    else if(strcmp(token,"H")==0){
     //traitement_lignes_BCFH(liste_h,token);
      traitement_lignes_CH2(liste_h,token);
    // printf("Sort de H\n");

    }

    else if(strcmp(token,"F")==0){
     // traitement_lignes_BF(liste_f,token);
      traitement_lignes_BF(liste_f,token);
      // printf("Sort de F\n");

    }

    else if(strcmp(token,"B")==0){
    //traitement_lignes_BF(liste_b,token);
     traitement_lignes_BF(liste_b,token);
         //   printf("Sort de B\n");

    }

    else if(strcmp(token,"E")==0){

      cmp=cmp+1;
      token = strtok(NULL," ");
      strcpy(nom_exec,token);
      if ((token = strtok(NULL," "))!=NULL){
         cmp=cmp+1;
      }

             //   printf("Sort de E\n");


    } else if(strcmp(token,"CH")==0){
      //traitement_lignes_CH_v2(liste_chsource, liste_chheader,token,indiceCH);
        char* tokentokench = strtok(NULL," ");
         while( tokentokench != NULL ) {
           if(strstr(tokentokench,"*") == NULL){
              char *Chaine;
              Chaine = strstr(tokentokench, ".c");
              if (Chaine != NULL){
             // printf("JE suis LALA  .c %s\n", tokentokench);
              insertion(liste_chsource[indiceCH],tokentokench);
               // printf("hahwha source %s\n",liste_chsource[indiceCH]->premier->mot );


              }else{
              //  printf("JE suis LALA .h %s\n", tokentokench);

                insertion(liste_chheader[indiceCH],tokentokench);
                //printf("hahwha header %s\n",liste_chheader[indiceCH]->premier->mot );
              }
          }else{
                  glob_t paths;
                  int retval;
    
                  paths.gl_pathc = 0;
                  paths.gl_pathv = NULL;
                  paths.gl_offs = 0;
   
                  retval = glob( token, GLOB_NOCHECK | GLOB_NOSORT,NULL, &paths );
                  if( retval == 0 ) {
                     int idx;
        
                     for( idx = 0; idx < paths.gl_pathc; idx++ ) {
                 //   printf( "[%d]: %s\n", idx, paths.gl_pathv[idx] );          
                      if (strstr(tokentokench,".c")){
                        insertion(liste_chsource[indiceCH],paths.gl_pathv[idx]);

                      }else{
                        insertion(liste_chheader[indiceCH],paths.gl_pathv[idx]);                         
                      }

                  }
        
               globfree( &paths );

             } 
          }
            tokentokench = strtok(NULL," ");
            // parse chaque ligne
          }
                          indiceCH++;

    }else if(strcmp(token,"J")==0){

      traitement_lignes_BF(liste_j,token);
      existjava++;

    }

    else if(strcmp(token,"#")==0 ||strcmp(token,"\n")==0 ){
    }


    else{
      etatfarifinal++;
     perror("Ligne non valide dans le fichier de description");
    }




  }





  liste_c = inversion_liste(liste_c);
  liste_b = inversion_liste(liste_b);
  liste_h = inversion_liste(liste_h);
  liste_f = inversion_liste(liste_f);
  liste_j = inversion_liste(liste_j);


      if (existjava == 0){
          if (cmp==1){ // un seul fichier exec est spécifié

    // printf("\n le parsing fonctionne\n nom : %s\n",nom_exec );
    //   printf(" liste_c\n");
    // afficherListe(liste_c);
    // printf(" liste_b\n");
    // afficherListe(liste_b);
    // printf(" liste_f\n");
    // afficherListe(liste_f);
    // printf(" liste_h\n");
    // afficherListe(liste_h);

    
              

        int etatfari=0;
        struct stat hp;
        if (stat(nom_exec, &hp) == -1){
                 //   printf("Je suis dans le if1\n");
          int zzz=0;
         zzz= compilation_totale(liste_c, liste_f,liste_command);
         if(zzz != 0 ){
          etatfari++;
         }
          //printf("Je suis dans le if2\n");
          //printf("Bonjifbjbvqsklvqld");
         int xyz = 0;
         xyz= compilation_executable(liste_c, nom_exec, liste_b, liste_f,command);
        if(xyz != 0 ){
          etatfari++;
         }
        }else{

           tableau* walid=malloc(sizeof(tableau));
                    //  printf("Je suis dans le else1\n");

           walid = traitement_datec(liste_c,liste_h,liste_chsource,liste_chheader,indiceCH);
                                // printf("Je suis dans le else2\n");
           int azer=0;
          suppression(liste_command);
          liste_command = initialisation();
           azer=traitement_compilation_et_recompilation(walid,liste_h,liste_c,liste_f,liste_b,nom_exec,liste_command,command,liste_chsource,liste_chheader,indiceCH);
                //    printf("Je suis dans le else3\n");
            if(azer != 0 ){
              etatfari++;
            }      

         }
           Element* actuelfichiersheaders = liste_h->premier;
           int testcaserreur = 0;
           while(actuelfichiersheaders != NULL){
              time_t tesh = 0;
              tesh = date_fichier2(actuelfichiersheaders->mot);
              if(tesh == 123){
                testcaserreur++;
              }
              actuelfichiersheaders = actuelfichiersheaders->suivant;
           }


           if(testcaserreur != 0){
             etatfarifinal =  etatfarifinal +3000;
           }

           if (etatfari!=0){
               etatfarifinal++;
           }
           
        }else{
          if(cmp==0){
            perror("Fichier d'execution non spécifié");

          }else{
            perror("Un seul fichier d'execution doit être spécifié");
          }
          etatfarifinal++;
        }
        

       generate_json(liste_h,liste_c,liste_b,liste_f,nom_exec,fari_error_msg, command,liste_command,error_msg0,error_msg1,fari_msg,etatfarifinal);

    }else{
      Element *actueljava = liste_j->premier;
      Element *actueljava2= liste_j->premier;

        int javacounter = 0;
        int statusjava;

 
        while (actueljava != NULL)
      {   
        javacounter++;
            actueljava = actueljava->suivant;
        }
        char* argjava[javacounter+2];
        argjava[1+javacounter]=NULL;
        argjava[0]= "javac";


        int javaindex=1;
        while (actueljava2 != NULL)
      {
        argjava[javaindex]=actueljava2->mot;
          javaindex++;
          actueljava2 = actueljava2->suivant;

        }
        if(fork()==0){
          const char* commandejava="javac";
            execvp(commandejava,argjava);
            perror("Erreur de compilation");
            exit(0);
        }else{
          //wait(NULL);
          waitpid(-1,&statusjava,0);
         if(!WIFEXITED(statusjava) || (WEXITSTATUS(statusjava) != 0) ){
            etatfarifinal++;
          }   
        }  

    }

      if (line){
        free(line);
      }
   


     


      for (int sk=0; sk<10000; sk++){
        suppression(liste_chsource[sk]);
        suppression(liste_chheader[sk]);
      }
      suppression(liste_c);
      suppression(liste_f);
      suppression(liste_b);
      suppression(liste_h);
      //suppression(liste_chsource);
      // suppression(liste_chheaders);
      suppression(liste_j);


      return etatfarifinal;
}





int  main(int argc, char* argv[]){
  int continuation_erreur = 0;
    FILE * fp;
    if(argc>1){


      for(int i=1;i<argc;i++){
        if (strcmp(argv[i],"-k")==0){
          continuation_erreur=i;
       //   printf("continuation_erreur  \n");
        }
      }



    if (continuation_erreur!=0){                                // continuation erreur
      continuation_erreur=-1*(continuation_erreur-3);

      if(argc==3){
              fp = fopen(argv[continuation_erreur], "r+");
      if ( fp == NULL ) {
      //  fprintf(stderr, "Value of errno: %d\n", errno);
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
     //   printf("La valeur de sortie %d\n", errno);
        exit(errno);
      }else{
        int b = 0; 
        b= parseur(fp);
      //  printf("La valeur de sortie %d\n", b);
        exit(b);
      }


      }
    if (argc==2){

            fp = fopen("farifile", "r+");
      // début

    

      if ( fp == NULL ) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }else{
        //début
       int a = 0;
       a= parseur(fp);
      // printf("La valeur de sortie %d\n", a);
      exit(a);
      }


    }



    }
    else{

        fp = fopen(argv[1], "r+");
      if ( fp == NULL ) {
      //  fprintf(stderr, "Value of errno: %d\n", errno);
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(errno);
      }else{
        int b = 0; 
        b= parseur(fp);
       // printf("La valeur de sortie %d\n", b);
        exit(b);
      }


    }




    }

    else{
      fp = fopen("farifile", "r+");
      // début

    

      if ( fp == NULL ) {
        fprintf(stderr, "Error opening file: %s\n", strerror(errno));
        exit(EXIT_FAILURE);
      }else{
        //début
       int a = 0;
       a= parseur(fp);
      //printf("La valeur de sortie %d\n", a);

       exit(a);
      }
    }
     

    //return 0;
}                                                                                                                                                                              
    //return 0;
