#ifndef DISTANCE_LEVENSHTEIN_H
#define DISTANCE_LEVENSHTEIN_H

/**
 * @brief Retourne le minimum entre trois nombres flottants.
 * * Cette fonction est privée (static) car elle n'est qu'un utilitaire 
 * interne nécessaire à la récursion de l'algorithme de Levenshtein.
 * * @param one Le premier flottant.
 * @param two Le deuxième flottant.
 * @param three Le troisième flottant.
 * @return La valeur minimale parmi les trois paramètres.
 */
static float min3float(const float one, const float two , const float three);

/**
 * @brief Calcule la distance de Levenshtein entre deux chaînes de caractères.
 * * La distance de Levenshtein représente le nombre minimum d'opérations 
 * (insertion, suppression ou substitution) requises pour transformer une chaîne 
 * en une autre. Cette implémentation utilise une approche récursive.
 * * Complexité en temps : O(3^max(N, M)) où N et M sont les longueurs des mots. 
 * Complexité en espace : O(max(N, M)) correspondant à la profondeur de la pile d'appels.
 * * @param mot1 Pointeur vers la première chaîne de caractères.
 * @param mot2 Pointeur vers la deuxième chaîne de caractères.
 * @return La distance calculée sous forme de flottant, ou -1.0f en cas d'erreur.
 */
float Distance_levenshtein(const char *mot1, const char *mot2);

/**
 * @brief Calcule récursivement la distance de Levenshtein avec un accumulateur.
 * * Fonction utilitaire récursive (privée). Elle compare les caractères courants 
 * et s'appelle elle-même en explorant les trois opérations possibles (substitution, 
 * insertion, suppression) lorsque les caractères diffèrent.
 * Cet algorithme a une complexité temporelle exponentielle O(3^max(N, M)).
 * * @param mot1 Pointeur vers la position courante dans la première chaîne.
 * @param mot2 Pointeur vers la position courante dans la deuxième chaîne.
 * @param distanceacc La distance accumulée jusqu'à présent dans la branche récursive.
 * @return La distance totale calculée pour cette branche.
 */
static float DistanceAccumulateur(char const *mot1 , char const *mot2 , float distanceacc);

#endif