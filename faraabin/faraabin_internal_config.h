/**
 ******************************************************************************
 * @file           : faraabin_internal_config.h
 * @brief          : Faraabin internal configurations.
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
 * This file is used internally.
 * 
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_INTERNAL_CONFIG_H
#define FARAABIN_INTERNAL_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
extern const char* RootPath____;  /*!< Default path "root" for fobjects defined without path. */
extern bool FaraabinInit___;

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_INTERNAL_CONFIG_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
