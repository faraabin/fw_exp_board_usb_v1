/**
 ******************************************************************************
 * @file           : faraabin_addon_state_machine.h
 * @brief          : Faraabin state machine fobject header file.
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
#ifndef FARAABIN_ADDON_STATE_MACHINE_H
#define FARAABIN_ADDON_STATE_MACHINE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../../faraabin_dependency.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief State machine system event type definitions.
 * 
 */
typedef enum {
	
	eSM_EVENT_INFO_USER_DATA_RECEIVED = 0,
	
	eSM_EVENT_INFO_TRANSITION_OCCURRED,
	eSM_EVENT_INFO_TRANSITION_REJECTED,
	eSM_EVENT_INFO_STATE_ENTERED,
	eSM_EVENT_INFO_STATE_EXITED,
	eSM_EVENT_INFO_CMD_MANUAL_TRANSITION,
	eSM_EVENT_INFO_CMD_VIRTUAL_TRANSITION,
	eSM_EVENT_INFO_CMD_RESET,
	
	eSM_EVENT_INFO_DICT_END,
	
	eSM_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
	eSM_EVENT_ERROR_MANUAL_TRANSITION_SRC_MISMATCH,
	eSM_EVENT_ERROR_MANUAL_TRANSITION_DST_NULL,
	eSM_EVENT_ERROR,
	
}eFaraabinFobjectStateMachine_SystemEventId;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_ADDON_STATE_MACHINE_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
