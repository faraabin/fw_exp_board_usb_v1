/**
 ******************************************************************************
 * @file           : faraabin_default_fobjects.h
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
#ifndef FARAABIN_DEFAULT_FOBJECTS_H
#define FARAABIN_DEFAULT_FOBJECTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes default fobjects.
 * 
 * @note This function is called internally and user does not need to call it manually.
 * 
 * @return result Returns '0' if successful, otherwise '1'. 
 */
uint8_t fFaraabinDefaultFobjects_Init(void);

/**
 * @brief Runs default databus.
 * 
 */
void fFaraabinDefaultFobjects_Run(void);

/* Exported variables --------------------------------------------------------*/
#if defined(FB_FEATURE_FLAG_DEFAULT_DATABUS) && defined(FARAABIN_ENABLE)
FARAABIN_DATABUS_DEF_EXTERN_(DefaultDatabus);
#endif

#if defined(FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP) && defined(FARAABIN_ENABLE)
FARAABIN_EVENT_GROUP_DEF_EXTERN_(DefaultEventGroup);
#endif

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_DEFAULT_FOBJECTS_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
