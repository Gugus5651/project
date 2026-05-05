#ifndef DISTANCE_HAMMING_H
#define DISTANCE_HAMMING_H

/**
 * Calcule la distance de Hamming généralisée entre deux chaînes de caractères.
 * * Contrairement à la distance de Hamming classique qui exige des mots de même longueur,
 * cette implémentation accepte des mots de longueurs différentes. 
 * Elle compare d'abord les caractères à des positions identiques. Ensuite, si une chaîne 
 * est plus longue que l'autre, chaque caractère supplémentaire ajoute 1 à la distance.
 * * Complexité en temps : O(N) où N est la longueur de la chaîne la plus longue.
 * Complexité en espace : O(1) car la vérification se fait en place sans allocation.
 * @param mot1 Pointeur vers la première chaîne de caractères.
 * @param mot2 Pointeur vers la deuxième chaîne de caractères.
 * @return La distance calculée sous forme de flottant, ou -1.0f en cas d'erreur (pointeur NULL).
 */
float Distance_hamming(const char *mot1, const char *mot2);

#endif