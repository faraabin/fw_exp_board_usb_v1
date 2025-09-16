/**
 ******************************************************************************
 * @file           : faraabin_addon_unity.c
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
 * 
 * For being able to port unity report mechanism for Faraabin, and event group (UnityReport) 
 * is used. Using macros in unity_config.h, all reports can be sent to Faraabin.
 * 
 * @endverbatim
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin_addon_unity.h"

#include "../../faraabin.h"

#ifdef FARAABIN_ENABLE
#ifdef FB_ADD_ON_FEATURE_FLAG_UNITY

/* Private define ------------------------------------------------------------*/
#define UNITY_BUFFER_SIZE	1000

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
FARAABIN_EVENT_GROUP_DEF_STATIC_(UnityReport);

static sChrono UnityChrono;

static uint8_t UnityBuffer[UNITY_BUFFER_SIZE + 1];

static uint16_t UnityBufferIndex;

static float UnityTestExecutionTimeMs;

/* Private function prototypes -----------------------------------------------*/
static void ResetUnityBuffer(void);

/* Variables -----------------------------------------------------------------*/

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                          ##### Exported Functions #####                          ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/
/**
 * @brief Initializes unity addon.
 * 
 * @return result Returns '0' if successful, otherwise '1'.
 */
uint8_t fFaraabinAddOn_Unity_Init(void) {

  FARAABIN_EventGroup_Init_WithPath_(&UnityReport, "UNITY");
  FARAABIN_EventGroup_Enable_(&UnityReport);
  
  ResetUnityBuffer();
  
  return FARAABIN_UNITY_OK;
}

/**
 * @brief Adds outputted character of the unity library to a temporary buffer.
 * 
 * @note If temporary buffer is full, it will be flushed to send all data to Faraabin.
 *       This method is used to prevent sending reports character by character.
 * 
 * @param c character to add to buffer.
 */
void fFaraabinAddOn_Unity_Char(uint8_t c) {
	
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
  
  UnityBuffer[UnityBufferIndex] = c;
  UnityBufferIndex++;
  if(UnityBufferIndex >= UNITY_BUFFER_SIZE) {
    
    fFaraabinAddOn_Unity_Flush();
    
    UnityBufferIndex = 0;
  }
}

/**
 * @brief Flushes temporary buffer that holds unity outputted characters.
 * 
 */
void fFaraabinAddOn_Unity_Flush(void) {
	
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
  
  UnityBuffer[UnityBufferIndex] = '\0';
  FARAABIN_PutsToTerminal_(&UnityReport, (char*)UnityBuffer);

  ResetUnityBuffer();
	
	fFaraabin_FlushBuffer();
}

/**
 * @brief Resets the unity buffer and sends start message to faraabin.
 * 
 * @note Normally this is called at the beginning of the tests.
 * 
 */
void fFaraabinAddOn_Unity_Start(void) {
	
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
  
  ResetUnityBuffer();
  FARAABIN_PrintfToTerminal_(&UnityReport, "\r\n[%u]----Unity Start\r\n", fChrono_GetTick());
  
}

/**
 * @brief Flushes the unity buffer and send the complete message to faraabin.
 * 
 * @note Normally this is called at the end of the tests.
 * 
 */
void fFaraabinAddOn_Unity_Complete(void) {
	
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
  
	fFaraabinAddOn_Unity_Flush();
  FARAABIN_PrintfToTerminal_(&UnityReport, "\r\n[%u]----Unity Complete\r\n", fChrono_GetTick());
  
}

/**
 * @brief Starts test execution time.
 * 
 */
void fFaraabinAddOn_Unity_ExeTimeStart(void) {

  fChrono_Start(&UnityChrono);

}

/**
 * @brief Measures test execution time.
 * 
 */
void fFaraabinAddOn_Unity_ExeTimeStop(void) {

  UnityTestExecutionTimeMs = fChrono_ElapsedUs(&UnityChrono) / 1000.0f;

}

/**
 * @brief Prints test execution time.
 * 
 */
void fFaraabinAddOn_Unity_ExeTimePrint(void) {
	
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}

	char buffer[20];
	sprintf(buffer, " (exe: %.2fms)", UnityTestExecutionTimeMs);
	UnityPrint(buffer);
}

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                            ##### Private Functions #####                         ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/
/**
 * @brief Resets unity buffer.
 * 
 */
static void ResetUnityBuffer(void) {

  for(int i = 0; i < sizeof(UnityBuffer); i++) {
    UnityBuffer[i] = 0;
  }
  
  UnityBufferIndex = 0;
}

#endif
#endif

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
