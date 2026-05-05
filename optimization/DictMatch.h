#ifndef DICTMATCH_H
#define DICTMATCH_H

#include "common.h"
#include <stdlib.h>
#include <stdint.h>


/*
Ici on définit deux constantes. Elles représentent la taille maximale des tableaux 
matching_words et matching_dicts. Elles servent juste à définir une taille au préalable pour
pouvoir initialiser les tableaux. Si en testant on voit qu'il faut augmenter/diminuer une
constante on peut le faire sans problème mais attention à la complexité spatiale du
programme.
*/
#define MAX_WORDS 3
#define MAX_DICTS 3


// Structure qui représente un dictionnaire "candidat" qui pourrait être dans la bonne langue.
typedef struct {

    char* language;                         // Langue du dictionnaire
    float dist;                             // Distance entre le mot et le dictionnaire 
    char* matching_words[MAX_WORDS];        // Les mots du dictionnaire qui sont à la plus petite distance du mot du texte. 
    size_t word_count;                      // Nombre de mots dans matching_words

} DictMatch; 

// Structure qui contient un mot et les dictionnaires qui lui sont le plus proches.
typedef struct {

    char* word;                              // Mot du texte sur base duquel on cherche un dictionnaire adapté
    DictMatch matching_dicts[MAX_DICTS];     // Liste de dictionnaires "candidats"
    size_t dict_count;                       // Nombre de dictionnaires dans matching_dicts

} LanguageMatch;

int init_dictmatch(DictMatch* dm);

int init_languagematch(LanguageMatch* lm, char* word);

int add_matchingword(DictMatch* dm, char* word);

int add_matchingdict(LanguageMatch* lm, DictMatch dict);


#endif 