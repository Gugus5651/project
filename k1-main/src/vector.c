#include <stdlib.h>
#include <stdint.h>
#include "vector.h"


/**
 * Initialise un vecteur dynamique avec une capacité par défaut.
 * * Alloue la mémoire nécessaire pour le tableau interne avec une capacité 
 * initiale de 16 éléments et met la taille courante à 0.
 * * Complexité en temps : O(1).
 * Complexité en espace : O(1) (allocation d'un bloc de taille constante).
 * * @param v Pointeur vers la structure Vector à initialiser.
 * @return  1 en cas de succès, 0 en cas d'erreur (pointeur NULL ou échec d'allocation).
 */
int vector_init(Vector* v) {

    if (v == NULL) {
        return 0;
    }

    v -> size = 0;
    v -> capacity = 16;
    v -> data = malloc(v -> capacity * sizeof(uint32_t));

    if (v -> data == NULL) {
        return 0;
    }

    return 1;
}

/**
 * Ajoute un élément à la fin du vecteur dynamique.
 * * Algorithme de redimensionnement : si le vecteur a atteint sa capacité maximale, 
 * un nouveau bloc de mémoire de taille double (capacity * 2) est alloué via realloc(). 
 * Cela permet de garantir une complexité amortie constante.
 * * Complexité en temps : O(1) en moyenne (amortie), mais O(N) dans le pire des cas 
 * (lorsqu'une réallocation et copie des données sont nécessaires).
 * * @param v Pointeur vers le vecteur cible.
 * @param value La valeur entière (uint32_t) à ajouter.
 * @return 1 en cas de succès, 0 en cas d'erreur (pointeur NULL ou échec de réallocation).
 */
int vector_add(Vector* v, uint32_t value) {

    if (v == NULL) {
        return 0;
    }

    if (v -> size == v -> capacity) {
        
        size_t new_capacity = 2 * v->capacity;
        uint32_t* new_data = realloc(v -> data, new_capacity * sizeof(uint32_t));
        if (new_data == NULL) {
            return 0;
        }

        v -> data = new_data;
        v -> capacity = new_capacity;
    }

    v -> data[v -> size] = value;
    v -> size++;
    return 1;
}

/**
 * Libère la mémoire allouée pour le vecteur dynamique.
 * * Libère le tableau interne et réinitialise les compteurs (taille et capacité) à 0 
 * ainsi que le pointeur de données à NULL pour éviter les problèmes de "dangling pointers" 
 * (pointeurs fous) si le vecteur est réutilisé par erreur.
 * * Complexité en temps : O(1).
 * * @param v Pointeur vers le vecteur à détruire.
 * @return 1 en cas de succès, 0 si le pointeur passé en argument est NULL.
 */
int vector_free(Vector* v) {

    if (v == NULL) {
        return 0;
    }

    free(v -> data);
    v -> data = NULL;
    v -> size = 0;
    v -> capacity = 0;

    return 1;
}

