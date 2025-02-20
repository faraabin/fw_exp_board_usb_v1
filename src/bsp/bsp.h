/**
 ******************************************************************************
 * @file           : bsp.h
 * @brief          :
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 FaraabinCo.
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
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef bsp_H
#define bsp_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported defines ----------------------------------------------------------*/
/**
 * @brief Bsp function return types.
 * 
 */
#define BSP_OK        ((uint8_t)0U) /*!< Indicates no error. */
#define BSP_NOT_INIT  ((uint8_t)1U) /*!< Indicates that BSP is not initialized. */

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
uint8_t fBsp_Init(void);

uint8_t fBsp_Led_On(void);
uint8_t fBsp_Led_Off(void);

void Error_Handler(void);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* BSP_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
