#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <stdbool.h>

#include "../headers/common.h"
#include "../headers/detection.h"
#include <string.h>

#include "common.h"
#include "vector.h"
#include "detection.h"


// dico
static char* words_fr[] = { "aimer", "bonjour", "chat", "chien", "fromage", "moi", "pizza", "quattre" };
static char* words_en[] = { "cat", "code", "dog", "four", "hello", "I", "love", "pizza", "you" };

void test_compare(void) {
    CU_ASSERT_EQUAL(compare("chat", "chat"), 0);
    CU_ASSERT_TRUE(compare("apple", "banana") < 0);
    CU_ASSERT_TRUE(compare("zebra", "apple") > 0);
    CU_ASSERT_TRUE(compare("Chat", "chat") != 0); 
}

void test_binary2(void) {
    int size_fr = 8;
    
    // vrai
    CU_ASSERT_TRUE(binary2((const char**)words_fr, 0, size_fr - 1, "chat"));
    CU_ASSERT_TRUE(binary2((const char**)words_fr, 0, size_fr - 1, "aimer"));
    CU_ASSERT_TRUE(binary2((const char**)words_fr, 0, size_fr - 1, "quattre"));
    
    // faux
    CU_ASSERT_FALSE(binary2((const char**)words_fr, 0, size_fr - 1, "avion"));
    CU_ASSERT_FALSE(binary2((const char**)words_fr, 0, size_fr - 1, "zebre"));
}

void test_binary1(void) {
    Dictionary_t dict_en = { words_en, 9, "en", 1 };
    Dictionary_t dict_fr = { words_fr, 8, "fr", 0 };

    CU_ASSERT_TRUE(binary1(&dict_en, "hello"));
    CU_ASSERT_TRUE(binary1(&dict_en, "love"));
    CU_ASSERT_FALSE(binary1(&dict_en, "bonjour"));
}

void test_pretty_print_detection_null(void) {
    // tester que ça retourne -1 si on lui passe des arguments NULL
    CU_ASSERT_EQUAL(pretty_print_detection(NULL, 0, 0, NULL), -1);
}

void test_detect_error_handling(void) {
    Vector* indices = NULL;
    Dictionary_t* dicts = NULL;
    size_t dict_count = 0;
    char** lines = NULL;
    uint32_t* lines_size = NULL;
    size_t line_count = 0;

    // Test avec un fichier inexistant
    int res = detect(&indices, "non_existent.txt", &dicts, &dict_count, &lines, &lines_size, &line_count);
    CU_ASSERT_EQUAL(res, 0); 
}

int main() {
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    CU_pSuite pSuite = CU_add_suite("Suite_Detection_Binaire", NULL, NULL);
    if (NULL == pSuite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_add_test(pSuite, "test_compare", test_compare);
    CU_add_test(pSuite, "test_binary2", test_binary2);
    CU_add_test(pSuite, "test_binary1", test_binary1);
    CU_add_test(pSuite, "test_pretty_print_null", test_pretty_print_detection_null);
    CU_add_test(pSuite, "test_detect_logic", test_detect_error_handling);

    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();
    
    return CU_get_error();
}