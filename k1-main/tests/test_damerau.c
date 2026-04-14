#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../headers/Distance-Damerau.h"

void test_damerau(void) {
    CU_ASSERT_EQUAL(Distance_damerau("", ""), 0);
    CU_ASSERT_EQUAL(Distance_damerau("pizza", "pizza"), 0);
    CU_ASSERT_EQUAL(Distance_damerau("clemence", "celmence"), 1);
    CU_ASSERT_EQUAL(Distance_damerau("climence", "clemence"), 1);
    CU_ASSERT_EQUAL(Distance_damerau("yo", "oy"), 1);
    CU_ASSERT_EQUAL(Distance_damerau("chien", "cihen"), 1);
    CU_ASSERT_EQUAL(Distance_damerau("ordinateur", "ordnateur"), 1);
}

