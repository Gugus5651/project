#include "../headers/Distance-Damerau.h"
#include <stddef.h>
int min(const int one, const int two){
    if(one == two){return one;}
    if(one > two){return two;}
    else {return one;}
}

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
