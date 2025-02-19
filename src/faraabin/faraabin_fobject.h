/**
 ******************************************************************************
 * @file           : faraabin_fobject.h
 * @brief          : Faraabin fobject types header file.
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
#ifndef FARAABIN_FOBJECT_H
#define FARAABIN_FOBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_internal_config.h"

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/**
 * @brief Generates the filename as a string.
 * 
 */
#define FILENAME__  strrchr(__FILE__, (int)'\\') ? &(strrchr(__FILE__, (int)'\\')[1]) : __FILE__

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Faraabin fobject types.
 * 
 */
typedef enum {
	
	eFO_TYPE_MCU = 0,
	eFO_TYPE_GENERAL,
  eFO_TYPE_VAR,
	eFO_TYPE_VAR_TYPE,
  eFO_TYPE_EVENT_GROUP,
	eFO_TYPE_FUNCTION,
	eFO_TYPE_FUNCTION_GROUP,
	eFO_TYPE_FUNCTION_GROUP_TYPE,
  eFO_TYPE_DATABUS,
	eFO_TYPE_PROCESS,
	eFO_TYPE_CODE_BLOCK,
	eFO_TYPE_STATE_MACHINE,
	eFO_TYPE_CONTAINER,
	
	eFO_TYPE_FOBJECT_END,
	
	eFO_TYPE_FOBJECT,
	
	eFO_TYPE_STATE_MACHINE_STATE,
	eFO_TYPE_STATE_MACHINE_TRANSITION,
	eFO_TYPE_VAR_TYPE_ENUM_MEMBER,
	eFO_TYPE_VAR_TYPE_STRUCT_MEMBER,
	eFO_TYPE_ENTITY_NUMERICAL,
	eFO_TYPE_ENTITY_EVENT,
	eFO_TYPE_FUNCTION_GROUP_TYPE_MEMBER,
  
}eFaraabin_FobjectType;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
