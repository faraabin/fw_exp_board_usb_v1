/**
 ******************************************************************************
 * @file           : faraabin_internal.h
 * @brief          : Faraabin internal header file.
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
 * This header file is for internal use.
 * 
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_INTERNAL_H
#define FARAABIN_INTERNAL_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

#include "faraabin_fobject_vartype_wrapper.h"
#include "faraabin_defines.h"

#include "faraabin_config.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define FARAABIN_CRITICIAL_ENTER_ FB_PORT_DISABLE_IRQ
#define FARAABIN_CRITICIAL_EXIT_  FB_PORT_ENABLE_IRQ

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(bool_t);      /*!< Defining primitive varible type of bool. */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(uint8_t);   /*!< Defining primitive varible type of uint8_t. */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(int8_t);    /*!< Defining primitive varible type of int8_t. */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(uint16_t);  /*!< Defining primitive varible type of uint16_t. */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(int16_t);   /*!< Defining primitive varible type of int16_t. */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(uint32_t);  /*!< Defining primitive varible type of uint32_t. */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(int32_t);   /*!< Defining primitive varible type of int32_t. */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(uint64_t);  /*!< Defining primitive varible type of uint64_t. */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(int64_t);   /*!< Defining primitive varible type of int64_t. */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(float32_t); /*!< Defining primitive varible type of float32_t. */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(float64_t); /*!< Defining primitive varible type of float64_t. */

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_INTERNAL_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
