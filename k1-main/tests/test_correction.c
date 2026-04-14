#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../headers/correction.h"
#include "../headers/detection.h"
#include "../headers/common.h"

// Mots triés alphabétiquement (requis pour la recherche binaire)
static char* words_fr[] = { "bonjour", "chat", "chien", "pizza", "quattre", "moi", "aimer", "fromage"};
static char* words_en[] = { "cat", "dog", "hello" , "pizza", "four", "code", "love", "you", "I"};


void test_find_best_dict(void) {
    Dictionary_t dicts[2] = {
        { words_fr, 3, "fr", 0 },
        { words_en, 3, "en", 1 }
    };
    CU_ASSERT_EQUAL(find_best_dict_for_line("bonjour chat", dicts, 2), 0);
    CU_ASSERT_EQUAL(find_best_dict_for_line("cat dog", dicts, 2), 1);
    CU_ASSERT_EQUAL(find_best_dict_for_line("I love pizza", dicts, 3), 1);
    CU_ASSERT_EQUAL(find_best_dict_for_line("moi love you", dicts, 3), 1);
    CU_ASSERT_EQUAL(find_best_dict_for_line("love quattre", dicts, 2), 0);
    CU_ASSERT_EQUAL(find_best_dict_for_line("", dicts, 0), 0);
    CU_ASSERT_EQUAL(find_best_dict_for_line("lov", dicts, 1), 0);

}

void test_find_best_correction(void) {
    Dictionary_t dict = { words_fr, 3, "fr", 0 };
    CU_ASSERT_STRING_EQUAL(find_best_correction("chein", &dict), "chien");
    CU_ASSERT_PTR_NULL(find_best_correction(NULL, NULL));
}

void test_pretty_print_null(void) {
    CU_ASSERT_EQUAL(pretty_print_correction(NULL, 0, 0, NULL, NULL), -1);
}