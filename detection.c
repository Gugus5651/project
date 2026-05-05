#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "common.h"
#include "detection.h"
#include "vector.h"
#include "io.h"

int compare(const char *mot1, const char *mot2) {
    if (mot1 == NULL || mot2 == NULL) {
        return 0; 
    }

    return strcmp(mot1, mot2);
}

bool binary2(char const **liste, int pos_bas, int pos_haut, char* word) {

    if (pos_bas > pos_haut) return false;

    int middle = (pos_bas + pos_haut) / 2;

    int res = compare(word, liste[middle]);

    if (res == 0) return true;
    if (res < 0) return binary2(liste, pos_bas, middle - 1, word);
    else return binary2(liste, middle + 1, pos_haut, word);
}

bool binary1(Dictionary_t *dict, char *word) {

    if (dict == NULL || word == NULL) return false; 

    return binary2((const char**) dict -> words, 0, dict -> word_count-1, word);
}

int count_correct_words(const char* line, Dictionary_t* dict) {
    char* line_copy = strdup(line);
    int compteur_correct = 0;
    
    // On prend mot par mot
    char* token = strtok(line_copy, " \n");
    while (token != NULL) {
        if (binary1(dict, token)) { 
            compteur_correct++;
        }
        token = strtok(NULL, " \n");
    }
    
    free(line_copy);
    return compteur_correct;
}

uint32_t find_best_dict_for_line(const char* line, Dictionary_t* dicts, size_t dict_count) {
    uint32_t meilleur_index = 0;
    int max_correct = -1;

    for (size_t d = 0; d < dict_count; d++) {
        int current_correct = count_correct_words(line, &dicts[d]);
        
        if (current_correct > max_correct) {
            max_correct = current_correct;
            meilleur_index = (uint32_t) d;
        }
    }
    return meilleur_index;
}

int detect(Vector** indices, char *input_path, char* dict_path, Dictionary_t** dicts, size_t* dict_count, char*** lines, uint32_t** lines_size, size_t* line_count, uint32_t** best_dicts) {

    if (load_dictionaries(dict_path, dicts, dict_count) != 0) {
        printf("Erreur de chargement du dictionaire\n");
        return 0;
    }

    if (read_input_file(input_path, lines, lines_size, line_count) != 0) {
        printf("Erreur de lecture du fichier input\n");
        return 0;
    }

    Vector* bad_indices = malloc(*line_count * sizeof(Vector));
    if (bad_indices == NULL) return 0;

    uint32_t* best_dicts_arr = malloc(*line_count * sizeof(uint32_t));
    if (best_dicts_arr == NULL) {
        free(bad_indices);
        return 0;
    }

    for (size_t i = 0 ; i < *line_count ; i++) {

        uint32_t best_dict_idx = find_best_dict_for_line((*lines)[i], *dicts, *dict_count);
        best_dicts_arr[i] = best_dict_idx;

        char* line_copy = strdup((*lines)[i]);
        if (line_copy == NULL) {
            for (size_t j = 0 ; j < i ; j++) {
                vector_free(&bad_indices[j]);
            }
            free(bad_indices);
            free(best_dicts_arr);
            return 0;
        }

        if (vector_init(&bad_indices[i]) != 1) {
            free(line_copy);
            for (size_t j = 0 ; j < i ; j++) {
                vector_free(&bad_indices[j]);
            }
            free(bad_indices);
            free(best_dicts_arr);
            return 0;
        }

        uint32_t word_index = 0;
        char* token = strtok(line_copy, " ");
        while (token != NULL) {

            if (!binary1(&(*dicts)[best_dict_idx], token)) {
                if (vector_add(&bad_indices[i], word_index) != 1) {
                    free(line_copy);
                    for (size_t j = 0 ; j < i ; j++) {
                        vector_free(&bad_indices[j]);
                    }
                    free(bad_indices);
                    free(best_dicts_arr);
                    return 0;
                }
            }

            word_index++;
            token = strtok(NULL, " ");
        }
        free(line_copy);
    }
    *indices = bad_indices;
    *best_dicts = best_dicts_arr;
    return 1;
}

int linedetect(Vector* indices, char *line, int linesize, Dictionary_t* dicts, size_t dict_count, uint32_t* best_dict) {
    if (indices == NULL || line == NULL || linesize <= 0 || dicts == NULL || best_dict == NULL || dict_count == 0) {
        return 0;
    }

    if (vector_init(indices) != 1) {
        return 0;
    }

    // On trouve le meilleur dictionnaire pour la ligne.
    uint32_t best_dict_idx = find_best_dict_for_line(line, dicts, dict_count);
    *best_dict = best_dict_idx;

    int word_index = 0;
    int pos = 0;

    while (pos < linesize && line[pos] != '\0') {
        // On skip les caractères terminaux et les espaces.
        while (pos < linesize && line[pos] != '\0' && isspace((unsigned char) line[pos])) {
            pos++;
        }
        if (pos >= linesize || line[pos] == '\0') {
            break;
        }

        int start = pos;
        while (pos < linesize && line[pos] != '\0' && !isspace((unsigned char) line[pos])) {
            pos++;
        }
        int end = pos;
        // Calcul de la longueur du mot.
        int len = end - start;

        if (len <= 0) {
            continue;
        }

        char* token = malloc((size_t)len + 1);
        if (token == NULL) {
            vector_free(indices);
            return 0;
        }
        // On copie le mot courant dans token, on en fait un String valide.
        memcpy(token, line + start, (size_t)len);
        token[len] = '\0';

        bool correct = binary1(&dicts[best_dict_idx], token);

        // On gère à part le premier mot, pour la majuscule au début de phrase.
        if (!correct && word_index == 0) {  
            char token_lower[len + 1];
            memcpy(token_lower, token, len + 1);
            // On regarde si le 1er mot avec sa 1ere lettre en minuscule est dans le dictionnaire.
            token_lower[0] = tolower((unsigned char) token_lower[0]);
            correct = binary1(&dicts[best_dict_idx], token_lower);
        }

        // Gestion des mots classiques, on les ajoute au vector indices si ils sont incorrects.
        if (!correct) {
            if (vector_add(indices, word_index) != 1) {
                free(token);
                vector_free(indices);
                return 0;
            }
        }

        free(token);
        word_index++;
    }

    return 1;
}