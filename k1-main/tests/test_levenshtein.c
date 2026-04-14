#include <CUnit/CUnit.h>
#include <CUnit/Basic.h>
#include "../headers/Distance-Levenshtein.h"


void test_levenshtein(void){
    CU_ASSERT_EQUAL(Distance_levenshtein(NULL, ""), -1);
    CU_ASSERT_EQUAL(Distance_levenshtein(NULL, NULL), -1);
    CU_ASSERT_EQUAL(Distance_levenshtein("", NULL), -1);
    CU_ASSERT_EQUAL(Distance_levenshtein("chaton", NULL), -1);
    CU_ASSERT_EQUAL(Distance_levenshtein("", ""), 0);
    CU_ASSERT_EQUAL(Distance_levenshtein("neige", "neige"), 0);
    CU_ASSERT_EQUAL(Distance_levenshtein("clemence", "celmence"), 2);
    CU_ASSERT_EQUAL(Distance_levenshtein("climence", "clemence"), 1);
    CU_ASSERT_EQUAL(Distance_levenshtein("yo", "oy"), 2);
    CU_ASSERT_EQUAL(Distance_levenshtein("chien", "cihen"), 2);
    CU_ASSERT_EQUAL(Distance_levenshtein("ordinateur", "ordnateur"), 7);
    CU_ASSERT_EQUAL(Distance_levenshtein("NULL" , ""),4);
    CU_ASSERT_EQUAL(Distance_levenshtein("\0" , ""),2);
    CU_ASSERT_EQUAL(Distance_levenshtein("\0" , "\0"),0);
    CU_ASSERT_EQUAL(Distance_levenshtein("\0hahafeinte" , ""),11);
    CU_ASSERT_EQUAL(Distance_levenshtein("Hamming" , "hamming"),1);
    CU_ASSERT_EQUAL(Distance_levenshtein("hamming","hamming "),1);
    CU_ASSERT_EQUAL(Distance_levenshtein("hamming","hammin g "),3);
}