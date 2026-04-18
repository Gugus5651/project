#include "Distance-Levenshtein.h"
#include <stddef.h>


/**
 * @brief Retourne le minimum entre trois nombres flottants.
 * * Cette fonction est privée (static) car elle n'est qu'un utilitaire 
 * interne nécessaire à la récursion de l'algorithme de Levenshtein.
 * * @param one Le premier flottant.
 * @param two Le deuxième flottant.
 * @param three Le troisième flottant.
 * @return La valeur minimale parmi les trois paramètres.
 */
static float min3float(const float one, const float two , const float three){
    if(one <= two && one <= three ){return one;}
    if(two <= one && two <= three ){return two;}
    else {return three;}
}
// prototype de Distance accumulateur : il sera défini plus bas mais il doit y avoir un prototype pour pouvoir l'utiliser dans Distance
// static indique que distanceAccumulateur n'existe que dans ce fichier
static float DistanceAccumulateur(const char *mot1, const char *mot2, float distanceacc);


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
float Distance_levenshtein(char const *mot1 , char const *mot2){
    // si il y a un problème renvoie -1 pour signaler l'erreur
    if (mot1 == NULL || mot2 == NULL) {
        return -1;
    }
    // s'il n'y a pas de problème on appelle la fonction DistanceAccumulateur qui a une troisième entrée : la longueur accumulée durant la récursion
    // l'intérêt de DistanceAccumulateur est de permettre d'utiliser Distance sans àvoir à donner 0 en arguent manuellement
    return DistanceAccumulateur(mot1 , mot2, 0);

}

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
static float DistanceAccumulateur(char const *mot1 , char const *mot2 , float distanceacc){
    // si un des deux mots est de longueur nulle la distance est la distance accumulée + la longueur (restante) de l'autre
    if (*mot1 == '\0'){
        float longueur = distanceacc;
        while (*mot2 != '\0'){
            mot2 +=1;
            longueur +=1;
        }
        return longueur;
        
    }
    if (*mot2 == '\0'){
        float longueur = distanceacc;
        while (*mot1 != '\0'){
            mot1 +=1;
            longueur +=1;
        }
        return longueur;
    }
    // si les deux mots ont le même caractère actuel alors on appelle récursivement DistanceAccumulateur sans incrémenter la distance accumulée
    if (*mot1 == *mot2){
        return DistanceAccumulateur((mot1+1) , (mot2+1) , distanceacc);
    }
    // sinon on prend le minimum entre les décalages des deux pointeurs ou d'un seul et on incrémente la distance accumulée de 1
    else {
        return min3float(DistanceAccumulateur((mot1+1) , (mot2+1) , distanceacc+1), DistanceAccumulateur((mot1) , (mot2+1) , distanceacc+1), DistanceAccumulateur((mot1+1) , (mot2) , distanceacc+1));

    }
    // cette implémentation est en O(3^n) n étant le nombre de lettres si je comprends bien 
    //à voir si c'est un algorithme suffisament rapide pour le projet

}