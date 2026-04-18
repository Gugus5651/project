#ifndef CORRECTION_H
#define CORRECTION_H

#include "vector.h"
#include "common.h"

char* find_best_correction(char* s, Dictionary_t* dict);

int corrections(char**** correction, char** lines, Vector* indices, Dictionary_t* dicts, size_t line_count, size_t dict_count);

#endif