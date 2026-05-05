#ifndef CORRECTION_H
#define CORRECTION_H

#include "vector.h"
#include "common.h"

/**
 * Trouve la meilleure correction pour un mot donné en utilisant la distance de Jaro-Winkler.
 * * L'algorithme filtre d'abord les mots ayant une différence de longueur supérieure à 3 
 * pour optimiser les performances avant de calculer la distance exacte.
 * * Complexité : O(N * M) où N est le nombre de mots dans le dictionnaire et M la longueur du mot.
 * @param mot Le mot mal orthographié à corriger.
 * @param dict Pointeur vers la structure de dictionnaire contenant le lexique de référence.
 * @return Un pointeur vers le mot corrigé dans le dictionnaire, ou NULL si aucune correction n'est trouvée/valide.
 */
char* find_best_correction(char* s, Dictionary_t* dict);

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
int corrections(char**** correction, char** lines, Vector* indices, Dictionary_t* dicts, size_t line_count, size_t dict_count);

/*correction one line
fonction qui prends une ligne à corriger en entrée, cette fonction sera déjà passé par detect one line.
@param correction : correction (en i) de la ième faute dans la ligne
@param line la ligne à traiter
@param indices endroit où on doit corriger les mots
@param dict dico choisit
return un int de réussite ou d'échec (à spécifier ici)
La fonction doit avoir réussi à remplir correction
*/
int linecorrections(char*** correction, char* line, Vector* indices, Dictionary_t* dict);

#endif