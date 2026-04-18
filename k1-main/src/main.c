#include <stdio.h>
#include "common.h"
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "detection.h"
#include "correction.h"
#include "vector.h"
#include "Distance-JW.h"
#include "input.h"
#include "io.h"

void free_args(CommandLineArgs_t* args) {
    if (args) {
        if (args->output_path != NULL)
            free(args->output_path);
        free(args->input_path);
        free(args->mode);
        free(args);
    }
}

void print_help(const char* program_name) {
    printf("Usage: %s --dicts <dictionary_path> --input <input_path> [--output <output_path>] [--mode <mode>] [--verbose]\n", program_name);
    printf("Modes: detection\n");
    printf("Options:\n");
    printf("  -d|--dicts <path>          Path to the dictionaries directory.\n");
    printf("  -i|--input <path>          Path to input .bin file to read words from.\n");
    printf("  -o|--output <path>         Path to output file to write results to (default is stdout).\n");
    printf("  -m|--mode <mode>           Mode of operation (e.g., detection).\n");
    printf("  -v|--verbose               Enable verbose output.\n");
    printf("  -t|--threads <num>         Number of threads to use (default is 1).\n");
    printf("  -h|--help                  Show this help message.\n");
    printf("Examples:\n");
    printf("  %s --dicts ./dictionaries --input input.bin \n", program_name);
    printf("  %s --dicts ./dictionaries --mode detection --input input.bin --dicts dicts/ --output output\n", program_name);
    printf("  %s --help\n", program_name);
    printf("Note: If no mode is specified, the default is 'championship'.\n");
    printf("Note: If no output file is specified, the program will write to stdout.\n");
}

CommandLineArgs_t* parse_args(int argc, char const *argv[]) {
    CommandLineArgs_t* args = (CommandLineArgs_t*) malloc(sizeof(CommandLineArgs_t)); 
    args->output_path = NULL;
    args->verbose = false;
    args->input_path = NULL;
    args->mode = NULL;
    args->dictionnaries_path[0] = '\0';
    args->threads = 1;
    
    int opt;
    struct option long_options[] = {
        {"dicts", required_argument, NULL, 'd'},
        {"input", required_argument, NULL, 'i'},
        {"output", required_argument, NULL, 'o'},
        {"mode", required_argument, NULL, 'm'},
        {"verbose", no_argument, NULL, 'v'},
        {"help", no_argument, NULL, 'h'},
        {"threads", required_argument, NULL, 't'},
        {NULL, 0, NULL, 0}
    };
    while ((opt = getopt_long(argc, (char* const*)argv, "d:i:o:m:vht:", long_options, NULL)) != -1) {
        switch (opt) {
            case 'd':
                strncpy(args->dictionnaries_path, optarg, MAX_PATH_LENGTH - 1);
                args->dictionnaries_path[MAX_PATH_LENGTH - 1] = '\0';
                break;
            case 'i':
                args->input_path = strdup(optarg);
                break;
            case 'o':
                args->output_path = strdup(optarg);
                break;
            case 'm':
                args->mode = strdup(optarg);
                if (args->mode == NULL) {
                    perror("Failed to allocate memory for mode");
                    exit(EXIT_FAILURE);
                }
                break;
            case 't':
                args->threads = atoi(optarg);
                break;
            case 'v':
                args->verbose = true;
                break;
            case 'h':
                print_help(argv[0]);
                free_args(args);
                exit(EXIT_SUCCESS);
            default:
                fprintf(stderr, "Unknown argument: %s\n", argv[optind - 1]);
                exit(EXIT_FAILURE);
        }
    }

    if (args->mode == NULL) {
        args->mode = strdup("detection");
        if (args->mode == NULL) {
            perror("Failed to allocate memory for mode");
            exit(EXIT_FAILURE);
        }
    }

    return args;
}

int main(int argc, char const *argv[]) {
    CommandLineArgs_t* args = parse_args(argc, argv);

    printf("Dictionary Path: %s\n", strlen(args->dictionnaries_path) > 0 ? args->dictionnaries_path : "Not Provided");
    printf("Input File: %s\n", args->input_path ? args->input_path : "Not Provided");
    printf("Output File: %s\n", args->output_path ? args->output_path : "Not Provided");
    printf("Mode: %s\n", args->mode ? args->mode : "Not Provided");
    printf("Verbose: %s\n", args->verbose ? "Enabled" : "Disabled");
    printf("Threads: %u\n", args->threads);

    // On recupère le fichier de sortie avec l'argument output_path.
    // Si c'est NULL, on print dans le terminal.
    OutputStreams_t* streams = NULL;
    if (args -> output_path != NULL) {
        printf("Opening output: %s\n", args->output_path);
        streams = open_outputs(args -> output_path);
    }

    free_args(args);

    Vector* indices;
    size_t line_count;
    char** lines;
    uint32_t* lines_size;
    Dictionary_t* dicts;
    size_t dict_count;
    uint32_t* best_dicts;

    if (detect(&indices, NULL, &dicts, &dict_count, &lines, &lines_size, &line_count, &best_dicts) != 1) {
        printf("Erreur lors de la détection des mots\n");
        if (streams) close_outputs(streams);
        return 0;
    }

    char*** correction;
    if (corrections(&correction, lines, indices, dicts, line_count, dict_count) != 1) {
        printf("Erreur lors de la correction des mots\n");
        for (size_t i = 0 ; i < line_count ; i++) vector_free(&indices[i]);
        free(indices);
        free(best_dicts);
        if (streams) close_outputs(streams);
        return 0;
    } 

    for (size_t i = 0 ; i < line_count ; i++) {
        uint32_t bad_words_count = indices[i].size;

        // If/else en fonction de s'il y a un fichier output ou pas. Si non, print dans le 
        // terminal avec pretty_print_correction().
        if (streams != NULL) {
            write_detection(streams, (uint32_t) i, (uint32_t) best_dicts[i], bad_words_count, indices[i].data);
            write_correction(streams, bad_words_count, correction[i]);
        }
        else {
            pretty_print_correction(lines[i], (uint32_t) i, (size_t) bad_words_count, indices[i].data, correction[i]);
        }
    }

    if (streams) close_outputs(streams);
    free(best_dicts);

    for (size_t i = 0 ; i < line_count ; i++) {
        for (size_t j = 0 ; j < indices[i].size ; j++) free(correction[i][j]);
        free(correction[i]);
        vector_free(&indices[i]);
    }
    free(correction);
    free(indices);

    for (size_t i = 0 ; i < line_count ; i++) free(lines[i]);
    free(lines);
    free(lines_size);
    free(dicts);
    
    return 0;
}