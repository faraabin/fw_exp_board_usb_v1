/**
 ******************************************************************************
 * @file           : faraabin_fobject_mcu.c
 * @brief          :
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 FaraabinCo.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 *
 * https://faraabinco.ir/
 * https://github.com/FaraabinCo
 *
 ******************************************************************************
 * @verbatim
 * 
 * This fobject is the main link between Faraabin and user code. All frames sent
 * to MCU are passed through this fobject.
 * This fobject also calculates some statistics and manages requests from PC to access
 * Faraabin database and also main stage in the connection process.
 * All system events are also sent via this fobject internally.
 * 
 * @endverbatim
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_mcu.h"

#include "faraabin_fobject_system_event_wrapper.h"
#include "faraabin_fobject.h"
#include "faraabin_database.h"
#include "faraabin_config.h"

#include <stdlib.h>

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
sFaraabinFobjectMcu faraabin_mcu__; /*!< Singleton MCU Fobject of the Faraabin. */

/*
===============================================================================
                ##### faraabin_fobject_mcu.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes MCU Fobject.
 * 
 * @return result Returns '0' if success
 */
uint8_t fFaraabinFobjectMcu_Init(void) {
  
	FaraabinInit___ = true;
  if(fFaraabinDatabase_AddDict((uint32_t)&faraabin_mcu__) != 0U) {
    return 1;
  }
	FaraabinInit___ = false;
	
	faraabin_mcu__._type = (uint8_t)eFO_TYPE_MCU;
  faraabin_mcu__._init = FALSE;
	
	faraabin_mcu__.Enable = TRUE;
	faraabin_mcu__.Path = "System";

  faraabin_mcu__.Seq = 0U;
  
	faraabin_mcu__._isHostConnected = FALSE;
  
  fChrono_StartTimeoutMs(&(faraabin_mcu__.ChronoLiveTimeout), FB_LIVE_TIMEOUT_MS);
  
  faraabin_mcu__.StatisticsRxFramesCnt = 0U;
  faraabin_mcu__.StatisticsRxBytesCnt = 0U;
  faraabin_mcu__.StatisticsRxFramesEscapingErrorCnt = 0U;
  faraabin_mcu__.StatisticsRxFramesChecksumErrorCnt = 0U;
  faraabin_mcu__.StatisticsRxFramesOverrideErrorCnt = 0U;
  faraabin_mcu__.StatisticsRxFramesMinimumSizeErrorCnt = 0U;
  faraabin_mcu__.StatisticsTxFramesCnt = 0U;
  faraabin_mcu__.StatisticsTxBytesCnt = 0U;
	
	faraabin_mcu__.BootTimeMs = 0;
	faraabin_mcu__.BootTimeFirstFlag = TRUE;
	
  faraabin_mcu__._init = TRUE;
  return 0;
}

/**
 * @brief Runs all statistics in Faraabin related to the MCU Fobject.
 * 
 */
void fFaraabinFobjectMcu_Run(void) {

  if(!faraabin_mcu__._init) {
    return;
  }
	
	if(faraabin_mcu__.BootTimeFirstFlag) {
		
		faraabin_mcu__.BootTimeFirstFlag = false;
		
		faraabin_mcu__.BootTimeMs = fChrono_TimeSpanMs(fChrono_GetTickInitValue(), fChrono_GetTick());
	}
	
	FaraabinFlags.Status.Bitfield.ProfilerSend = faraabin_mcu__.IsSendingCpuProfiler;
	
	if(fChrono_IsTimeout(&(faraabin_mcu__.ChronoLiveTimeout)) == true) {
    
    fChrono_Start(&(faraabin_mcu__.ChronoLiveTimeout));
    
    faraabin_mcu__._isHostConnected = false;
  }
}

/**
 * @brief Sends system event to Faraabin.
 * 
 * @param eventId ID of the event.
 */
void fFaraabinFobjectMcu_SendEventSystem(uint16_t eventId) {
	
	Faraabin_EventSystem_End_((uint32_t)0xFFFFFFFFU, &faraabin_mcu__.Seq, faraabin_mcu__.Enable, eventId);
		
}

/**
 * @brief Sends system exception event to Faraabin.
 * 
 * @param eventId ID of the event.
 */
void fFaraabinFobjectMcu_SendEventSystemException(uint16_t eventId) {
	
	Faraabin_EventSystemException_End_((uint32_t)0xFFFFFFFFU, &faraabin_mcu__.Seq, faraabin_mcu__.Enable, eventId);
		
}

/**
 * @brief Sends system event response.
 * 
 * @param eventId ID of the event.
 * @param reqSeq Request sequence.
 */
void fFaraabinFobjectMcu_SendEventSystemResponse(uint16_t eventId, uint8_t reqSeq) {
	
	Faraabin_EventSystem_EndResponse_((uint32_t)0xFFFFFFFFU, &faraabin_mcu__.Seq, true, eventId, reqSeq);
		
}

/**
 * @brief Sends system exception event response.
 * 
 * @param eventId 
 * @param reqSeq 
 */
void fFaraabinFobjectMcu_SendEventSystemExceptionResponse(uint16_t eventId, uint8_t reqSeq) {
	
	Faraabin_EventSystemException_EndResponse_((uint32_t)0xFFFFFFFFU, &faraabin_mcu__.Seq, true, eventId, reqSeq);
		
}

/**
 * @brief Gets the pointer to the MCU Fobject.
 * 
 * @return ptr Pointer to the MCU fobject.
 */
sFaraabinFobjectMcu* fFaraabinFobjectMcu_GetFobject(void) {
	
	return &faraabin_mcu__;
}

/*
===============================================================================
                ##### faraabin_fobject_mcu.c Private Functions #####
===============================================================================*/

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
