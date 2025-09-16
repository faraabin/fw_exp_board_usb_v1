/**
 ******************************************************************************
 * @file           : unity_config.h
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
 * This file defines unity output macros for sending unity reports to Faraabin.
 * 
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef unity_config_H
#define unity_config_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_addon_unity.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define UNITY_OUTPUT_CHAR(a)    fFaraabinAddOn_Unity_Char(a)
#define UNITY_OUTPUT_FLUSH()    fFaraabinAddOn_Unity_Flush()
#define UNITY_OUTPUT_START()    fFaraabinAddOn_Unity_Start()
#define UNITY_OUTPUT_COMPLETE() fFaraabinAddOn_Unity_Complete()

#define UNITY_EXEC_TIME_START() fFaraabinAddOn_Unity_ExeTimeStart()
#define UNITY_EXEC_TIME_STOP()  fFaraabinAddOn_Unity_ExeTimeStop()
#define UNITY_PRINT_EXEC_TIME() fFaraabinAddOn_Unity_ExeTimePrint()

#define UNITY_OUTPUT_COLOR

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* UNITY_CONFIG_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
