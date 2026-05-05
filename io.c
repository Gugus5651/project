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
#include <dirent.h>

#include "common.h"
#include "detection.h"
#include "vector.h"
#include "dict.h"
#include "correction.h"
#include "io.h"
#include "portable_endian.h"

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

    if (line == NULL || (word_indices == NULL && word_count > 0)) {
        return -1;
    }

    printf("Ligne %u : ", line_number);
    if (word_count == 0) {
        printf("aucune faute\n");
    }
    else {
        printf("faute(s) au(x) offset(s) ");
        for (size_t i = 0 ; i < word_count ; i++) {
            if (i > 0) printf(", ");
            printf("%u", word_indices[i]);
        }
        printf("\n");
    }

    return 0;
}


/*
Pour éviter des problèmes de définition mutliple à travers les différents fichiers qui 
utilisent les fonctions de io.h, on a laissé les déclarations dans io.h et bougé les 
implémentations de ces fonctions vers io.c.
Ainsi on évite les conflits entre les définitions des fonctions dans chaque fichier 
où il y a "#include "io.h"".
*/

int read_input_file(char *input_path, char ***lines, uint32_t **line_sizes, size_t *line_count) {
    
    int fd = open(input_path, O_RDONLY);
    if (fd == -1) return -1;

    // On récupère les infos du fichier.
    struct stat st;
    if (fstat(fd, &st) == -1) {
        close(fd);
        return -1;
    }

    // On mappe le fichier en mémoire.
    char *map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    close(fd);
    if (map == MAP_FAILED) return -1;

    // On compte le nombre de lignes.
    size_t nb_lines = 0;
    size_t pos = 0;
    while (pos + 4 <= (size_t) st.st_size) {
        // On convertit la taille de la ligne à venir en uint_32.
        uint32_t line_size = be32toh(*(uint32_t*) (map+pos));
        // On avance pos de 4 octets + la taille de la ligne.
        pos += 4 + line_size;
        nb_lines++;
    }

    // On alloue les tableaux lines et line_sizes.
    *lines = malloc(nb_lines * sizeof(char*));
    *line_sizes = malloc(nb_lines * sizeof(uint32_t));
    if (!*lines || !*line_sizes) {
        free(*lines);
        free(*line_sizes);
        munmap(map, st.st_size);
        return -1;
    }

    // On remplit les tableaux.
    pos = 0;
    for (size_t i = 0 ; i < nb_lines ; i++) {

        uint32_t line_size = be32toh(*(uint32_t*) (map+pos));
        pos += 4;

        (*lines)[i] = malloc(line_size + 1);
        if (!(*lines)[i]) {
            for (size_t j = 0 ; j < i ; j++) {
                free((*lines)[j]);
            }
            free(*lines);
            free(*line_sizes);
            munmap(map, st.st_size);
            return -1;
        }

        // On copie la ligne dans lines[i].
        memcpy((*lines)[i], map + pos, line_size);
        (*lines)[i][line_size] = '\0';
        (*line_sizes)[i] = line_size;

        // On avance pos par la taille de la ligne. 
        pos += line_size;
    }

    // On initialise line_count. 
    *line_count = nb_lines;
    munmap(map, st.st_size);
    return 0;

}

int load_dictionaries(const char *path, Dictionary_t **dicts, size_t *dict_count) {
    
    if (path == NULL || dicts == NULL || dict_count == NULL) return -1; 

    /* 
    On ouvre le dossier avec opendir(path) qui retourne un pointeur DIR* (exactement 
    comme on ferait avec open(path) pour un fichier).
    */
    DIR* dir = opendir(path);
    if (dir == NULL) return -1;

    /* 
    On compte le nombre de dictionnaires dans le dossier avec readdir(path) qui lit l'entrée
    suivante du dossier et retourne un pointeur vers un objet de la structure dirent qui a comme 
    attributs le nom du fichier actuel et un indentifiant interne du fichier pour le système.
    */
    size_t count = 0;
    struct dirent* current_file;
    while ((current_file = readdir(dir)) != NULL) {
        // On vérifie le format du nom du fichier pour voir si c'est bien un dictionnaire.
        char* end = strrchr(current_file -> d_name, '.');
        if (end && strcmp(end, ".dict") == 0) count++;
    }
    // On revient au début du dossier.
    rewinddir(dir);

    if (count == 0) {
        closedir(dir);
        return -1;
    }

    // On alloue le tableau de dictionnaires.
    *dicts = malloc(count * sizeof(Dictionary_t));
    if (!*dicts) {
        closedir(dir);
        return -1;
    }
    
    // On peut maintenant charger le dictionnaire de chaque fichier.
    size_t dict_index = 0;
    while ((current_file = readdir(dir)) != NULL && dict_index < count) {

        char* end = strrchr(current_file -> d_name, '.');
        if (!end || strcmp(end, ".dict") != 0) continue;

        Dictionary_t* current_dict = &(*dicts)[dict_index];

        // Ici on construit le chemin vers le dictionnaire courant avec snprintf().
        char dictpath[MAX_PATH_LENGTH];
        snprintf(dictpath, sizeof(dictpath), "%s/%s", path, current_file -> d_name);

        int fd = open(dictpath, O_RDONLY);
        if (fd == -1) continue;

        // On récupère les infos du fichier (la taille nous intéresse surtout).
        struct stat st;
        fstat(fd, &st);

        /*
        On mappe le dictionnaire en mémoire. A cet endroit, map est un pointeur vers un tableau
        d'octets qui est en fait le contenu du dictionnaire.
        */ 
        char* map = mmap(NULL, st.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
        close(fd);
        if (map == MAP_FAILED) continue;

        // On lit l'ID du dictionnaire (sur la première ligne d'après le format dans les consignes).
        size_t pos = 0;
        while (pos < (size_t) st.st_size && map[pos] != '\n') pos++;
        char id[16] = {0};  // Un petit buffer pour stocker l'ID en texte.
        memcpy(id, map, pos < 15 ? pos : 15);  // Copie les premiers octets de map dans id.

        current_dict -> id = (uint32_t) atoi(id); 
        pos++;

        // D'après le format, langue du dictionnaire = nom du fichier sans le ".dict"
        char lang[64];
        strncpy(lang, current_file -> d_name, sizeof(lang) - 1);
        lang[sizeof(lang) - 1] = '\0';
        *strrchr(lang, '.') = '\0';
        current_dict -> lang = strdup(lang);

        // On compte les mots du dictionnaire.
        size_t nb_words = 0;
        for (size_t i = pos ; i < (size_t) st.st_size ; i++) {
            if (map[i] == '\n') nb_words++;
        }
        if (map[st.st_size - 1] != '\n') nb_words++;

        // On alloue et on remplit current_dict -> words
        current_dict -> words = malloc(nb_words * sizeof(char*));
        if (!current_dict -> words) {
            munmap(map,st.st_size);
            continue;
        }

        /*
        On initialise un index dans current_dict -> words, et on reprend pos qui pointe juste
        après le "\n" de la première de ligne (après l'ID selon le format).
        */ 
        size_t word_index = 0, debut = pos;
        for (size_t i = pos ; i <= (size_t) st.st_size ; i++) {
            if (i == (size_t) st.st_size || map[i] == '\n') {

                // On calcule facilement la longueur du mot grâce à "début".
                size_t word_len = i - debut;

                if (word_len > 0) {
                    current_dict -> words[word_index] = malloc(word_len+1);
                    // On copie le mot dans current_dict -> words.
                    memcpy(current_dict -> words[word_index], map + debut, word_len);
                    // Ajout du caractère terminal pour avoir un string valide.
                    current_dict -> words[word_index][word_len] = '\0';
                    word_index++;
                }

                debut = i + 1;
            }
        }
        current_dict -> word_count = word_index;

        // On libère "map" (comme un free mais pour les map).
        munmap(map, st.st_size);
        dict_index++;
    }

    closedir(dir);
    *dict_count = dict_index;
    return 0;
}


void free_dictionaries(Dictionary_t* dicts, size_t dict_count) {

    if (dicts == NULL) {
        return;
    }

    for (size_t i = 0 ; i < dict_count ; i++) {
        for (uint32_t j = 0 ; j < dicts[i].word_count ; j++) {
            free(dicts[i].words[j]);
        }
        free(dicts[i].words);
        free(dicts[i].lang);
    }
    free(dicts);
}