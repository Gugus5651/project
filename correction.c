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
#include "correction.h"

// Inclut la distance d'intérêt. Si on change la distance, on doit changer la condition 
// de la meilleure note dans find_best_correction() en conséquence.
#include "Distance-JW.h" 

char* find_best_correction(char* mot, Dictionary_t* dict) {

    if (mot == NULL || dict == NULL) {
        return NULL;
    }

    // score élevé car, un score JW élevé veut dire un mot incorrect 
    double best_score = 10.0; 
    char* best_word = NULL;
    size_t len_s = strlen(mot);

    for (uint32_t i = 0 ; i < dict -> word_count ; i++) {

        char* current_word = dict -> words[i];

        // on ignore les mots dont la longueur diffère de plus de 3 caractères
        if (abs((int) len_s - (int) strlen(current_word)) > 3) continue;

        double current_score = Distance(mot, current_word);
        if (current_score < best_score) {
            best_score = current_score;
            best_word = current_word;
        }
    }

    return best_word;
}


int corrections(char**** correction, char** lines, Vector* indices, Dictionary_t* dicts, size_t line_count, size_t dict_count) {

    if (correction == NULL || lines == NULL || indices == NULL || dicts == NULL) return 0;

    // Allocation du tableau principal (une entrée par ligne)
    char*** corr = malloc(line_count * sizeof(char**));
    if (corr == NULL) return 0;

    for (size_t i = 0 ; i < line_count ; i++) {

        uint32_t best_dict_idx = find_best_dict_for_line(lines[i], dicts, dict_count); //nombre de dico (2)

        size_t word_count = indices[i].size;

        // Allocation du sous-tableau pour les corrections de la ligne i
        corr[i] = malloc(word_count * sizeof(char*));
        if (corr[i] == NULL) {
            // Rollback : libération de la mémoire allouée précédemment en cas d'échec
            for (size_t j = 0 ; j < i ; j++) {
                for (size_t k = 0 ; k < indices[j].size ; k++) {
                    free(corr[j][k]);
                }
                free(corr[j]);
            }
            free(corr);
            return 0;
        }

        // On duplique la ligne car strtok la modifie en remplaçant les espaces par des '\0'
        char* line_copy = strdup(lines[i]);
        if (line_copy == NULL) {
            // Rollback complet
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

        // Parcours des mots de la ligne
        while (token != NULL && bad_word_index < word_count) {
            // Si le mot courant est identifié comme une faute on cherche une correction
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


int linecorrections(char*** correction, char* line, Vector* indices, Dictionary_t* dict) {
    if (correction == NULL || line == NULL || indices == NULL || dict == NULL ) {
        return 0;
    }

    size_t bad_word_count = indices->size;
    
    if (bad_word_count == 0) {
        *correction = NULL;
        return 1;
    }

    char** corr = malloc(bad_word_count * sizeof(char*));
    if (corr == NULL) {
        return 0;
    }

    size_t word_index = 0;
    size_t bad_word_index = 0;
    int pos = 0;

    while (line[pos] != '\0' && bad_word_index < bad_word_count) {
        while (line[pos] != '\0' && isspace((unsigned char) line[pos])) {
            pos++;
        }
        if (line[pos] == '\0') {
            break;
        }

        int start = pos;
        
        while (line[pos] != '\0' && !isspace((unsigned char) line[pos])) {
            pos++;
        }
        int end = pos;
        int len = end - start;

        if (len <= 0) {
            continue;
        }

        if (word_index == indices->data[bad_word_index]) {
            
            char token[len + 1];
            memcpy(token, line + start, (size_t)len);
            token[len] = '\0';

            char* suggestion = find_best_correction(token, dict);
            
            if (suggestion != NULL) {
                corr[bad_word_index] = strdup(suggestion);
                if (corr[bad_word_index] == NULL) {
                    for (size_t j = 0; j < bad_word_index; j++) {
                        free(corr[j]);
                    }
                    free(corr);
                    return 0;
                }
            } else {
                corr[bad_word_index] = NULL;
            }
            bad_word_index++;
        }
        
        word_index++;
    }

    if (bad_word_index != bad_word_count) {
        for (size_t j = 0; j < bad_word_index; j++) {
            free(corr[j]);
        }
        free(corr);
        return 0;
    }

    *correction = corr;
    return 1;
}