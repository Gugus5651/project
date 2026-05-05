#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

// Convertit input.h en fichier binaire (format des consignes)
#include "../headers/input.h"
#include "../headers/portable_endian.h"

int main() {

    int fd = open("input.bin", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        perror("Erreur ouverture fichier");
        return 1;
    }

    for (size_t i = 0; i < INPUT_LINE_COUNT; i++) {
        // Taille de la ligne en octets
        uint32_t len = (uint32_t) strlen(input_data[i]);

        // On écrit la taille en big-endian sur 4 octets
        uint32_t len_be = htobe32(len);
        write(fd, &len_be, sizeof(uint32_t));

        // On écrit le texte de la ligne
        write(fd, input_data[i], len);
    }

    close(fd);
    printf("input.bin généré avec %d lignes.\n", INPUT_LINE_COUNT);
    return 0;
}