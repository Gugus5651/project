#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include "DictMatch.h"

void freeDictMatch(DictMatch* dico) {
    if (dico == NULL) return;

    free(dico->language);

    for (size_t i = 0; i < dico->word_count; i++) {
        if (dico->matching_words[i] != NULL) {
            free(dico->matching_words[i]);
        }
    }
    free(dico);
}


/*essaye de faire le find best correction pour plusieurs mot
on donne un dictionnaire et il rend un DictMatch avec les MAX_WORDS premiers mots de la plus petites distance
 (ou plus grand matching avec jaro_winkler ici)
*/
DictMatch* find_best_correction_multi(char* s, Dictionary_t* dict) {
    //On initialise la structure, on ne la remplira qu'à la fin. On va utiliser un tableau dynamique pendant la fonction à cause des nombreux changement possible
    

    //on verifie que tout sinisialise bien 
    if (s == NULL || dict == NULL) {
        return NULL;
    }
    DictMatch* dm = malloc(sizeof(DictMatch));
    if (dm == NULL) {
        return NULL;
    }
    if (!init_dictmatch(dm)) {
        free(dm);
        return NULL;
    }

    // best_score = meilleur score, best_word = liste des meilleurs mot (max MAX_WORDS), len_s = taille du mot à corriger, status  = nombre de mot dans best_word
    
    double best_score = -1.0;
    char* best_word[MAX_WORDS] = {NULL};
    size_t len_s = strlen(s);
    int status = 0;

    //on parcours tout le dictionnaire
    for (uint32_t i = 0 ; i < dict -> word_count ; i++) {

        char* current_word = dict -> words[i];

        //si le mot s et le current word on plus de 3 de différence niveau longueur, on ne calcul même pas
        if (abs((int) len_s - (int) strlen(current_word)) > 3) continue;
        

        double current_score = jaro_winkler_distance(s, current_word);

        //si le meilleur score est plus grand que l'ancien (avec un minimum de delta), on reset best_word.
        //et on rajoute le meilleur mot dans best_word
        if (current_score > best_score+ 1e-9) {
            best_score = current_score;
                for (int k = 0; k < MAX_WORDS; k++) {
                    best_word[k] = NULL;
                }
            best_word[0] = current_word;
            status =
            
            1;
        }
        //si ils sont égaux, alors on ajoute à la liste, attention, status doit être inférieur à max word, sinon on a trop de best word comparé à ce que l'on demande
        else if(fabs(current_score - best_score) < 1e-9 && status<MAX_WORDS){
            best_word[status] =current_word;
            status+=1;
        }
    }

    // on transforme pour mettre dans la heap
    dm->dist = best_score;
    dm->word_count = status;
    dm-> language=strdup(dict->lang);

    // on vérifie que c'est bien initialiser
    if (dm->language == NULL) {
        free(dm);
        return NULL;
    }
    // on initialise les mots. on vérifie que c'est bien initialiser, si ça marche pas, on efface
    for (int i = 0; i < status; i++) {
        dm->matching_words[i] = strdup(best_word[i]);
        if (dm->matching_words[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(dm->matching_words[j]);
            }
            free(dm->language);
            free(dm);
            return NULL;
        }
    }
    if (status < MAX_WORDS) {
        dm->matching_words[status] = NULL;
    }
    return dm;
}


/*
Fonction qui pour un ensemble de dic, va tester chaqu'un des dictionnaire et ressortir les 3 meilleurs. C'est à dire qu'il va chercher les meilleurs mots dans
chaque dic et ressortir
*/
LanguageMatch* find_best_language_one_word(char*s, Dictionary_t* dict[]){
    if (s == NULL || dict == NULL) {
        return NULL;
    }
    LanguageMatch* dm = malloc(sizeof(LanguageMatch));
    if (dm == NULL) {
        return NULL;
    }
    if (!init_languagematch(dm,s)) {
        free(dm);
        return NULL;
    }
// Il faudrait peut être rajouter des vérifications pour éviter tout type d'indice négatifs ou autres andouillerie du genre



    // nombredico = nombre de dico, peut être à optimiser car on le calcul à chaque mots
    int nombredico =0;
    while (dict[i] != NULL) {
        i++;  //regarder si il ne faut pas créer dict tel que taille +1
    }

    // là où on va répartir les différents dico. j'ai laissé indesirable de taille i et pas i-MAX_DICTS car pas envie d'avoir une erreur de taille nég
    DictMatch* indesirable[i] = {NULL};
    int a =0;
    DictMatch* troismeilleurs[MAX_DICTS] = {NULL};
    float worstdistanceinmeilleurs = 1.0;

    //status permet de directe retenir les trois premier, pour gain de temps
    int status =0;

    //ici on boucle sur tout les dicos qui n'ont pas été null avant.
    for (int j =0; j<nombredico; j++){
        DictMatch* actual = find_best_correction_multi( s,  dict[j]);

        if (actual == NULL) {
            // si un dico la première function plante, on free tout avant de return NULL
            for (int k = 0; k < status; k++) {
                freeDictMatch(troismeilleurs[k]);
            }
            free(dm->word);
            free(dm);
            return NULL;
        }

        if(status <MAX_DICTS){
            troismeilleurs[status] = actual;
            status +=1;

            /*quand il est rempli, on trouve le worst distance, cela nous permet de directement enlever les dico qui essayerais
            de rentrer avec une distance moins bonne dans les meilleurs.
            Cependant, ce système doit calculer à chaque fois qu'on rajoute un dico dans troismeilleur worst distance. ou encore de 
            devoir rechercher worst distance. 
            je pense qu'un algorithme qui trierais la liste nous aiderais beaucoup et éviterais de refouiller la liste. si les dico sont trier, on sait où chercher le pire.
            */
            if(status ==MAX_DICTS){
                worstdistanceinmeilleurs = troismeilleurs[0]->dist;
                for(int k =1; k<MAX_DICTS; k++){

                    //changer le sens si Jaro winkler
                    if(worstdistanceinmeilleurs> troismeilleurs[k]->dist){
                        worstdistanceinmeilleurs = troismeilleurs[k]->dist;
                    }
                }
            }
        }
        else{
            //changer le sens si pas Jaro winkler
            if(worstdistanceinmeilleurs < actual->dist){
                for(int k =0; k<MAX_DICTS; k++){
                    if(troismeilleurs[k]->dist == worstdistanceinmeilleurs){
                        indesirable[a] = troismeilleurs[k];
                        a+=1;
                        troismeilleurs[k] = actual;




                        //ici on recalcul la plus petite distance. très long, même optimisation qu'avant à faire
                        worstdistanceinmeilleurs = troismeilleurs[0]->dist;
                        for(int k =1; k<MAX_DICTS; k++){

                            //changer le sens si pas Jaro winkler
                            if(worstdistanceinmeilleurs> troismeilleurs[k]->dist){
                                worstdistanceinmeilleurs = troismeilleurs[k]->dist;
                            }
                        }

                        break;
                    }

                }
            }


            else{
                indesirable[a] = actual;
                a+=1;
            }

        }
    }
    for (int k = 0; k < a; k++){
        if (indesirable[k] != NULL) {
            freeDictMatch(indesirable[k]);
            free(indesirable[k]);
        }
    }
    if (status == 0) {
        dm->dict_count = 0;
        dm->matching_dicts[0].language = NULL;
        return dm;
    }

    dm->dict_count = status;
    for (int k = 0; k < status; k++) {
        dm->matching_dicts[k] = *troismeilleurs[k];
    }
    if (status < MAX_DICTS) {
        dm->matching_dicts[status].language = NULL;
    }
    
    return dm;
}