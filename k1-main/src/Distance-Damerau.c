#include "../headers/Distance-Damerau.h"
#include <stddef.h>


/**
 * Retourne le minimum entre deux entiers.
 * Cette fonction est privée (static) car elle n'est utilisée qu'ici.
 * @param one Le premier entier.
 * @param two Le deuxième entier.
 * @return    La valeur minimale entre 'one' et 'two'.
 */
static int min(const int one, const int two){
    if(one == two){return one;}
    if(one > two){return two;}
    else {return one;}
}

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
float Distance_damerau(const char* mot1, const char* mot2){
    if(mot1==NULL || mot2 == NULL){
        return -1.0f;
    }
    
    // 2 boule while pour récupérer les longeurs des mots
    int longueur1 =0;
    int longueur2 =0;
    const char* pizza1 = mot1;
    const char* pizza2 = mot2;
    while(*pizza1 != '\0'){
        
            longueur1 +=1;
            pizza1 +=1;
        
    }
    while(*pizza2 != '\0'){
        
            longueur2 +=1;
            pizza2 +=1;
        
    }

    int tb[longueur1+1][longueur2+1];//crea d'un tableau de comparaison


    for(int i =0; i<=longueur1;i++){
        tb[i][0] = i;
    }

    for(int j =0; j<=longueur2;j++){
        tb[0][j] = j;
    }

    for(int i =1; i<=longueur1;i++){
        for(int j =1; j<=longueur2;j++){
        if(*(mot1+i-1)== *(mot2+j-1)){
            tb[i][j] = tb[i-1][j-1];
        }
        else if( j >1 && i>1 && *(mot1+i-1) == *(mot2+j-2) && *(mot1+i-2) == *(mot2+j-1) ){
            tb[i][j] = 1 + min(min(tb[i-1][j],tb[i][j-1]), min(tb[i-1][j-1],tb[i-2][j-2]));
        }
        else{
            tb[i][j] = 1 + min(min(tb[i-1][j],tb[i][j-1]), tb[i-1][j-1]);
        }
        }
    }
    float distancefinale;
    distancefinale = (float)tb[longueur1][longueur2];
    return distancefinale;
}
