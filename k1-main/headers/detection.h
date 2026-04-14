#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "dict.h"
#include "common.h"
#include "vector.h"


#ifndef DETECTION_H
#define DETECTION_H

int compare(const char *mot1, const char *mot2);

bool binary2(char const **liste, int pos_bas, int pos_haut, char* word);

bool binary1(Dictionary_t *dict, char *word);

// Détecte les mots incorrects dans un fichier passé en entrée.
// Retourne 1 si la détection est réussie, et 0 si il y a eu une erreur.
int detect(Vector** indices, char *input_path, Dictionary_t** dicts, size_t* dict_count, char*** lines, uint32_t** lines_size, size_t* line_count);

uint32_t find_best_dict_for_line(const char* line, Dictionary_t* dicts, size_t dict_count);

int pretty_print_detection(char *line, uint32_t line_number, size_t word_count, uint32_t *word_indices);

#endif
