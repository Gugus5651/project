#include <stdlib.h>
#include <stdint.h>
#include "vector.h"

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

