/**
 ******************************************************************************
 * @file           : faraabin_internal.h
 * @brief          : Faraabin internal header file.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2012-2025 FaraabinCo.
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

#include "faraabin_dependency.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define FARAABIN_CRITICAL_ENTER_ fFaraabin_CriticalSectionEnter()
#define FARAABIN_CRITICAL_EXIT_  fFaraabin_CriticalSectionExit()

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief This function add fobject to faraabin.
 * 
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fDatabase_AddDict(uint32_t fobjectPtr);

/* Exported variables --------------------------------------------------------*/
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(bool_t);    /*!< Defining primitive varible type of bool. */
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

extern char* RootPath____;                          /*!< Default path "root" for fobjects defined without path. */

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_INTERNAL_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
