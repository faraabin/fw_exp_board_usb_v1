/**
 ******************************************************************************
 * @file           : faraabin_default_fobjects_test.c
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
#include "faraabin_default_fobjects_test.h"

#include "faraabin.h"
#include "unity_fixture.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/**
 * @brief 
 * 
 */
typedef enum {

  eA,
  eB,
  eC,
  eD

}eTestEvent;

/* Private variables ---------------------------------------------------------*/
TEST_GROUP(DefaultFobjectsTest);

static char Text[] = "TEST STRING";

FARAABIN_DICT_GROUP_DEF_STATIC_(DefaultFobjectsTestDg);
FARAABIN_VAR_TYPE_DEF_STATIC_(eTestEvent);
static eTestEvent TestEvent;

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
 * @brief Function that runs all unit tests and returns the result.
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int FaraabinDefaultFobjectsTest_Run(int argc, const char* argv[]) {

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
TEST_GROUP_RUNNER(DefaultFobjectsTest) {
  
  RUN_TEST_CASE(DefaultFobjectsTest, TemplateTest);
  
}

/**
 * @brief Run all tests in test group.
 * 
 */
static void RunTests(void) {
  RUN_TEST_GROUP(DefaultFobjectsTest);
}

/**
 * @brief Unit test setup.
 * 
 */
TEST_SETUP(DefaultFobjectsTest) {

}

/**
 * @brief Setup that executes before all tests.
 * 
 */
static void OneTimeSetup(void) {

  FARAABIN_DictGroup_Init_(&DefaultFobjectsTestDg);
  FARAABIN_VAR_TYPE_ENUM_INIT_(eTestEvent);

  FARAABIN_Printf_("Printf(%s)", Text);
  FARAABIN_PrintfWarning_("PrintfWarning(%s)", Text);
  FARAABIN_PrintfError_("PrintfError(%s)", Text);

  FARAABIN_Puts_(Text);
  FARAABIN_PutsWarning_(Text);
  FARAABIN_PutsError_(Text);

  FARAABIN_SendEvent_(eTestEvent, eA);
  FARAABIN_SendEventWarning_(eTestEvent, eA);
  FARAABIN_SendEventError_(eTestEvent, eA);
  
}

/**
 * @brief Teardown that executes after all tests (or in case of a test failure).
 * 
 */
static void OneTimeTeardown(void) {

}

/**
 * @brief Unit test teardown.
 * 
 */
TEST_TEAR_DOWN(DefaultFobjectsTest) {

}

/**
 * @brief Template test.
 * 
 */
TEST(DefaultFobjectsTest, TemplateTest) {
  
  TEST_ASSERT(true);
}

/**
 * @brief 
 * 
 */
FARAABIN_DICT_GROUP_FUNC_(DefaultFobjectsTestDg) {

  FARAABIN_VAR_STRUCT_ARRAY_DICT_(TestEvent, eTestEvent);

  FARAABIN_DICT_GROUP_FUNC_END_;
}

/**
 * @brief 
 * 
 */
static FARAABIN_VAR_TYPE_ENUM_FUNC_(eTestEvent)
{
  FARAABIN_VAR_TYPE_ENUM_FUNC_START_(eTestEvent);
  
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eA);
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eB);
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eC);
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eD);
  
  FARAABIN_VAR_TYPE_ENUM_FUNC_END_(eTestEvent);
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
