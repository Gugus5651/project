#ifndef VECTOR_H
#define VECTOR_H

#include <stdlib.h>
#include <stdint.h>

// Implémente un "Vector" qui est l'équivalent en C d'une ArrayList en Java.

typedef struct {
    uint32_t* data;
    size_t size;
    size_t capacity;
} Vector;

// Initialise un Vector vide.
// Retourne 1 si l'initialisation est réussie, 0 sinon.
int vector_init(Vector* v);

// Ajoute un élément à v et double sa taille si il est size == capacity.
// Retourne 1 si l'ajout est réussie, 0 sinon.
int vector_add(Vector* v, uint32_t value);

// Libère la mémoire de v.
// Retourne 1 si la libération de mémoire est réussie, 0 sinon.
int vector_free(Vector* v);

#endif