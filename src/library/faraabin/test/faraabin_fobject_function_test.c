/**
 ******************************************************************************
 * @file           : faraabin_fobject_function_test.c
 * @brief          :
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 FaraabinCo.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component
 * 
 * https://faraabinco.ir/
 * https://github.com/FaraabinCo
 *
 ******************************************************************************
 * @verbatim
 * @endverbatim
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_function_test.h"
#include "test_function_fn.h"

#include "unity_fixture.h"
#include "faraabin.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TEST_GROUP(FunctionTest);

FARAABIN_DICT_GROUP_DEF_STATIC_(TestFunctionDg);

static float TestVar;

/* Private function prototypes -----------------------------------------------*/
static void RunTests(void);
static void OneTimeSetup(void);
static void OneTimeTeardown(void);

/* Variables -----------------------------------------------------------------*/

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                          ##### Exported Functions #####                          ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/

/**
 * @brief Function that runs all unit tests and returns the result
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int FaraabinFobjectFunctionTest_Run(int argc, const char* argv[]) {

  OneTimeSetup();
  
  int testsFailed = UnityMain(argc, argv, RunTests);

  OneTimeTeardown();

  return testsFailed;
}

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                            ##### Private Functions #####                         ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/

/**
 * @brief Test group runner.
 * 
 */
TEST_GROUP_RUNNER(FunctionTest) {
  
  RUN_TEST_CASE(FunctionTest, TemplateTest);
  
}

/**
 * @brief Run all tests in test group.
 * 
 */
static void RunTests(void) {
  RUN_TEST_GROUP(FunctionTest);
}

/**
 * @brief Unit test setup.
 * 
 */
TEST_SETUP(FunctionTest) {

}

/**
 * @brief Unit test teardown.
 * 
 */
TEST_TEAR_DOWN(FunctionTest) {

}

/**
 * @brief Template test.
 * 
 */
TEST(FunctionTest, TemplateTest) {
  
  TEST_ASSERT(true);
}

/**
 * @brief Setup that executes before all tests.
 * 
 */
static void OneTimeSetup(void) {

  FARAABIN_DictGroup_Init_(&TestFunctionDg);
  FARAABIN_FunctionGroupType_Init_(&TestFunctionFn);

}

/**
 * @brief Teardown that executes after all tests (or in case of a test failure).
 * 
 */
static void OneTimeTeardown(void) {

}

/**
 * @brief 
 * 
 */
FARAABIN_DICT_GROUP_FUNC_(TestFunctionDg) {
  
	FARAABIN_FUNCTION_GROUP_DICT_(TestFunctionFn);
	FARAABIN_FUNCTION_GROUP_OBJECT_DICT_(TestFunctionFn, TestVar);
  
  FARAABIN_DICT_GROUP_FUNC_END_;
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
