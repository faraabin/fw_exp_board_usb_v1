/**
******************************************************************************
* @file           : faraabin_fobject_databus_test.c
* @brief          :
* @note           :
* @copyright      : COPYRIGHT© 2024 FaraabinCo
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright© 2024 FaraabinCo.
* All rights reserved.</center></h2>
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
* @verbatim
* @endverbatim
*/

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_databus_test.h"

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

  eA

}eTypeTest;

/**
 * @brief 
 * 
 */
typedef struct {

  uint8_t A;

}sTypeTest;

/* Private variables ---------------------------------------------------------*/
TEST_GROUP(DatabusTest);

FARAABIN_DATABUS_DEF_STATIC_(StaticDatabus);
FARAABIN_DATABUS_DEF_(DynamicDatabus);
FARAABIN_DATABUS_DEF_EXTERN_(DynamicDatabus);

static sFaraabinFobjectDataBus_Channel StaticChannelBuffer[10];
static sFaraabinFobjectDataBus_CaptureValue StaticCaptureBuffer[10];

static uint8_t TestVar[8];
static eTypeTest TestEnum[2];
static sTypeTest TestStruct[2];
static CPU_PROFILER_CODEBLOCK_DEF_(TestCodeBlock);

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
 * @brief Function that runs all unit tests and returns the result.
 * 
 * @return int 
 */
int FaraabinFobjectDataBusTest_Run(int argc, const char* argv[]) {

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
TEST_GROUP_RUNNER(DatabusTest) {
  
  RUN_TEST_CASE(DatabusTest, TemplateTest);
  
}

/**
 * @brief Run all tests in test group.
 * 
 */
static void RunTests(void) {
  RUN_TEST_GROUP(DatabusTest);
}

/**
 * @brief Unit test setup.
 * 
 */
TEST_SETUP(DatabusTest) {

}

/**
 * @brief Setup that executes before all tests.
 * 
 */
static void OneTimeSetup(void) {
  
  FARAABIN_DATABUS_SET_VALUE_(StaticDatabus.Enable, true);
  FARAABIN_DATABUS_SET_VALUE_(StaticDatabus.ChannelQty, 10);
  FARAABIN_DATABUS_SET_VALUE_(StaticDatabus.StreamDivideBy, 0);
  FARAABIN_DATABUS_SET_VALUE_(StaticDatabus.BufferCaptureSize, 10);
  FARAABIN_DATABUS_SET_VALUE_(StaticDatabus.TimerWindowMs, 1000);
  FARAABIN_DATABUS_SET_VALUE_(StaticDatabus.TimeAfterTrigMs, 1000);
  FARAABIN_DATABUS_SET_VALUE_(StaticDatabus.TimerDivideBy, 0);
  FARAABIN_DATABUS_SET_VALUE_(StaticDatabus.TrigDivideBy, 0);
  FARAABIN_DATABUS_SET_VALUE_(StaticDatabus.ApiTrigEnable, false);
  FARAABIN_DATABUS_SET_VALUE_(StaticDatabus.fpUserTerminalCallback, UserTerminalCallback);
  FARAABIN_DataBus_AdvFeat_SetBufferChannelsStatically_(&StaticDatabus, StaticChannelBuffer);
  FARAABIN_DataBus_AdvFeat_SetBufferCaptureStatically_(&StaticDatabus, StaticCaptureBuffer);
  FARAABIN_DataBus_Init_WithPath_(&StaticDatabus, "DatabusTest\\Wp");
  FARAABIN_DataBus_Enable_(&StaticDatabus);
  FARAABIN_DataBus_Disable_(&StaticDatabus);
  FARAABIN_DataBus_StartStreamMode_(&StaticDatabus);
  FARAABIN_DataBus_StartTriggerMode_(&StaticDatabus);
  FARAABIN_DataBus_ForceTrigger_(&StaticDatabus);
  FARAABIN_DataBus_Stop_(&StaticDatabus);
  FARAABIN_DataBus_AttachVariable_BOOL_(&StaticDatabus, (bool*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_BOOL_ARRAY_(&StaticDatabus, (bool*)TestVar);
  FARAABIN_DataBus_AttachVariable_U8_(&StaticDatabus, (uint8_t*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_U8_ARRAY_(&StaticDatabus, (uint8_t*)TestVar);
  FARAABIN_DataBus_AttachVariable_I8_(&StaticDatabus, (int8_t*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_I8_ARRAY_(&StaticDatabus, (int8_t*)TestVar);
  FARAABIN_DataBus_AttachVariable_U16_(&StaticDatabus, (uint16_t*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_U16_ARRAY_(&StaticDatabus, (uint16_t*)TestVar);
  FARAABIN_DataBus_AttachVariable_I16_(&StaticDatabus, (int16_t*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_I16_ARRAY_(&StaticDatabus, (int16_t*)TestVar);
  FARAABIN_DataBus_AttachVariable_U32_(&StaticDatabus, (uint32_t*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_U32_ARRAY_(&StaticDatabus, (uint32_t*)TestVar);
  FARAABIN_DataBus_AttachVariable_I32_(&StaticDatabus, (int32_t*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_I32_ARRAY_(&StaticDatabus, (int32_t*)TestVar);
  FARAABIN_DataBus_AttachVariable_U64_(&StaticDatabus, (uint64_t*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_U64_ARRAY_(&StaticDatabus, (uint64_t*)TestVar);
  FARAABIN_DataBus_AttachVariable_I64_(&StaticDatabus, (int64_t*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_I64_ARRAY_(&StaticDatabus, (int64_t*)TestVar);
  FARAABIN_DataBus_AttachVariable_F32_(&StaticDatabus, (float32_t*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_F32_ARRAY_(&StaticDatabus, (float32_t*)TestVar);
  FARAABIN_DataBus_AttachVariable_F64_(&StaticDatabus, (float64_t*)&TestVar[0]);
  FARAABIN_DataBus_AttachVariable_F64_ARRAY_(&StaticDatabus, (float64_t*)TestVar);
  FARAABIN_DataBus_AttachVariable_UserDefined_Enum_(&StaticDatabus, &TestEnum[0]);
  FARAABIN_DataBus_AttachVariable_UserDefined_Enum_ARRAY_(&StaticDatabus, TestEnum);
  FARAABIN_DataBus_AttachVariable_UserDefined_Struct_(&StaticDatabus, &TestStruct[0]);
  FARAABIN_DataBus_AttachVariable_UserDefined_Struct_ARRAY_(&StaticDatabus, TestStruct);
  FARAABIN_DataBus_AttachCodeBlock_(&StaticDatabus, &TestCodeBlock);
  FARAABIN_DataBus_DetachAllChannels_(&StaticDatabus);
  FARAABIN_DataBus_Run_(&StaticDatabus);

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
TEST_TEAR_DOWN(DatabusTest) {

}

/**
 * @brief Template test.
 * 
 */
TEST(DatabusTest, TemplateTest) {
  
  TEST_ASSERT(true);
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
