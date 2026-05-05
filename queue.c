#include "queue.h"

void init_queue(WorkQueue_t* q) {
    q->head = 0;
    q->tail = 0;
    q->count = 0;

    pthread_mutex_init(&q->lock, NULL);
    pthread_cond_init(&q->not_full, NULL);
    pthread_cond_init(&q->not_empty, NULL);
}


void destroy_queue(WorkQueue_t* q) {
    // detruire le mutex et les conditions
    pthread_mutex_destroy(&q->lock);
    pthread_cond_destroy(&q->not_full);
    pthread_cond_destroy(&q->not_empty);
}

void push_task(WorkQueue_t* q, Task_t task) {
    pthread_mutex_lock(&q->lock);

    // On attend que la file ne soit plus pleine
    while (q->count == QUEUE_SIZE) {
        pthread_cond_wait(&q->not_full, &q->lock);
    }

    q->tasks[q->tail] = task;
    q->tail = (q->tail + 1) % QUEUE_SIZE;  
    q->count++;

    // Signaler qu'il y a quelque chose
    pthread_cond_signal(&q->not_empty);
    pthread_mutex_unlock(&q->lock);
}


Task_t pop_task(WorkQueue_t* q) {
    pthread_mutex_lock(&q->lock);

    // Attendre qu'il y ait au moins une tache
    while (q->count == 0) {
        pthread_cond_wait(&q->not_empty, &q->lock);
    }

    Task_t task = q->tasks[q->head];
    q->head = (q->head + 1) % QUEUE_SIZE;
    q->count--;

    pthread_cond_signal(&q->not_full);
    pthread_mutex_unlock(&q->lock);

    return task;
}