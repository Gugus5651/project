#include "Distance-Hamming.h"
#include <stddef.h>



/**
 * Calcule la distance de Hamming généralisée entre deux chaînes de caractères.
 * * Contrairement à la distance de Hamming classique qui exige des mots de même longueur,
 * cette implémentation accepte des mots de longueurs différentes. 
 * Elle compare d'abord les caractères à des positions identiques. Ensuite, si une chaîne 
 * est plus longue que l'autre, chaque caractère supplémentaire ajoute 1 à la distance.
 * * Complexité en temps : O(N) où N est la longueur de la chaîne la plus longue.
 * Complexité en espace : O(1) car la vérification se fait en place sans allocation.
 * * @param mot1 Pointeur vers la première chaîne de caractères.
 * @param mot2 Pointeur vers la deuxième chaîne de caractères.
 * @return La distance calculée sous forme de flottant, ou -1.0f en cas d'erreur (pointeur NULL).
 */
float Distance_hamming(char const *mot1 , char const *mot2){
    // si il y a un problème renvoie -1 pour signaler l'erreur
    if (mot1 == NULL || mot2 == NULL) {
        return -1;
    }
    else {
        // tant que l'on est pas arrivé à la fin d'un des deux mot on fait hamming classique
        // on compare le premier caractère de mot 1 avec le premier de mot deux puis le deuxième avec le deuxième et ainsi de suite
        int i = 0;
        float distance = 0;
        while(*(mot1+i) != '\0' && *(mot2+i) != '\0'){
                if(*(mot1+i)!=*(mot2+i)){
                    distance += 1;
                }
                i+=1;
        }
        // s'ils sont de longueur différentes on ajoute 1 pour chaque caractère en trop
        if(*(mot1+i) == '\0'){
            while(*(mot2+i) != '\0'){
                distance +=1;
                i +=1;
            }
        }
        else if(*(mot2+i) == '\0'){
            while(*(mot1+i) != '\0'){
                distance +=1;
                i +=1;
            }
        }
        return distance;
    }

}
// ce programme n'est pas très malin mais il devrait fonctionner en O(n) et être donc rapide