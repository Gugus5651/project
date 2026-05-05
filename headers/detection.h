#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include "dict.h"
#include "common.h"
#include "vector.h"


#ifndef DETECTION_H
#define DETECTION_H

/**
* Prend les pointeurs vers des char 'mot1' et 'mot2'.
* Retourne 1 si la valeur est trouvée, 0 sinon.
* Si un ou deux des pointeurs sont NULL alors on obtient aussi un 0
* la complexité en temps est celle de strcmp et la complexité mémoire est de O(n) sur la stack
*/
int compare(const char *mot1, const char *mot2);

/*fonction binary2
ici on va chercher dans toute la liste tirée "liste" un mot "word". 
pour cela on va utiliser l'algorithme binary search.
On utilise la fonction compare. compare(a,b) renvoie 0 si a ==b, 1 si a plus petit et -1 si a plus grand.
"pos_bas" est le plus petit indice où "word" pourrait se situer , "pos_haut" le plus grand
complexité temporelle O(log n) , complexité spaciale O(1)
*/
bool binary2(char const **liste, int pos_bas, int pos_haut, char* word);

/*fonction binary1
ici on va chercher dans toute la liste tirée "liste" de taille size, un mot word. 
pour cela on va utiliser l'algorithme binary search.
On utilise la fonction compare. compare(a,b) renvoie 0 si a ==b, 1 si a plus petit et -1 si a plus grand.
"pos_bas" est le plus petit indice où "word" pourrait se situer , "pos_haut" le plus grand
complexité temporelle de binary2 , complexité spaciale O(1)
*/
bool binary1(Dictionary_t *dict, char *word);

// Combien de mots existent dans un dictionnaire donné
/*"line" une chaine de char
soit L la longueur de la ligne et 
K la taille du dictionnaire
complexité temporelle O(L log K) 
[plus la chaine est longue plus on aura des "token" et on fait binary1 sur chaque "token"]
 complexité spaciale O(n) [strdup alloue de la mémoire pour une copie de line]
 la mémoire est bien libérée après l'utilisation
*/

int count_correct_words(const char* line, Dictionary_t* dict);

/*"indices" un pointeur vers un pointeur de Vector, on l'utilisera pour pointer vers "bad_indices"
   "input_path" un pointeurs vers des char (string) pour accéder au fichier à lire
   "dicts" un pointeur vers un pointeur des dictionnaires qu'on va utiliser 
   pour trouver lequel correspond à quelle ligne
   "dict_count" un pointeur vers le nombre de dictionnaires qu'on utilise
   "lines" un triple pointeur ves des char (double pointeur vers des strings),
    on l'utilise pour stocker les lignes du fichier.
   "line_size" indique la taille des lignes
   "line_count" indique le nombre de lignes
   "best_dicts" est un double pointeur vers des int, 
   on l'utilisera pour stocker l'index des meilleurs dictionnaires pour chaque ligne
   soit L la longueur de la ligne  
   soit K la taille du dictionnaire
   soit a le nombre de dictionnaires
   soit N le nombre de lignes
   complexité temporelle O(N*a*L log K) : on observe que si L , K et a sont égaux on est en O(n^3)
   si on fait l'hypothèse que les dictionnaires sont plus ou moins fixés alors on est plutôt en O(n log n)
   complexité spatiale O(n) [le tableau de Vector de bad_indices + best_dicts_arr]
 */
int detect(Vector** indices, char *input_path, char* dict_path, Dictionary_t** dicts, size_t* dict_count, char*** lines, uint32_t** lines_size, size_t* line_count, uint32_t** best_dicts);

/*
fonction qui prends une ligne en argument et qui va ressortir un vecteur bad_inicies et best_dicts.
Pour des raisons de facilité, l'adresse du vecteur et du best dics sont déjà donné. ATTENTION : NE PAS UTILISER STRTOK
@param indices endroit où mettre le vecteur des bad indice, à remplir dans la fonction
@param line la ligne à traiter
@param linesize taille de la ligne à traiter
@param dicts liste de tout les dicos
@param dict_count nombre de dico
@param best_dicts liste des meilleurs dico, à remplir dans la fonction
return un int de réussite ou d'échec (à spécifier ici)
La fonction doit avoir réussi à remplir, indices, best_ditcs
*/
int linedetect(Vector* indices, char *line, int linesize, Dictionary_t* dicts, size_t dict_count, uint32_t* best_dict);

// Teste tous les dictionnaires et renvoie celui qui a le plus de mots justes
/*"line" une chaine de char
"dicts" un pointeur vers des dictionnaires
"dict_count" le nombre de dictionnaires
soit L la longueur de la ligne  
soit K la taille du dictionnaire
soit a le nombre de dictionnaires
complexité temporelle O(a*L log K) 
[la complexité de count_correct_words * le nombre de dictionnaires. 
Vu qu'on sait qu'il n'y a pas un grand nombre de dictionnaires on peut le voir comme une constante]
 complexité spaciale O(n) [c'est celle de count_correct_words]
*/
uint32_t find_best_dict_for_line(const char* line, Dictionary_t* dicts, size_t dict_count);

#endif
