#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include <math.h>

float Distance(const char *s1, const char *s2);

// Tolérance du float
#define TOL 0.05f

// Même mots
void test_identical(void) {
    CU_ASSERT_DOUBLE_EQUAL(Distance("Hello", "Hello"), 0.0, TOL);
}

// Test chaînes proches
void test_close(void) {
    CU_ASSERT_DOUBLE_EQUAL(Distance("Hello", "Halo"), 0.217, TOL);
    CU_ASSERT_DOUBLE_EQUAL(Distance("abcdef", "abcfed"), 0.222, TOL);
    CU_ASSERT_DOUBLE_EQUAL(Distance("martha", "marhta"), 0.067, TOL);
}

// Test chaînes totalement différentes
void test_different(void) {
    CU_ASSERT_DOUBLE_EQUAL(Distance("Hello", "World"), 1.0, TOL);
    CU_ASSERT_DOUBLE_EQUAL(Distance("abcdef", "ghijkl"), 1.0, TOL);
}

// Test chaînes vides
void test_empty(void) {
    CU_ASSERT_DOUBLE_EQUAL(Distance("", ""), 1.0, TOL);
    CU_ASSERT_DOUBLE_EQUAL(Distance("", "Hello"), 1.0, TOL);
}

// Test préfixe commun
void test_prefix(void) {
    CU_ASSERT_DOUBLE_EQUAL(Distance("abc", "abcde"), 0.133, TOL);
    CU_ASSERT_DOUBLE_EQUAL(Distance("flaw", "lawn"), 0.375, TOL);
}

int main() {
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    CU_pSuite suite = CU_add_suite("Distance_JaroWinkler_TestSuite", 0, 0);
    if (!suite) {
        CU_cleanup_registry();
        return CU_get_error();
    }

    CU_add_test(suite, "Identical strings", test_identical);
    CU_add_test(suite, "Close strings", test_close);
    CU_add_test(suite, "Different strings", test_different);
    CU_add_test(suite, "Empty strings", test_empty);
    CU_add_test(suite, "Prefix tests", test_prefix);

    // Lancer les tests
    CU_basic_set_mode(CU_BRM_VERBOSE);
    CU_basic_run_tests();
    CU_cleanup_registry();

    return CU_get_error();
}