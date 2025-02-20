/**
 ******************************************************************************
 * @file           : faraabin_fobject_var.h
 * @brief          : Variable fobject header file.
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
#ifndef FARAABIN_FOBJECT_VAR_H
#define FARAABIN_FOBJECT_VAR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief Access type to the variable from Faraabin commands.
 * 
 */
typedef enum {
  
  eVAR_ACCESS_TYPE_READ = 0,
  eVAR_ACCESS_TYPE_WRITE = 1
  
}eFaraabinVarAccessType;

/**
 * @brief Type of function pointer for user callbacks whenever a new access is occured to the variable via Faraabin commands.
 * 
 */
typedef uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size);

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_VAR_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
