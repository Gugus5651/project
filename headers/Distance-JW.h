#ifndef JARO_WINKLER_H
#define JARO_WINKLER_H

/**
 * Calcule la distance de Jaro-Winkler entre deux chaînes de caractères.
 * * L'algorithme de Jaro-Winkler mesure la similarité entre deux chaînes. 
 * Cette implémentation renvoie la distance (définie ici comme 1.0 - similarité).
 * Plus la valeur est proche de 0.0, plus les mots sont similaires.
 * La méthode calcule les correspondances dans une fenêtre glissante, compte les 
 * transpositions, applique la formule de Jaro, puis le bonus de préfixe de Winkler.
 * * Complexité en temps : O(N *min(M, R)) où N et M sont les longueurs des mots, 
 * et R est la taille de la fenêtre de recherche.
 * Complexité en espace : O(N + M) pour allouer les tableaux de correspondances.
 * @param s1 Pointeur vers la première chaîne de caractères.
 * @param s2 Pointeur vers la deuxième chaîne de caractères.
 * @return   Un flottant représentant la distance (0.0f pour identique, 1.0f pour différent).
 */
float Distance(const char *s1, const char *s2);

#endif 