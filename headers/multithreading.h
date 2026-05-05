#ifndef MULTITHREADING_H
#define MULTITHREADING_H

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "vector.h"
#include "queue.h"
#include "io.h"

/**
 *Arguments passés aux threads
 */
typedef struct {
    WorkQueue_t* q;             // file d'attente des tâches restantes
    Dictionary_t* dicts;        // tableau de dictionnares
    size_t dict_count;          // nombre de dictionnaires
    Vector* indices_array;      // vecteur des indices de mots incorrect par ligne
    char*** correction_array;   // vecteur des correction pas ligne
    uint32_t* best_dicts_array; // tableau des ID des meilleurs dictionnaires par ligne
    bool do_correction;         // true si mode == correction, false sinon 
    pthread_mutex_t* mutex;     // mutex pour éviter les accès concurrents 
    OutputStreams_t* streams;   // fichiers de sortie (NULL si output_path non spécifié)
} ThreadArgs_t;

// Signatures des fonctions (implèmentées dans le .c)
void* threadfct(void* argu);

/*
Fonction qui gère tt pour une ligne, elle utilise detect one line et corr one line
Elle reçoit une ligne et faut faire attention au format dans lequel il faut rendre.
Elle écrit dans coorections à sa propre ligne,
dans vector indicies à sa propre ligne
dict_count  = nombre de dictionnaires
*/
void traitement_ligne(char*** correction, char* line, Vector* indices, int linesize, Dictionary_t* dicts, size_t* dict_count, uint32_t* best_dict_out, bool do_correction);

#endif