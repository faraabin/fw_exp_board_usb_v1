/**
 ******************************************************************************
 * @file           : faraabin_addon_cpu_profiler.h
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
#ifndef FARAABIN_ADDON_CPU_PROFILER_H
#define FARAABIN_ADDON_CPU_PROFILER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "../../faraabin_dependency.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief CPU profiler system event identifiers.
 * 
 */
typedef enum {
  
	eCB_EVENT_INFO_USER_DATA_RECEIVED = 0,
	
  eCB_EVENT_INFO_RUN,
  eCB_EVENT_INFO_RESET,
  
  eCB_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
  eCB_EVENT_ERROR_MCU_PARAM,
  
}eFaraabinCpuProfiler_SystemEventId;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
void fFaraabinAddOn_CpuProfiler_SendingEnable(void);
void fFaraabinAddOn_CpuProfiler_SendingDisable(void);
bool fFaraabinAddOn_CpuProfiler_IsSending(void);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_ADDON_CPU_PROFILER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
