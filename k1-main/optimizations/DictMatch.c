#include <stdlib.h>
#include <stdint.h>
#include "DictMatch.h"

/* 
Initialise un DictMatch à partir de son adresse. Par défaut tout est vide.
On initialise un DictMatch avec son adresse (DictMatch* dm) pour que la fonction 
modifie vraiment le DictMatch original. 
Si on passait juste (Dictmatch dm), la fonction modifierait juste une copie du DictMatch passé 
en argument et il resterait inchangé à l'extérieur de la fonction. 
Retourne 0 en cas d'échec.
*/
int init_dictmatch(DictMatch* dm) {

    if (dm == NULL) return 0;

    dm -> language = NULL;
    dm -> dist = 0;
    dm -> word_count = 0;

    for (size_t i = 0 ; i < MAX_WORDS ; i++) {
        dm -> matching_words[i] = NULL;
    }

    return 1;
}

/*
Initialise un LanguageMatch. Tout est vide par défaut.
On initialise un LanguageMatch avec son adresse pour la même raison qu'avec un DictMatch.
Retourne 0 en cas d'échec.
*/
int init_languagematch(LanguageMatch* lm, char* word) {

    if (lm == NULL || word == NULL) return 0;

    lm -> word = strdup(word);
    if (lm -> word == NULL) {
        return 0;
    }

    lm -> dict_count = 0;

    for (size_t i = 0 ; i < MAX_DICTS ; i++) {
        init_dictmatch(&lm -> matching_dicts[i]);
    }

    return 1;
}

/*
Ajoute un mot proche du mot du texte dans la liste des matching_words. 
Retourne 0 en cas d'échec.
*/
int add_matchingword(DictMatch* dm, char* word) {

    if (dm == NULL || word == NULL) return 0;

    if (dm -> word_count == MAX_WORDS) {
        printf("Longueur maximale de matching_words atteinte !");
        return 0;
    }

    dm -> matching_words[dm -> word_count++] = strdup(word);
    return 1;
}

/*
Ajoute un dictionnaire "candidat" à la liste matching_dicts.
Retourne 0 en cas d'échec.
*/
int add_matchingdict(LanguageMatch* lm, DictMatch dict) {

    if (lm == NULL) return 0;

    if (lm -> dict_count == MAX_DICTS) {
        printf("Longueur maximale de matching_dicts atteinte !");
        return 0;
    }

    lm -> matching_dicts[lm -> dict_count++] = dict;
    return 1;
}

