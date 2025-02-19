/**
 ******************************************************************************
 * @file           : faraabin_database.c
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
 * @endverbatim
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin_database.h"

#include "faraabin_link_handler.h"
#include "faraabin_fobject_mcu.h"
#include "faraabin_config.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/**
 * @brief Faraabin database object.
 * 
 */
typedef struct {
  
  uint32_t DatabaseArray[FB_MAX_FOBJECT_QTY];
	
  uint16_t DatabaseArrayIndex;
  
}sFaraabinDatabase;

/* Private variables ---------------------------------------------------------*/
/**
 * @brief Faraabin database instance.
 * 
 */
static sFaraabinDatabase FaraabinDatabase;

/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/**
 * @brief Faraabin flags singleton.
 * 
 */
sFaraabinFlags FaraabinFlags;

/*
===============================================================================
                  ##### fb_database.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes Faraabin database
 * 
 * @return result returns '0' after initialization.
 */
uint8_t fFaraabinDatabase_Init(void) {
  
  for(uint16_t i = 1; i < FB_MAX_FOBJECT_QTY; i++) {
    
    FaraabinDatabase.DatabaseArray[i] = 0U;
  }
  
  FaraabinDatabase.DatabaseArrayIndex = 0U;

  FaraabinFlags.Features.U32 = 0U;
  FaraabinFlags.Status.U32 = 0U;
  
#ifdef FB_FEATURE_FLAG_DEFAULT_DATABUS
  FaraabinFlags.Features.Bitfield.DefaultDataBus = 1U;
#endif
#ifdef FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP
  FaraabinFlags.Features.Bitfield.DefaultEventGroup = 1U;
#endif

#ifdef FB_FEATURE_FLAG_MCU_CLI
  FaraabinFlags.Features.Bitfield.McuCli = 1U;
#endif
#ifdef FB_FEATURE_FLAG_BUFFER_OVF
  FaraabinFlags.Features.Bitfield.BufferOverFlow = 1U;
#endif

#ifdef FB_ADD_ON_FEATURE_FLAG_UNITY
  FaraabinFlags.Features.Bitfield.Unity = 1U;
#endif
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  FaraabinFlags.Features.Bitfield.CpuProfiler = 1U;
#endif
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
  FaraabinFlags.Features.Bitfield.StateMachine = 1U;
#endif

#ifdef FB_FEATURE_FLAG_ALLOW_SEND_DICT_BLOCKING
  FaraabinFlags.Features.Bitfield.AllowSendDickBlocking = 1U;
#endif

  FaraabinFlags.Status.Bitfield.McuReset = 1U;
  
  return 0U;
}

/**
 * @brief Adds a new dictionary to Faraabin database.
 * 
 * @param fobjectPtr Pointer of the fobject.
 * @return result Returns 0 if dict is added successfully, 1 if fobject is already in the database and 2 if database is full.
 */
uint8_t fFaraabinDatabase_AddDict(uint32_t fobjectPtr) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}
  
  if(fobjectPtr == 0U) {

    FaraabinFlags.Status.Bitfield.NullDict = 1U;
    fFaraabinFobjectMcu_GetFobject()->_init = false;
    
    return 1;
  }
	
	if(fFaraabinDatabase_IsDictExist(fobjectPtr)) {
		
		FaraabinFlags.Status.Bitfield.DictDuplicate = 1U;
		return 1;
	}
	
	if(LinkHandler.DictSendingMode.SendFlag) {
		
		FaraabinFlags.Status.Bitfield.UnexpectedDict = 1U;
    fFaraabinFobjectMcu_GetFobject()->_init = false;
		
		return 1;
	}
  
  if(FaraabinDatabase.DatabaseArrayIndex >= (uint16_t)((uint16_t)FB_MAX_FOBJECT_QTY - (uint16_t)1)) {
    
    FaraabinFlags.Status.Bitfield.DictOverflow = 1U;
    fFaraabinFobjectMcu_GetFobject()->_init = false;
    
    return 1;
  }
  
  FaraabinDatabase.DatabaseArray[FaraabinDatabase.DatabaseArrayIndex] = fobjectPtr;
  FaraabinFlags.Status.Bitfield.NewDict = 1U;
  
  FaraabinDatabase.DatabaseArrayIndex++;
  
  return 0;
}

/**
 * @brief Gets the number of added fobject to database.
 * 
 * @return numberOfItems Number of items in database.
 */
uint16_t fFaraabinDatabase_GetNumberOfAddedDicts(void) {
  return FaraabinDatabase.DatabaseArrayIndex;
}

/**
 * @brief Check for existance of dict in list
 * 
 * @param fobjectPtr Pointer of the fobject.
 * @return result Returns TRUE if dict is exist, FALSE if not exist.
 */
bool fFaraabinDatabase_IsDictExist(uint32_t fobjectPtr) {
	
	for(uint32_t i = 0; i < FB_MAX_FOBJECT_QTY; i++) {\
		
		if(FaraabinDatabase.DatabaseArray[i] == fobjectPtr) {
			return true;
		}
	}
	
	return false;
}

/**
 * @brief Gets pointer of a fobject in database.
 * 
 * @param dictIndex Index of the dictionary in database.
 * @return fobjectPtr Pointer of the fobject.
 */
uint32_t fFaraabinDatabase_GetFobjectPointerFromDict(uint16_t dictIndex) {
  
  if(dictIndex >= FaraabinDatabase.DatabaseArrayIndex) {
    //TODO: Send error
    return 0;
  }
  
  return FaraabinDatabase.DatabaseArray[dictIndex];
}

/**
 * @brief Returns enable status of all feature flags as a uint32_t variable.
 * 
 * @note Each bit represents a specific flag.
 * 
 * @return flagsStat Status of the feature flags.
 */
uint32_t fFaraabinDatabase_GetAllFeatureFlags(void) {
  return FaraabinFlags.Features.U32;
}

/**
 * @brief Returns enable status of all status flags as a uint16_t variable.
 * 
 * @note Each bit represents a specific flag.
 * 
 * @return flagsStat Status of the status flags.
 */
uint32_t fFaraabinDatabase_GetAllStatusFlags(void) {
  return FaraabinFlags.Status.U32;
}

/**
 * @brief Gets the RAM usage of the CPU.
 * 
 * @return uint32_t Amount of RAM usage in bytes.
 */
uint32_t fFaraabinDatabase_GetRamUsage(void) {
	
	return sizeof(sFaraabinDatabase) + sizeof(sFaraabinFlags);
}

/*
===============================================================================
                    ##### fb_database.c Private Functions #####
===============================================================================*/

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
