#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "io.h"
#include "common.h"
#include "detection.h"
#include "vector.h"

int compare(const char *mot1, const char *mot2) {
    if (mot1 == NULL || mot2 == NULL) {
        return 0; 
    }

    return strcmp(mot1, mot2);
}

/*fonction binary 1 et 2
ici on va chercher dans toute la liste tirée "liste" de taille size, un mot word. 
pour cela on va utiliser l'algorithme binary search.
On utilise la fonction compare. compare(a,b) renvoie 0 si a ==b, 1 si a plus petit et -1 si a plus grand.
*/

bool binary2(char const **liste, int pos_bas, int pos_haut, char* word){

    if (pos_bas > pos_haut) return false;

    int middle = (pos_bas + pos_haut) / 2;

    int res = compare(word, liste[middle]);

    if (res == 0) return true;
    if (res < 0) return binary2(liste, pos_bas, middle - 1, word);
    else return binary2(liste, middle + 1, pos_haut, word);
}

bool binary1(Dictionary_t *dict, char *word){

    if (dict == NULL || word == NULL) return false; 

    return binary2((const char**) dict -> words, 0, dict -> word_count-1, word);
}

int pretty_print_detection(char *line, uint32_t line_number, size_t word_count, uint32_t *word_indices) {

    if (line == NULL || word_indices == NULL) {
        return -1;
    }

    printf("%u", line_number);
    for (size_t i = 0 ; i < word_count ; i++) {
        printf(",%u", word_indices[i]);
    }
    printf("\n");

    return 0;
}

// Combien de mots existent dans un dictionnaire donné
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

// Teste tous les dictionnaires et renvoie celui qui a le plus de mots justes
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


int detect(Vector** indices, char *input_path, Dictionary_t** dicts, size_t* dict_count, char*** lines, uint32_t** lines_size, size_t* line_count) {

    if (load_dictionaries(DEFAULT_DICT_PATH, dicts, dict_count) != 0) {
        printf("Erreur de chargement du dictionaire\n");
        return 0;
    }

    if (read_input_file(input_path, lines, lines_size, line_count) != 0) {
        printf("Erreur de lecture du fichier input\n");
        return 0;
    }

    Vector* bad_indices = malloc(*line_count * sizeof(Vector));
    if (bad_indices == NULL) return 0;

    for (size_t i = 0 ; i < *line_count ; i++) {

        uint32_t best_dict_idx = find_best_dict_for_line((*lines)[i], *dicts, *dict_count);

        char* line_copy = strdup((*lines)[i]);
        if (line_copy == NULL) {
            for (size_t j = 0 ; j < i ; j++) {
                vector_free(&bad_indices[j]);
            }
            free(bad_indices);
            return 0;
        }

        if (vector_init(&bad_indices[i]) != 1) {
            free(line_copy);
            for (size_t j = 0 ; j < i ; j++) {
                vector_free(&bad_indices[j]);
            }
            free(bad_indices);
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
                    return 0;
                }
            }

            word_index++;
            token = strtok(NULL, " ");
        }
        free(line_copy);
    }
    *indices = bad_indices;
    return 1;
}
