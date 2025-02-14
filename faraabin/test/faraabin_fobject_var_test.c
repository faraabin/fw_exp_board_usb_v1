/**
 ******************************************************************************
 * @file           : faraabin_fobject_var_test.c
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
#include "faraabin_fobject_var_test.h"

#include "unity_fixture.h"
#include "faraabin.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
TEST_GROUP(VarTest);

FARAABIN_DICT_GROUP_DEF_STATIC_(VarTestDg);

// Variables christmas tree
// *
// ***
// *****
// *******
// *********
// ***********
// *************
// ***************
// *****************
// *******************
static bool_t TestBool;
static bool_t TestBoolWp;
static bool_t TestBoolCb;
static bool_t TestBoolWpCb;
static bool_t TestBoolArray[3];
static bool_t TestBoolArrayWp[3];
static bool_t TestBoolArrayCb[3];
static bool_t TestBoolArrayWpCb[3];
static uint8_t TestU8;
static uint8_t TestU8Wp;
static uint8_t TestU8Cb;
static uint8_t TestU8WpCb;
static uint8_t TestU8Array[3];
static uint8_t TestU8ArrayWp[3];
static uint8_t TestU8ArrayCb[3];
static uint8_t TestU8ArrayWpCb[3];
static uint16_t TestU16;
static uint16_t TestU16Wp;
static uint16_t TestU16Cb;
static uint16_t TestU16WpCb;
static uint16_t TestU16Array[3];
static uint16_t TestU16ArrayWp[3];
static uint16_t TestU16ArrayCb[3];
static uint16_t TestU16ArrayWpCb[3];
static uint32_t TestU32;
static uint32_t TestU32Wp;
static uint32_t TestU32Cb;
static uint32_t TestU32WpCb;
static uint32_t TestU32Array[3];
static uint32_t TestU32ArrayWp[3];
static uint32_t TestU32ArrayCb[3];
static uint32_t TestU32ArrayWpCb[3];
static uint64_t TestU64;
static uint64_t TestU64Wp;
static uint64_t TestU64Cb;
static uint64_t TestU64WpCb;
static uint64_t TestU64Array[3];
static uint64_t TestU64ArrayWp[3];
static uint64_t TestU64ArrayCb[3];
static uint64_t TestU64ArrayWpCb[3];
static int8_t TestI8;
static int8_t TestI8Wp;
static int8_t TestI8Cb;
static int8_t TestI8WpCb;
static int8_t TestI8Array[3];
static int8_t TestI8ArrayWp[3];
static int8_t TestI8ArrayCb[3];
static int8_t TestI8ArrayWpCb[3];
static int16_t TestI16;
static int16_t TestI16Wp;
static int16_t TestI16Cb;
static int16_t TestI16WpCb;
static int16_t TestI16Array[3];
static int16_t TestI16ArrayWp[3];
static int16_t TestI16ArrayCb[3];
static int16_t TestI16ArrayWpCb[3];
static int32_t TestI32;
static int32_t TestI32Wp;
static int32_t TestI32Cb;
static int32_t TestI32WpCb;
static int32_t TestI32Array[3];
static int32_t TestI32ArrayWp[3];
static int32_t TestI32ArrayCb[3];
static int32_t TestI32ArrayWpCb[3];
static int64_t TestI64;
static int64_t TestI64Wp;
static int64_t TestI64Cb;
static int64_t TestI64WpCb;
static int64_t TestI64Array[3];
static int64_t TestI64ArrayWp[3];
static int64_t TestI64ArrayCb[3];
static int64_t TestI64ArrayWpCb[3];
static float32_t TestF32;
static float32_t TestF32Wp;
static float32_t TestF32Cb;
static float32_t TestF32WpCb;
static float32_t TestF32Array[3];
static float32_t TestF32ArrayWp[3];
static float32_t TestF32ArrayCb[3];
static float32_t TestF32ArrayWpCb[3];
static float64_t TestF64;
static float64_t TestF64Wp;
static float64_t TestF64Cb;
static float64_t TestF64WpCb;
static float64_t TestF64Array[3];
static float64_t TestF64ArrayWp[3];
static float64_t TestF64ArrayCb[3];
static float64_t TestF64ArrayWpCb[3];
// *******************
// *******************
// *******************
// *******************

/* Private function prototypes -----------------------------------------------*/
static void RunTests(void);
static void OneTimeSetup(void);
static void OneTimeTeardown(void);

static uint8_t VarTestAccessCallback(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size);

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
int FaraabinFobjectVarTest_Run(int argc, const char* argv[]) {

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
TEST_GROUP_RUNNER(VarTest) {
  
  RUN_TEST_CASE(VarTest, TemplateTest);
  
}

/**
 * @brief Run all tests in test group.
 * 
 */
static void RunTests(void) {
  RUN_TEST_GROUP(VarTest);
}

/**
 * @brief Unit test setup.
 * 
 */
TEST_SETUP(VarTest) {

}

/**
 * @brief Unit test teardown.
 * 
 */
TEST_TEAR_DOWN(VarTest) {

}

/**
 * @brief Template test.
 * 
 */
TEST(VarTest, TemplateTest) {
  
  TEST_ASSERT(true);
}

/**
 * @brief Setup that executes before all tests.
 * 
 */
static void OneTimeSetup(void) {

  FARAABIN_DictGroup_Init_(&VarTestDg);
  
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
static uint8_t VarTestAccessCallback(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size) {

  return 0;
}

/**
 * @brief DictGroup function
 * 
 */
FARAABIN_DICT_GROUP_FUNC_(VarTestDg) {

  FARAABIN_VAR_BOOL_DICT_(TestBool);
  FARAABIN_VAR_BOOL_DICT_WP_(TestBoolWp, "TestVar\\Wp");
  FARAABIN_VAR_BOOL_DICT_CB_(TestBoolCb, VarTestAccessCallback);
  FARAABIN_VAR_BOOL_DICT_CB_WP_(TestBoolWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_BOOL_ARRAY_DICT_(TestBoolArray);
  FARAABIN_VAR_BOOL_ARRAY_DICT_WP_(TestBoolArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_BOOL_ARRAY_DICT_CB_(TestBoolArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_BOOL_ARRAY_DICT_CB_WP_(TestBoolArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_U8_DICT_(TestU8);
  FARAABIN_VAR_U8_DICT_WP_(TestU8Wp, "TestVar\\Wp");
  FARAABIN_VAR_U8_DICT_CB_(TestU8Cb, VarTestAccessCallback);
  FARAABIN_VAR_U8_DICT_CB_WP_(TestU8WpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_U8_ARRAY_DICT_(TestU8Array);
  FARAABIN_VAR_U8_ARRAY_DICT_WP_(TestU8ArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_U8_ARRAY_DICT_CB_(TestU8ArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_U8_ARRAY_DICT_CB_WP_(TestU8ArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_U16_DICT_(TestU16);
  FARAABIN_VAR_U16_DICT_WP_(TestU16Wp, "TestVar\\Wp");
  FARAABIN_VAR_U16_DICT_CB_(TestU16Cb, VarTestAccessCallback);
  FARAABIN_VAR_U16_DICT_CB_WP_(TestU16WpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_U16_ARRAY_DICT_(TestU16Array);
  FARAABIN_VAR_U16_ARRAY_DICT_WP_(TestU16ArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_U16_ARRAY_DICT_CB_(TestU16ArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_U16_ARRAY_DICT_CB_WP_(TestU16ArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_U32_DICT_(TestU32);
  FARAABIN_VAR_U32_DICT_WP_(TestU32Wp, "TestVar\\Wp");
  FARAABIN_VAR_U32_DICT_CB_(TestU32Cb, VarTestAccessCallback);
  FARAABIN_VAR_U32_DICT_CB_WP_(TestU32WpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_U32_ARRAY_DICT_(TestU32Array);
  FARAABIN_VAR_U32_ARRAY_DICT_WP_(TestU32ArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_U32_ARRAY_DICT_CB_(TestU32ArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_U32_ARRAY_DICT_CB_WP_(TestU32ArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_U64_DICT_(TestU64);
  FARAABIN_VAR_U64_DICT_WP_(TestU64Wp, "TestVar\\Wp");
  FARAABIN_VAR_U64_DICT_CB_(TestU64Cb, VarTestAccessCallback);
  FARAABIN_VAR_U64_DICT_CB_WP_(TestU64WpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_U64_ARRAY_DICT_(TestU64Array);
  FARAABIN_VAR_U64_ARRAY_DICT_WP_(TestU64ArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_U64_ARRAY_DICT_CB_(TestU64ArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_U64_ARRAY_DICT_CB_WP_(TestU64ArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_I8_DICT_(TestI8);
  FARAABIN_VAR_I8_DICT_WP_(TestI8Wp, "TestVar\\Wp");
  FARAABIN_VAR_I8_DICT_CB_(TestI8Cb, VarTestAccessCallback);
  FARAABIN_VAR_I8_DICT_CB_WP_(TestI8WpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_I8_ARRAY_DICT_(TestI8Array);
  FARAABIN_VAR_I8_ARRAY_DICT_WP_(TestI8ArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_I8_ARRAY_DICT_CB_(TestI8ArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_I8_ARRAY_DICT_CB_WP_(TestI8ArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_I16_DICT_(TestI16);
  FARAABIN_VAR_I16_DICT_WP_(TestI16Wp, "TestVar\\Wp");
  FARAABIN_VAR_I16_DICT_CB_(TestI16Cb, VarTestAccessCallback);
  FARAABIN_VAR_I16_DICT_CB_WP_(TestI16WpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_I16_ARRAY_DICT_(TestI16Array);
  FARAABIN_VAR_I16_ARRAY_DICT_WP_(TestI16ArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_I16_ARRAY_DICT_CB_(TestI16ArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_I16_ARRAY_DICT_CB_WP_(TestI16ArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_I32_DICT_(TestI32);
  FARAABIN_VAR_I32_DICT_WP_(TestI32Wp, "TestVar\\Wp");
  FARAABIN_VAR_I32_DICT_CB_(TestI32Cb, VarTestAccessCallback);
  FARAABIN_VAR_I32_DICT_CB_WP_(TestI32WpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_I32_ARRAY_DICT_(TestI32Array);
  FARAABIN_VAR_I32_ARRAY_DICT_WP_(TestI32ArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_I32_ARRAY_DICT_CB_(TestI32ArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_I32_ARRAY_DICT_CB_WP_(TestI32ArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_I64_DICT_(TestI64);
  FARAABIN_VAR_I64_DICT_WP_(TestI64Wp, "TestVar\\Wp");
  FARAABIN_VAR_I64_DICT_CB_(TestI64Cb, VarTestAccessCallback);
  FARAABIN_VAR_I64_DICT_CB_WP_(TestI64WpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_I64_ARRAY_DICT_(TestI64Array);
  FARAABIN_VAR_I64_ARRAY_DICT_WP_(TestI64ArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_I64_ARRAY_DICT_CB_(TestI64ArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_I64_ARRAY_DICT_CB_WP_(TestI64ArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_F32_DICT_(TestF32);
  FARAABIN_VAR_F32_DICT_WP_(TestF32Wp, "TestVar\\Wp");
  FARAABIN_VAR_F32_DICT_CB_(TestF32Cb, VarTestAccessCallback);
  FARAABIN_VAR_F32_DICT_CB_WP_(TestF32WpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_F32_ARRAY_DICT_(TestF32Array);
  FARAABIN_VAR_F32_ARRAY_DICT_WP_(TestF32ArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_F32_ARRAY_DICT_CB_(TestF32ArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_F32_ARRAY_DICT_CB_WP_(TestF32ArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_F64_DICT_(TestF64);
  FARAABIN_VAR_F64_DICT_WP_(TestF64Wp, "TestVar\\Wp");
  FARAABIN_VAR_F64_DICT_CB_(TestF64Cb, VarTestAccessCallback);
  FARAABIN_VAR_F64_DICT_CB_WP_(TestF64WpCb, VarTestAccessCallback, "TestVar\\WpCb");
  FARAABIN_VAR_F64_ARRAY_DICT_(TestF64Array);
  FARAABIN_VAR_F64_ARRAY_DICT_WP_(TestF64ArrayWp, "TestVar\\Wp");
  FARAABIN_VAR_F64_ARRAY_DICT_CB_(TestF64ArrayCb, VarTestAccessCallback);
  FARAABIN_VAR_F64_ARRAY_DICT_CB_WP_(TestF64ArrayWpCb, VarTestAccessCallback, "TestVar\\WpCb");

  FARAABIN_DICT_GROUP_FUNC_END_;
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
