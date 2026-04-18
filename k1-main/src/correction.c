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

// Inclut la distance d'intérêt. Si on change la distance, on doit changer la condition 
// de la meilleure note dans find_best_correction() en conséquence.
#include "Distance-JW.h" 

/**
 * Trouve la meilleure correction pour un mot donné en utilisant la distance de Jaro-Winkler.
 * * L'algorithme filtre d'abord les mots ayant une différence de longueur supérieure à 3 
 * pour optimiser les performances avant de calculer la distance exacte.
 * * Complexité : O(N * M) où N est le nombre de mots dans le dictionnaire et M la longueur du mot.
 * @param mot Le mot mal orthographié à corriger.
 * @param dict Pointeur vers la structure de dictionnaire contenant le lexique de référence.
 * @return Un pointeur vers le mot corrigé dans le dictionnaire, ou NULL si aucune correction n'est trouvée/valide.
 */

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

/**
 * Trouve les corrections pour les mots mal orthographiés d'un texte et les stocke.
 * * L'algorithme procède ligne par ligne, copie chaque ligne pour la tokeniser de manière 
 * destructrice avec strtok(), puis cherche une correction pour les mots correspondant 
 * aux indices signalés comme fautifs. En cas d'échec d'allocation, toute la mémoire 
 * déjà allouée est proprement libérée (rollback) pour éviter les fuites.
 * @param correction  Pointeur de sortie (char****) qui recevra un tableau de tableaux de chaînes. 
 * correction[i][j] contiendra la suggestion pour la j-ème faute de la i-ème ligne.
 * @param lines       Tableau de chaînes de caractères représentant le texte source.
 * @param indices     Tableau de vecteurs indiquant les indices des mots mal orthographiés pour chaque ligne.
 * @param dicts       Tableau des dictionnaires disponibles.
 * @param line_count  Nombre total de lignes dans le texte.
 * @param dict_count  Nombre de dictionnaires disponibles.
 * @return            1 en cas de succès, 0 en cas d'erreur (paramètre NULL ou échec d'allocation).
 */
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

