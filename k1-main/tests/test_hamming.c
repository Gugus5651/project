#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../headers/Distance-Hamming.h"

void test_hamming(void){
    CU_ASSERT_EQUAL(Distance_hamming(NULL, ""), -1);
    CU_ASSERT_EQUAL(Distance_hamming(NULL, NULL), -1);
    CU_ASSERT_EQUAL(Distance_hamming("", NULL), -1);
    CU_ASSERT_EQUAL(Distance_hamming("chaton", NULL), -1);
    CU_ASSERT_EQUAL(Distance_hamming("", ""), 0);
    CU_ASSERT_EQUAL(Distance_hamming("neige", "neige"), 0);
    CU_ASSERT_EQUAL(Distance_hamming("clemence", "celmence"), 2);
    CU_ASSERT_EQUAL(Distance_hamming("climence", "clemence"), 1);
    CU_ASSERT_EQUAL(Distance_hamming("yo", "oy"), 2);
    CU_ASSERT_EQUAL(Distance_hamming("chien", "cihen"), 2);
    CU_ASSERT_EQUAL(Distance_hamming("ordinateur", "ordnateur"), 7);
    CU_ASSERT_EQUAL(Distance_hamming("NULL" , ""),4);
    CU_ASSERT_EQUAL(Distance_hamming("\0" , ""),0);
    CU_ASSERT_EQUAL(Distance_hamming("\0" , "\0"),0);
    CU_ASSERT_EQUAL(Distance_hamming("\0hahafeinte" , ""),0);
    CU_ASSERT_EQUAL(Distance_hamming("Hamming" , "hamming"),1);
    CU_ASSERT_EQUAL(Distance_hamming("hamming","hamming "),1);
    CU_ASSERT_EQUAL(Distance_hamming("hamming","hammin g "),3);
}


