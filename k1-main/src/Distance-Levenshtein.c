#include "Distance-Levenshtein.h"
#include <stddef.h>
// définition d'un min qui va servir dans DistanceAcc
float min3float(const float one, const float two , const float three){
    if(one <= two && one <= three ){return one;}
    if(two <= one && two <= three ){return two;}
    else {return three;}
}
// prototype de Distance accumulateur : il sera défini plus bas mais il doit y avoir un prototype pour pouvoir l'utiliser dans Distance
// static indique que distanceAccumulateur n'existe que dans ce fichier
static float DistanceAccumulateur(const char *mot1, const char *mot2, float distanceacc);
// implémente le distance de Levenshtein
float Distance_levenshtein(char const *mot1 , char const *mot2){
    // si il y a un problème renvoie -1 pour signaler l'erreur
    if (mot1 == NULL || mot2 == NULL) {
        return -1;
    }
    // s'il n'y a pas de problème on appelle la fonction DistanceAccumulateur qui a une troisième entrée : la longueur accumulée durant la récursion
    // l'intérêt de DistanceAccumulateur est de permettre d'utiliser Distance sans àvoir à donner 0 en arguent manuellement
    return DistanceAccumulateur(mot1 , mot2, 0);

}
// dans les faits c'est elle qui fait tous les calculs
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