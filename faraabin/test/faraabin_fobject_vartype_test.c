/**
 ******************************************************************************
 * @file           : faraabin_fobject_vartype_test.c
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
#include "faraabin_fobject_vartype_test.h"

#include "unity_fixture.h"
#include "faraabin.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/**
 * @brief 
 * 
 */
typedef struct {
  
  uint8_t A;
  uint8_t B;
  uint8_t C;
  uint8_t D;

}sTypeTestStruct;

/**
 * @brief 
 * 
 */
typedef enum {

  eA,
  eB,
  eC,
  eD

}eTypeTestEnum;

/**
 * @brief 
 * 
 */
typedef struct {

  uint8_t A;

}sTypeTestStatic;

/* Private variables ---------------------------------------------------------*/
TEST_GROUP(VartypeTest);

FARAABIN_DICT_GROUP_DEF_STATIC_(VartypeTestDg);

FARAABIN_VAR_TYPE_DEF_(sTypeTestStruct);
FARAABIN_VAR_TYPE_DEF_EXTERN_(sTypeTestStruct);
FARAABIN_VAR_TYPE_DEF_STATIC_(sTypeTestStatic);
FARAABIN_VAR_TYPE_DEF_(eTypeTestEnum);
FARAABIN_VAR_TYPE_DEF_EXTERN_(eTypeTestEnum);

static sTypeTestStruct TestVarStruct;
static sTypeTestStruct TestVarStructArray[2];
static eTypeTestEnum TestVarEnum;
static eTypeTestEnum TestVarEnumArray[2];

/* Private function prototypes -----------------------------------------------*/
static void RunTests(void);
static void OneTimeSetup(void);
static void OneTimeTeardown(void);

static uint8_t VartypeTestAccessCallback(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size);

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
int FaraabinFobjectVartypeTest_Run(int argc, const char* argv[]) {

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
TEST_GROUP_RUNNER(VartypeTest) {
  
  RUN_TEST_CASE(VartypeTest, TemplateTest);
  
}

/**
 * @brief Run all tests in test group.
 * 
 */
static void RunTests(void) {
  RUN_TEST_GROUP(VartypeTest);
}

/**
 * @brief Unit test setup.
 * 
 */
TEST_SETUP(VartypeTest) {

}

/**
 * @brief Unit test teardown.
 * 
 */
TEST_TEAR_DOWN(VartypeTest) {

}

/**
 * @brief Template test.
 * 
 */
TEST(VartypeTest, TemplateTest) {
  
  TEST_ASSERT(true);
}

/**
 * @brief Setup that executes before all tests.
 * 
 */
static void OneTimeSetup(void) {

  FARAABIN_DictGroup_Init_(&VartypeTestDg);

  FARAABIN_VAR_TYPE_STRUCT_INIT_(sTypeTestStruct);
  FARAABIN_VAR_TYPE_STRUCT_INIT_WP_(sTypeTestStruct, "TestVartype\\Wp");
  FARAABIN_VAR_TYPE_ENUM_INIT_(eTypeTestEnum);
  FARAABIN_VAR_TYPE_ENUM_INIT_WP_(eTypeTestEnum, "TestVartype\\Wp");
  FARAABIN_VAR_TYPE_STRUCT_INIT_WP_(sTypeTestStatic, "TestVartype\\Wp");

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
 * @param accessType 
 * @param varPtr 
 * @param data 
 * @param size 
 * @return uint8_t 
 */
static uint8_t VartypeTestAccessCallback(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size) {
  return 0;
}

/**
 * @brief DictGroup function.
 * 
 */
FARAABIN_DICT_GROUP_FUNC_(VartypeTestDg) {

  FARAABIN_VAR_STRUCT_DICT_(TestVarStruct, sTypeTestStruct);
  FARAABIN_VAR_STRUCT_DICT_WP_(TestVarStruct, sTypeTestStruct, "TestVartype\\Wp");
  FARAABIN_VAR_STRUCT_DICT_CB_(TestVarStruct, sTypeTestStruct, VartypeTestAccessCallback);
  FARAABIN_VAR_STRUCT_DICT_CB_WP_(TestVarStruct, sTypeTestStruct, VartypeTestAccessCallback, "TestVartype\\WpCb");
  
  FARAABIN_VAR_STRUCT_ARRAY_DICT_(TestVarStructArray, sTypeTestStruct);
  FARAABIN_VAR_STRUCT_ARRAY_DICT_WP_(TestVarStructArray, sTypeTestStruct, "TestVartype\\Wp");
  FARAABIN_VAR_STRUCT_ARRAY_DICT_CB_(TestVarStructArray, sTypeTestStruct, VartypeTestAccessCallback);
  FARAABIN_VAR_STRUCT_ARRAY_DICT_CB_WP_(TestVarStructArray, sTypeTestStruct, VartypeTestAccessCallback, "TestVartype\\WpCb");

  FARAABIN_VAR_ENUM_DICT_(TestVarEnum, eTypeTestEnum);
  FARAABIN_VAR_ENUM_DICT_WP_(TestVarEnum, eTypeTestEnum, "TestVartype\\Wp");
  FARAABIN_VAR_ENUM_DICT_CB_(TestVarEnum, eTypeTestEnum, VartypeTestAccessCallback);
  FARAABIN_VAR_ENUM_DICT_CB_WP_(TestVarEnum, eTypeTestEnum, VartypeTestAccessCallback, "TestVartype\\WpCb");

  FARAABIN_VAR_ENUM_ARRAY_DICT_(TestVarEnumArray, eTypeTestEnum);
  FARAABIN_VAR_ENUM_ARRAY_DICT_WP_(TestVarEnumArray, eTypeTestEnum, "TestVartype\\Wp");
  FARAABIN_VAR_ENUM_ARRAY_DICT_CB_(TestVarEnumArray, eTypeTestEnum, VartypeTestAccessCallback);
  FARAABIN_VAR_ENUM_ARRAY_DICT_CB_WP_(TestVarEnumArray, eTypeTestEnum, VartypeTestAccessCallback, "TestVartype\\WpCb");

  FARAABIN_DICT_GROUP_FUNC_END_;
}

/**
 * @brief 
 * 
 */
static FARAABIN_VAR_TYPE_STRUCT_FUNC_(sTypeTestStruct)
{
  FARAABIN_VAR_TYPE_STRUCT_FUNC_START_(sTypeTestStruct);
  
  FARAABIN_VAR_TYPE_STRUCT_MEMBER_(uint8_t, A, 1);
  FARAABIN_VAR_TYPE_STRUCT_MEMBER_(uint8_t, B, 1);
  FARAABIN_VAR_TYPE_STRUCT_MEMBER_(uint8_t, C, 1);
  FARAABIN_VAR_TYPE_STRUCT_MEMBER_(uint8_t, D, 1);
  
  FARAABIN_VAR_TYPE_STRUCT_FUNC_END_(sTypeTestStruct);
}

/**
 * @brief 
 * 
 */
static FARAABIN_VAR_TYPE_STRUCT_FUNC_(sTypeTestStatic)
{
  FARAABIN_VAR_TYPE_STRUCT_FUNC_START_(sTypeTestStatic);
  
  FARAABIN_VAR_TYPE_STRUCT_MEMBER_(uint8_t, A, 1);
  
  FARAABIN_VAR_TYPE_STRUCT_FUNC_END_(sTypeTestStatic);
}

/**
 * @brief 
 * 
 */
static FARAABIN_VAR_TYPE_ENUM_FUNC_(eTypeTestEnum)
{
  FARAABIN_VAR_TYPE_ENUM_FUNC_START_(eTypeTestEnum);
  
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eA);
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eB);
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eC);
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eD);
  
  FARAABIN_VAR_TYPE_ENUM_FUNC_END_(eTypeTestEnum);
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
