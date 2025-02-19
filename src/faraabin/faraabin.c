/**
 ******************************************************************************
 * @file           : faraabin.c
 * @brief          : This is the main core for running faraabin in the embedded
 *                   software.
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
 @mainpage
 
 @mainpage
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin.h"

#include "faraabin_config.h"
#include "faraabin_port.h"

#ifdef FB_ADD_ON_FEATURE_FLAG_UNITY
#include "add_on/unity/faraabin_addon_unity.h"
#endif

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
#include "cpu_profiler.h"
#endif

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/**
 * @brief Defining primitive variable types in C/C++, so that faraabin can detect
 *        variables of these types.
 * 
 */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(bool_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(uint8_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(int8_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(uint16_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(int16_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(uint32_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(int32_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(uint64_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(int64_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(float32_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(float64_t);

/**
 * @brief All fobjects that do not have a specified path in their declaration will
 *        use "root" as their default path.
 * 
 */
const char* RootPath____ = "root";

/**
 * @brief Flag that show faraabin initialize.
 * 
 */
bool FaraabinInit___ = false;

extern sFaraabinFobjectMcu faraabin_mcu__;

/*
===============================================================================
                      ##### faraabin.c Exported Functions #####
===============================================================================*/
/**
 * @brief This function initializes faraabin.
 * 
 * @note User need to call this function once in the initialization procedure of the software.
 * 
 * @return result Can be one of FARAABIN_RET values.
 */
fb_result_t fFaraabin_Init(void) {

  if(fFaraabinLinkSerializer_Init(fFaraabin_GetTxBufferPointer(), fFaraabin_GetTxBufferSize()) != 0U) {
    return FARAABIN_SERIALIZER_INIT_FAILED;
  }

  if(fFaraabinLinkHandler_Init(fFaraabin_GetRxBufferPointer(), fFaraabin_GetRxBufferSize()) != FB_LINK_HANDLER_RESULT_OK) {
    return FARAABIN_LINK_HANDLER_INIT_FAILED;
  }

  if(fFaraabinDatabase_Init() != 0U) {
    return FARAABIN_DATA_BASE_INIT_FAILED;
  }
  
  if(fFaraabinFobjectMcu_Init() != 0U) {
    return FARAABIN_MCU_FOBJECT_INIT_FAILED;
  }

  if(fFaraabinFunctionEngine_Init() != 0U) {
    return FARAABIN_FUNCTION_ENGINE_INIT_FAILED;
  }

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
	if(fCpuProfiler_Init() != 0) {
		return 1;
	}
#endif
	
	FaraabinInit___ = true;
	
#ifdef FB_ADD_ON_FEATURE_FLAG_UNITY
	fFaraabinAddOn_Unity_Init();
#endif
	
	FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(bool_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(uint8_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(int8_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(uint16_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(int16_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(uint32_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(int32_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(uint64_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(int64_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(float32_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(float64_t);
	
	if(fFaraabinDefaultFobjects_Init() != 0U) {
    return FARAABIN_DEFAULT_FOBJECTS_INIT_FAILED;
  }
	
	fFaraabinFobjectMcu_SendEventSystem(eMCU_EVENT_INFO_BOOT);
  
  return FARAABIN_OK;
}

/**
 * @brief This function enable faraabin.
 */
void fFaraabin_Enable(void) {
	
	faraabin_mcu__.Enable = TRUE;
}

/**
 * @brief This function disable faraabin.
 */
void fFaraabin_Disable(void) {
	
	faraabin_mcu__.Enable = FALSE;
}

bool fFaraabin_IsAllowEvent(void) {
	
	if(!faraabin_mcu__.Enable) {
		return false;
	}
	
	if(LinkHandler.DictSendingMode.SendFlag) {
		return false;
	}
	
	return true;
}

/**
 * @brief This function executes faraabin main modules. This way probe data are prepared and
 *        can be sent via faraabin link to the PC.
 * 
 * @note This function can be executed in a low priority runtime of the software.
 * 
 * @attention Faraabin uses a queue to hold incoming probe data from various parts of the software.
 *            To prevent data mixing, faraabin utilizes a critical section mechanism.
 *            For more details, refer to the user manual.
 * 
 */
void fFaraabin_Run(void) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
	}
  
  fFaraabinFobjectMcu_Run();
  fFaraabinFunctionEngine_Run();
  fFaraabinLinkHandler_Run();
  fFaraabinDefaultFobjects_Run();
	
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
	fCpuProfiler_Run();
#endif

  // Looping over all dictionaries to find databus pointers.
  // After finding databus pointer, captured data of that databus is sent.
  uint16_t dictQty = fFaraabinDatabase_GetNumberOfAddedDicts();
  for(uint16_t i = 0U; i < dictQty; i++) {

    uint32_t fobjectPtr = fFaraabinDatabase_GetFobjectPointerFromDict(i);
    uint8_t *fobjectType = (uint8_t*)fobjectPtr;

    if((eFaraabin_FobjectType)(*fobjectType) == eFO_TYPE_DATABUS) {
      
      fFaraabinFobjectDataBus_SendCaptureDataRun((sFaraabinFobjectDataBus*)fobjectPtr);

    }
  }
}

/**
 * @brief This function must be called after receving a byte from the link dedicated to faraabin.
 * 
 * @note If the user has a link that can fetch chunks of data, in the receive callback of their link,
 *       they should loop over all the fetched data and call this function for each received character.
 * 
 * @param c Received character.
 */
void fFaraabin_CharReceived(uint8_t c) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
	}

  fFaraabinLinkHandler_CharReceived(c);

}

/**
 * @brief Sets a new password for the faraabin.
 * 
 * @note This password is requested from user in the faraabin application (on PC) to initiate the connection procedure.
 *       Password is requested only if fFaraabin_PasswordEnable() is called.
 * 
 * @param password Password for starting the connection procedure in faraabin application.
 */
void fFaraabin_PasswordSet(char *password) {
  
  fFaraabinLinkHandler_SetPassword(password);
  
}

/**
 * @brief Enables request of the password in faraabin application.
 * 
 */
void fFaraabin_PasswordEnable(void) {
  
  FaraabinFlags.Features.Bitfield.Password = 1U;

}

/**
 * @brief Disables request of the password in faraabin application.
 * 
 */
void fFaraabin_PasswordDisable(void) {
  
  FaraabinFlags.Features.Bitfield.Password = 0U;
  
}

/**
 * @brief This function checks if the host (PC application) is connected to the MCU or not.
 * 
 * @retval IsHostConnected Connection status.
 */
bool fFaraabin_IsHostConnected(void) {
  
  return fFaraabinFobjectMcu_GetFobject()->_isHostConnected;
}

void fFaraabin_FlushBuffer(void) {
	
	fFaraabinLinkHandler_FlushBuffer();
}

/*
===============================================================================
                    ##### faraabin.c Private Functions #####
===============================================================================*/

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
