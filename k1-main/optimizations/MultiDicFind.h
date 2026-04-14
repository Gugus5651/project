#ifndef DICTMATCHMULTI_H
#define DICTMATCHMULTI_H

#include "DictMatch.h"
#include "dict.h"

DictMatch* find_best_correction_multi(char* s, Dictionary_t* dict);

LanguageMatch* find_best_language_one_word(char* s, Dictionary_t* dict[]);

void freeDictMatch(DictMatch* dico);

#endif