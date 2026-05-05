#ifndef INPUT_bis_H
#define INPUT_bis_H

#include <stdint.h>

static const char *const input_data[] = {
    "i love pizza",
    "my sster is pretty",
    "je suis une pizza",
    "cien chat ou pizza",
    "chien, chat ou pizza",
    "i eat a lott",
    "i loove eat",
    "chocolat",
    "chocola",

};

static const uint32_t INPUT_WORD_COUNTS[] = {
    3,4,4,4,4,4,3,1,1
};

#define INPUT_LINE_COUNT 9

#endif