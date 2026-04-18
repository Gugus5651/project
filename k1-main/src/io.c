#include <fcntl.h>    
#include <unistd.h>   
#include <sys/mman.h> 
#include <sys/stat.h> 

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <float.h>
#include <ctype.h>
#include <limits.h>

#include "common.h"
#include "detection.h"
#include "vector.h"
#include "dict.h"
#include "correction.h"
#include "io.h"

OutputStreams_t *open_outputs(const char *pathname) {

    if (pathname == NULL) return NULL;

    OutputStreams_t* streams = malloc(sizeof(OutputStreams_t));
    if (streams == NULL) return NULL;

    // On construit le chemin vers le fichier de détection : pathname.err
    char detection_path[256];
    snprintf(detection_path, sizeof(detection_path), "%s.err", pathname);

    // On construit le chemin vers le fichier de correction : pathname.fix
    char correction_path[256];
    snprintf(correction_path, sizeof(correction_path), "%s.fix", pathname);

    streams -> detection = open(detection_path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (streams -> detection == -1) {
        free(streams);
        return NULL;
    }

    streams -> correction = open(correction_path, O_WRONLY | O_TRUNC | O_CREAT, 0644);
    if (streams -> correction == -1) {
        close(streams -> detection);
        free(streams);
        return NULL;
    }

    return streams;
}

void close_outputs(OutputStreams_t* streams) {

    if (streams == NULL) return;
    close(streams -> detection);
    close(streams -> correction);

    free(streams);
}

int pretty_print_correction(char *line, uint32_t line_number, size_t word_count, uint32_t *word_indices, char **corrections) {

    if (line == NULL || (word_indices == NULL && word_count > 0) || (corrections == NULL && word_count > 0)) {
        return -1;
    }

    printf("[%u] ",line_number);

    char* pos = line;
    uint32_t current_word = 0;

    while (*pos) {

        // On saute les espaces 
        if (isspace((unsigned char) *pos)) {  // On cast *pos pour avoir une valeur entre 0 et 255 pour isspace().
            putchar(*pos++);
            continue;
        }

        // On arrive au début d'un mot
        char* word_start = pos;
        while (*pos && !isspace((unsigned char) *pos)) {
            pos++;
        }

        char* replacement = NULL;
        for (size_t i = 0 ; i < word_count ; i++) {
            if (word_indices[i] == current_word && corrections[i] != NULL) {
                replacement = corrections[i];
                break;
            }
        }

        if (replacement) printf(GREEN "%s" RESET, replacement);
        else {
            for (char* c = word_start ; c < pos ; c++) {
                putchar(*c);
            }
        }

        current_word++;
    }

    printf("\n");

    return 0;
}

int write_correction(OutputStreams_t* output_streams, uint32_t word_count, char** corrections) {

    if (output_streams == NULL || (corrections == NULL && word_count > 0)) return -1;

    for (uint32_t i = 0 ; i < word_count ; i++) {
        if (corrections[i] == NULL) continue;      // Cas où find_best_correction a retourné NULL

        uint32_t len = strlen(corrections[i]) + 1; // On fait +1 pour le /0
        uint32_t len_be = htobe32(len);            // On convertit en BigEndian

        // On écrit dans le fichier selon le format demandé : 
        // uint32_t longueur (4 octets) "caractères du mot" + \0 (tout en bytes)
        write(output_streams -> correction, &len_be, sizeof(uint32_t));
        write(output_streams -> correction, corrections[i], len);
    }

    return 0;
}

int write_detection(OutputStreams_t *output_stream, uint32_t line_number, uint32_t dict_index, uint32_t word_count, uint32_t *word_indices) {

    if (output_stream == NULL || (word_indices == NULL && word_count > 0)) return -1;

    uint32_t line_number_be = htobe32(line_number);
    uint32_t dict_index_be = htobe32(dict_index);
    uint32_t word_count_be = htobe32(word_count);

    write(output_stream -> detection, &line_number_be, sizeof(uint32_t));
    write(output_stream -> detection, &dict_index_be, sizeof(uint32_t));
    write(output_stream -> detection, &word_count_be, sizeof(uint32_t));

    for (uint32_t i = 0 ; i < word_count ; i++) {
        uint32_t offset_be = htobe32(word_indices[i]);
        write(output_stream -> detection, &offset_be, sizeof(uint32_t));
    }

    return 0;
}

int pretty_print_detection(char *line, uint32_t line_number, size_t word_count, uint32_t *word_indices) {

    if (line == NULL || word_indices == NULL) {
        return -1;
    }

    printf("%u", line_number);
    for (size_t i = 0 ; i < word_count ; i++) {
        printf(",%u", word_indices[i]);
    }
    printf("\n");

    return 0;
}


/*
Pour éviter des problèmes de définition mutliple à travers les différents fichiers qui 
utilisent les fonctions de io.h, on a laissé les déclarations dans io.h et bougé les 
implémentations de ces fonctions vers io.c.
Ainsi on évite les conflits entre les définitions des fonctions dans chaque fichier 
où il y a "#include "io.h"".
*/

int read_input_file(char *input_path, char ***lines, uint32_t **line_sizes,
                    size_t *line_count) {
    *line_count = 0;

    *lines = malloc(sizeof(char *) * INPUT_LINE_COUNT);
    if (!*lines)
        return -1;

    *line_sizes = malloc(sizeof(uint32_t) * INPUT_LINE_COUNT);
    if (!*line_sizes) {
        free(*lines);
        return -1;
    }
    for (size_t i = 0; i < INPUT_LINE_COUNT; i++) {
        (*lines)[i] = strdup(input_data[i]);

        if (!(*lines)[i]) {
        for (size_t j = 0; j < i; j++) {
            free((*lines)[j]);
        }
        free(*lines);
        free(*line_sizes);
        return -1;
        }

        (*line_sizes)[i] = INPUT_WORD_COUNTS[i];
    }
    *line_count = INPUT_LINE_COUNT;
    return 0;
}

int load_dictionaries(const char *path, Dictionary_t **dicts, size_t *dict_count) {

    *dicts = malloc(sizeof(Dictionary_t));
    if (!*dicts) return -1;

    Dictionary_t *d = *dicts;
    d->id = 1;
    d->word_count = DICTIONARY_SIZE;

    d->lang = strdup("wallon");
    if (!d->lang) {
        free(d);
        return -1;
    }

    d->words = malloc(sizeof(char *) * DICTIONARY_SIZE);
    if (!d->words) {
        free(d->lang);
        free(d);
        return -1;
    }

    for (size_t i = 0; i < DICTIONARY_SIZE; i++) {
        d->words[i] = strdup(dictionary[i]);

        if (!d->words[i]) {
            for (size_t j = 0; j < i; j++) {
                free(d->words[j]);
            }
            free(d->words);
            free(d->lang);
            free(d);
            return -1;
        }
    } 

    *dict_count = 1;
    return 0;
}