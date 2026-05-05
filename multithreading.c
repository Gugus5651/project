#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "vector.h"
#include "common.h"
#include "detection.h"
#include "correction.h"
#include "queue.h"
#include "multithreading.h"


void* threadfct(void *argu){
    ThreadArgs_t * arg = (ThreadArgs_t *) argu;
    while (true){

        /*
        On bloque la queue, s'il n'y a pas de tâche dans queue, on débloque la queue et on 
        destroy, sinon, on : 
        -) prend une tache
        -) débloque la queue
        -) traite la tâche
        -) recommence
        */

        pthread_mutex_lock(arg->mutex);
        if(arg->q->count < 1){
            pthread_mutex_unlock(arg->mutex);
            pthread_exit(NULL);
        }
    
        Task_t tache = pop_task(arg->q);
        int linesize = strlen(tache.line);
        pthread_mutex_unlock(arg->mutex);
        traitement_ligne(&arg->correction_array[tache.line_index] , tache.line, &arg->indices_array[tache.line_index], linesize, arg-> dicts, &(arg->dict_count), &arg->best_dicts_array[tache.line_index], arg -> do_correction);


        // Ces lignes servent à écrire au fur et à mesure, s'il y a un crash on ne perd rien.
        if (arg -> streams != NULL) {
            pthread_mutex_lock(arg -> mutex);
            write_detection(arg -> streams, tache.line_index, arg -> best_dicts_array[tache.line_index], arg -> indices_array[tache.line_index].size, arg -> indices_array[tache.line_index].data);
            if (arg -> do_correction) {
                write_correction(arg -> streams, arg -> indices_array[tache.line_index].size, arg -> correction_array[tache.line_index]);
            }
            pthread_mutex_unlock(arg -> mutex);
        }
    }
    
    return NULL;
}

void traitement_ligne(char*** correction, char* line, Vector* indices,  int linesize, Dictionary_t* dicts, size_t* dict_count, uint32_t* best_dict_out, bool do_correction) {

    uint32_t best_dict = 0;
    linedetect(indices,line, linesize, dicts, *dict_count, &best_dict);
    *best_dict_out = best_dict;

    if (do_correction) {
        Dictionary_t* meilleurdico = &dicts[best_dict];
        linecorrections(correction, line, indices, meilleurdico);
    }
}