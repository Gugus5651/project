#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <ctype.h>
#include <limits.h>

#include "common.h"
#include "detection.h"
#include "vector.h"
#include "dict.h"

// Include la distance d'intérêt, si on change la distance on doit changer find_best_correction()
// en conséquence.
#include "Distance-JW.h" 

char* find_best_correction(char* s, Dictionary_t* dict) {

    if (s == NULL || dict == NULL) {
        return NULL;
    }

    double best_score = 10.0; //score élevé car, un score élevé veut dire un mot incorrect 
    char* best_word = NULL;
    size_t len_s = strlen(s);

    for (uint32_t i = 0 ; i < dict -> word_count ; i++) {

        char* current_word = dict -> words[i];
        if (abs((int) len_s - (int) strlen(current_word)) > 3) continue;

        double current_score = Distance(s, current_word);
        if (current_score < best_score) {
            best_score = current_score;
            best_word = current_word;
        }
    }

    return best_word;
}

int corrections(char**** correction, char** lines, Vector* indices, Dictionary_t* dicts, size_t line_count, size_t dict_count) {

    if (correction == NULL || lines == NULL || indices == NULL || dicts == NULL) return 0;

    char*** corr = malloc(line_count * sizeof(char**));
    if (corr == NULL) return 0;

    for (size_t i = 0 ; i < line_count ; i++) {

        uint32_t best_dict_idx = find_best_dict_for_line(lines[i], dicts, dict_count); //nombre de dico (2)

        size_t word_count = indices[i].size;
        corr[i] = malloc(word_count * sizeof(char*));
        if (corr[i] == NULL) {
            for (size_t j = 0 ; j < i ; j++) {
                for (size_t k = 0 ; k < indices[j].size ; k++) {
                    free(corr[j][k]);
                }
                free(corr[j]);
            }
            free(corr);
            return 0;
        }

        char* line_copy = strdup(lines[i]);
        if (line_copy == NULL) {
            for (size_t j = 0 ; j <= i ; j++) {
                for (size_t k = 0 ; k < (j < i ? indices[j].size : 0) ; k++) {
                    free(corr[j][k]);
                }
                free(corr[j]);
            }
            free(corr);
            return 0;
        }

        char* token = strtok(line_copy, " ");
        size_t word_index = 0; 
        size_t bad_word_index = 0;

        while (token != NULL && bad_word_index < word_count) {
            if (word_index == indices[i].data[bad_word_index]) {
                char* suggestion = find_best_correction(token, &dicts[best_dict_idx]);
                corr[i][bad_word_index] = suggestion ? strdup(suggestion) : NULL;
                bad_word_index++;
            }
            word_index++;
            token = strtok(NULL, " ");
        }

        free(line_copy);
    }

    *correction = corr;
    return 1;
}

int pretty_print_correction(char *line, uint32_t line_number, size_t word_count, uint32_t *word_indices, char **corrections) {

    if (line == NULL || (word_indices == NULL && word_count > 0) || (corrections == NULL && word_count > 0)) {
        return -1;
    }

    printf("[%u] ",line_number);

    char* pos = line;
    uint32_t current_word = 0;

    while (*pos) {

        // On saute les espaces 
        if (isspace((unsigned char) *pos)) {  // On cast *pos pour avoir une valeur entre 0 et 255 pour isspace().
            putchar(*pos++);
            continue;
        }

        // On arrive au début d'un mot
        char* word_start = pos;
        while (*pos && !isspace((unsigned char) *pos)) {
            pos++;
        }

        char* replacement = NULL;
        for (size_t i = 0 ; i < word_count ; i++) {
            if (word_indices[i] == current_word && corrections[i] != NULL) {
                replacement = corrections[i];
                break;
            }
        }

        if (replacement) printf(GREEN "%s" RESET, replacement);
        else fwrite(word_start, 1, pos - word_start, stdout);

        current_word++;

    }

    printf("\n");

    return 0;
}
