#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>

void test_damerau(void);
void test_hamming(void);
void test_find_best_dict(void);
void test_find_best_correction(void);
void test_pretty_print_null(void);
void test_reuni_BinarySearchDetection(void);
void test_reuni_JW(void);
void test_levenshtein(void);
void test_reuni_correction(void);
void test_reuni_Detection(void);

/*
Fonction déjà là au début,
void test_function(void) {
    CU_ASSERT(1 == 1);
}
*/

int main(void) {
    CU_initialize_registry();
    CU_pSuite suite = CU_add_suite("Distance", 0, 0);
    //CU_add_test(suite, "test_function", test_function);
    CU_add_test(suite, "test_damerau", test_damerau);
    CU_add_test(suite, "test_hamming", test_hamming);
    CU_add_test(suite, "test_JW", test_reuni_JW);
    CU_add_test(suite, "test_levenshtein", test_levenshtein);

    CU_pSuite suite2 = CU_add_suite("Detection", 0, 0);
    CU_add_test(suite2, "test_Detection", test_reuni_Detection);

    CU_pSuite suite3 = CU_add_suite("Correction", 0, 0);
    CU_add_test(suite3, "test_Correction", test_reuni_correction);


    CU_basic_run_tests();
    CU_cleanup_registry();
    return 0;
}