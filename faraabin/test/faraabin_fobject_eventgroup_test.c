/**
 ******************************************************************************
 * @file           : faraabin_fobject_eventgroup_test.c
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
#include "faraabin_fobject_eventgroup_test.h"

#include "unity_fixture.h"
#include "faraabin.h"

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
TEST_GROUP(EventgroupTest);

FARAABIN_EVENT_GROUP_DEF_(EventgroupTest);
FARAABIN_EVENT_GROUP_DEF_EXTERN_(EventgroupTest);
FARAABIN_EVENT_GROUP_DEF_STATIC_(EventgroupStaticTest);

static char Text[] = "TEST STRING";
FARAABIN_VAR_TYPE_DEF_STATIC_(eTestEvent);

/* Private function prototypes -----------------------------------------------*/
static void RunTests(void);
static void OneTimeSetup(void);
static void OneTimeTeardown(void);

static void UserTerminalCallback(uint8_t *userData, uint16_t userDataSize);

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
int FaraabinFobjectEventgroupTest_Run(int argc, const char* argv[]) {

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
TEST_GROUP_RUNNER(EventgroupTest) {
  
  RUN_TEST_CASE(EventgroupTest, TemplateTest);
  
}

/**
 * @brief Run all tests in test group.
 * 
 */
static void RunTests(void) {
  RUN_TEST_GROUP(EventgroupTest);
}

/**
 * @brief Unit test setup.
 * 
 */
TEST_SETUP(EventgroupTest) {

}

/**
 * @brief Unit test teardown.
 * 
 */
TEST_TEAR_DOWN(EventgroupTest) {

}

/**
 * @brief Template test.
 * 
 */
TEST(EventgroupTest, TemplateTest) {
  
  TEST_ASSERT(true);
}

/**
 * @brief Setup that executes before all tests.
 * 
 */
static void OneTimeSetup(void) {

  FARAABIN_EVENT_GROUP_SET_VALUE_(EventgroupTest.Enable, true);
  FARAABIN_EVENT_GROUP_SET_VALUE_(EventgroupTest.fpUserTerminalCallback, UserTerminalCallback);

  FARAABIN_EventGroup_Init_WithPath_(&EventgroupStaticTest, "EventgroupTest\\Wp");
  FARAABIN_EventGroup_Init_(&EventgroupTest);
  FARAABIN_EventGroup_Enable_(&EventgroupTest);
  FARAABIN_EventGroup_Disable_(&EventgroupTest);

  FARAABIN_PrintfTo_(&EventgroupTest, "Printf %s", Text);
  FARAABIN_PrintfWarningTo_(&EventgroupTest, "Printf %s", Text);
  FARAABIN_PrintfErrorTo_(&EventgroupTest, "Printf %s", Text);
  FARAABIN_PrintfToTerminal_(&EventgroupTest, "Printf %s", Text);
  FARAABIN_PutsTo_(&EventgroupTest, Text);
  FARAABIN_PutsWarningTo_(&EventgroupTest, Text);
  FARAABIN_PutsErrorTo_(&EventgroupTest, Text);
  FARAABIN_PutsToTerminal_(&EventgroupTest, Text);
  FARAABIN_SendEventTo_(&EventgroupTest, eTestEvent, eA);
  FARAABIN_SendEventWarningTo_(&EventgroupTest, eTestEvent, eA);
  FARAABIN_SendEventErrorTo_(&EventgroupTest, eTestEvent, eA);

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
 * @param userData 
 * @param userDataSize 
 */
static void UserTerminalCallback(uint8_t *userData, uint16_t userDataSize) {

}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
