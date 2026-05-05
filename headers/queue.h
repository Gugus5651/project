#ifndef QUEUE_H
#define QUEUE_H

#include <pthread.h>
#include <stdint.h>
#include <stdbool.h>
#include "common.h"
#include "vector.h"

#define QUEUE_SIZE 100

// Tâche envoyée à un thread
typedef struct {
    char* line;
    uint32_t line_index;
    bool stop_thread;
} Task_t;

// File d'attente des threads (tickets de boucherie comme en Java)
typedef struct {
    Task_t tasks[QUEUE_SIZE];
    int head;
    int tail;
    int count;
    pthread_mutex_t lock;
    pthread_cond_t not_full;
    pthread_cond_t not_empty;
} WorkQueue_t;

// Initialise la file, head et tail a 0 et count aussi.
// Initialise les mutex et conditions. 
void init_queue(WorkQueue_t* q);

// Libère les ressources de la file, détruit les mutex.
void destroy_queue(WorkQueue_t* q);

// Ajoute une tache dans la file, bloque si elle est pleine.
void push_task(WorkQueue_t* q, Task_t task);

// Retire et retourne la prochaine tache, bloque si vide.
Task_t pop_task(WorkQueue_t* q);

#endif