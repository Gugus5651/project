#ifndef DAMERAU_H
#define DAMERAU_H

/**
 * Retourne le minimum entre deux entiers.
 * Cette fonction est privée (static) car elle n'est utilisée qu'ici.
 * @param one Le premier entier.
 * @param two Le deuxième entier.
 * @return    La valeur minimale entre 'one' et 'two'.
 */
static int min(const int one, const int two);

/**
 * Calcule la distance de Damerau-Levenshtein entre deux chaînes de caractères.
 * * La distance de Damerau-Levenshtein est une mesure de similarité définie comme le 
 * nombre minimum d'opérations (insertion, suppression, substitution ou transposition 
 * de deux caractères adjacents) nécessaires pour transformer une chaîne en une autre.
 * * L'algorithme utilise la programmation dynamique en construisant une matrice de 
 * dimension (longueur1 + 1) x (longueur2 + 1).
 * * Complexité en temps : O(N * M) où N et M sont les longueurs des mots.
 * Complexité en espace : O(N * M) via l'allocation d'un tableau VLA sur la pile.
 * @param mot1 Pointeur vers la première chaîne de caractères (doit être terminée par '\0').
 * @param mot2 Pointeur vers la deuxième chaîne de caractères (doit être terminée par '\0').
 * @return     La distance calculée sous forme de flottant, ou -1.0f si une erreur survient (paramètre NULL).
 */
float Distance_damerau(const char* mot1, const char* mot2);

#endif
