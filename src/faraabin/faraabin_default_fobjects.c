/**
 ******************************************************************************
 * @file           : faraabin_default_fobjects.c
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
 * Default fobjects in Faraabin are:
 * 1- A very low speed databus that runs each 100ms.
 * 2- An event group to have access to FARAABIN_Printf_(), FARAABIN_PrintfWarning_,
 *    FARAABIN_PrintfError_(), FARAABIN_SendEvent_(), FARAABIN_SendEventWarning_(),
 *    FARAABIN_SendEventError_() macros. These macros are used to send user messages
 *    without creating a new event group.
 * 
 * @endverbatim
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin_default_fobjects.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
#if defined(FARAABIN_ENABLE)
#if defined(FB_FEATURE_FLAG_DEFAULT_DATABUS) || defined(FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP)
/**
 * @brief Default Fobjects path.
 * 
 */
static const char* DefaultFobjectPath = "System";
#endif
#endif

/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
#if defined(FB_FEATURE_FLAG_DEFAULT_DATABUS) && defined(FARAABIN_ENABLE)

/**
 * @brief Default databus declaration.
 * 
 */
FARAABIN_DATABUS_DEF_(DefaultDatabus);

/**
 * @brief Default databus chrono for managing its runtime.
 * 
 */
static sChrono DefaultDatabusRunner;
#endif

#if defined(FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP) && defined(FARAABIN_ENABLE)
/**
 * @brief Default event group declartion.
 * 
 */
FARAABIN_EVENT_GROUP_DEF_(DefaultEventGroup);
#endif

/*
===============================================================================
          ##### faraabin_default_fobjects.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes default fobjects.
 * 
 * @note This function is called internally and user does not need to call it manually.
 * 
 * @return result Returns '0' if successful, otherwise '1'. 
 */
uint8_t fFaraabinDefaultFobjects_Init(void) {
  
#if defined(FB_FEATURE_FLAG_DEFAULT_DATABUS) && defined(FARAABIN_ENABLE)
  
  FARAABIN_DATABUS_SET_VALUE_(DefaultDatabus.ChannelQty, FB_DEFAULT_DATABUS_CHANNEL_QTY);
  FARAABIN_DataBus_Init_WithPath_(&DefaultDatabus, DefaultFobjectPath);
  
  FARAABIN_DataBus_StartStreamMode_(&DefaultDatabus);
	FARAABIN_DataBus_Enable_(&DefaultDatabus);
  
  fChrono_StartTimeoutMs(&DefaultDatabusRunner, FB_DEFAULT_DATABUS_CYCLE_MS);
  
#endif
  
#if defined(FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP) && defined(FARAABIN_ENABLE)
  
  FARAABIN_EventGroup_Init_WithPath_(&DefaultEventGroup, DefaultFobjectPath);
  FARAABIN_EventGroup_Enable_(&DefaultEventGroup);
  
#endif

  return 0;
}

/**
 * @brief Runs default databus.
 * 
 */
void fFaraabinDefaultFobjects_Run(void) {
	
#if defined(FB_FEATURE_FLAG_DEFAULT_DATABUS) && defined(FARAABIN_ENABLE)
  
  if(fChrono_IsTimeout(&DefaultDatabusRunner)) {
    
    fChrono_StartTimeoutMs(&DefaultDatabusRunner, FB_DEFAULT_DATABUS_CYCLE_MS);
    
    FARAABIN_DataBus_Run_(&DefaultDatabus);
    
  }
	
#endif
}

/*
===============================================================================
          ##### faraabin_default_fobjects.c Private Functions #####
===============================================================================*/

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
