/**
 ******************************************************************************
 * @file           : faraabin_database.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_DATABASE_H
#define FARAABIN_DATABASE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
#if FB_MAX_FOBJECT_QTY >= 65535
  #error "Maximum Fobjects quantity is 65534"
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief Feature flag bits to be shown in Faraabin application on PC.
 * 
 */
typedef struct {
  
  uint32_t DefaultDataBus     : 1;  /*!< Specifies whether the default databus is activated. */
  uint32_t DefaultEventGroup  : 1;  /*!< Specifies whether the default event group is activated. */
  
  uint32_t McuCli             : 1;  /*!< Specifies whether the MCU CLI feature is activated. */
  uint32_t BufferOverFlow     : 1;  /*!< Specifies whether the buffer overflow notification is activated. */
  uint32_t Password           : 1;  /*!< Specifies whether the Password feature is activated. */
  
  uint32_t CpuProfiler        : 1;  /*!< Specifies whether the CPU profiler addon is activated. */
  uint32_t StateMachine       : 1;  /*!< Specifies whether the state machine addon is activated. */
  uint32_t Unity              : 1;  /*!< Specifies whether the unity library addon is activated. */
  
  uint32_t AllowSendDickBlocking : 1;  /*!< Reserved feature flag for future use. */
	
  uint32_t ReservedFlag09     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag10     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag11     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag12     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag13     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag14     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag15     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag16     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag17     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag18     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag19     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag20     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag21     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag22     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag23     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag24     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag25     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag26     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag27     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag28     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag29     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag30     : 1;  /*!< Reserved feature flag for future use. */
  uint32_t ReservedFlag31     : 1;  /*!< Reserved feature flag for future use. */
  
}sFaraabin_FeatureFlag;

/**
 * @brief Union type for the feature flags.
 * 
 */
typedef union {
  
  uint32_t U32;                   /*!< Views feature flags as uint32_t variable. */

  sFaraabin_FeatureFlag Bitfield; /*!< Views feature flags as bitfields. */
  
}uFaraabin_FeatureFlag;

/**
 * @brief Status flags to be shown in Faraabin application on PC.
 * 
 */
typedef struct {
  
  uint32_t McuReset           : 1;  /*!< Specifies the capability of resetting the MCU via Faraabin. */
  uint32_t NewDict            : 1;  /*!< Indicates the introduction of a new dictionary to Faraabin. */
  uint32_t BufferOverflow     : 1;  /*!< Indicates Faraabin buffer overflow. */
  uint32_t DictOverflow       : 1;  /*!< Indicates dictionary overflow. */
  uint32_t NullDict           : 1;  /*!< Indicates that a dictionary has null reference and is not valid. */
  uint32_t UnexpectedDict     : 1;  /*!< Indicates an unexpected dictionary error has occured. */
  
  uint32_t ProfilerSend       : 1;  /*!< Specifies whether CPU profiler data sending to Faraabin is enabled. */
  uint32_t ProfilerListOvf    : 1;  /*!< Indicates an overflow in the number of processes in CPU profiler. */
  uint32_t ProfilerDepthOvf   : 1;  /*!< Indicates an overflow in the number of nested processes in CPU profiler. */
  uint32_t ProfilerDuplicate  : 1;  /*!< Indicates a repetitive process in CPU profiler. */
  
  uint32_t DictDuplicate      : 1;  /*!< Indicates a repetitive dictionary. */
	
	uint32_t UninitializedFaraabin : 1;  /*!< Specifies that faraabin api called before initialize. */
  
	uint32_t ReservedFlag12     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag13     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag14     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag15     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag16     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag17     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag18     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag19     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag20     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag21     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag22     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag23     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag24     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag25     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag26     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag27     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag28     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag29     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag30     : 1;  /*!< Reserved status flag for future use. */
  uint32_t ReservedFlag31     : 1;  /*!< Reserved status flag for future use. */
  
}sFaraabin_StatusFlag;

/**
 * @brief Union for status flags.
 * 
 */
typedef union {
  
  uint32_t U32;                   /*!< Views status flags as uint32_t variable. */

  sFaraabin_StatusFlag Bitfield;  /*!< Views status flags as bitfields. */
  
}uFaraabin_StatusFlag;

/**
 * @brief Faraabin status and feature flags type definition.
 * 
 */
typedef struct {

  uFaraabin_FeatureFlag Features; /*!< Feature flags. */

  uFaraabin_StatusFlag Status;    /*!< Status flags. */

}sFaraabinFlags;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes Faraabin database
 * 
 * @return result returns '0' after initialization.
 */
uint8_t fFaraabinDatabase_Init(void);

/**
 * @brief Adds a new dictionary to Faraabin database.
 * 
 * @param fobjectPtr Pointer of the fobject.
 * @return result Returns '0' if dict is added successfully, '1' if fobject is already in the database and 2 if database is full.
 */
uint8_t fFaraabinDatabase_AddDict(uint32_t fobjectPtr);

/**
 * @brief Gets the number of added fobject to database.
 * 
 * @return numberOfItems Number of items in database.
 */
uint16_t fFaraabinDatabase_GetNumberOfAddedDicts(void);

/**
 * @brief Check for existance of dict in list
 * 
 * @param fobjectPtr Pointer of the fobject.
 * @return result Returns TRUE if dict is exist, FALSE if not exist.
 */
bool fFaraabinDatabase_IsDictExist(uint32_t fobjectPtr);

/**
 * @brief Gets pointer of a fobject in database.
 * 
 * @param dictIndex Index of the dictionary in database.
 * @return fobjectPtr Pointer of the fobject.
 */
uint32_t fFaraabinDatabase_GetFobjectPointerFromDict(uint16_t dictIndex);

/**
 * @brief Returns enable status of all feature flags as a uint32_t variable.
 * 
 * @note Each bit represents a specific flag.
 * 
 * @return flagsStat Status of the feature flags.
 */
uint32_t fFaraabinDatabase_GetAllFeatureFlags(void);

/**
 * @brief Returns enable status of all status flags as a uint16_t variable.
 * 
 * @note Each bit represents a specific flag.
 * 
 * @return flagsStat Status of the status flags.
 */
uint32_t fFaraabinDatabase_GetAllStatusFlags(void);

/**
 * @brief Gets the RAM usage of the CPU.
 * 
 * @return uint32_t Amount of RAM usage in bytes.
 */
uint32_t fFaraabinDatabase_GetRamUsage(void);

/* Exported variables --------------------------------------------------------*/
extern sFaraabinFlags FaraabinFlags;

#ifdef __cplusplus
}
#endif

#endif /* __FARAABIN_DATABASE_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
