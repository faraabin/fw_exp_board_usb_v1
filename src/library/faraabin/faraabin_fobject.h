/**
 ******************************************************************************
 * @file           : faraabin_fobject.h
 * @brief          : Faraabin fobject types header file.
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
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_FOBJECT_H
#define FARAABIN_FOBJECT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
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
#define FO_TYPE_MCU                         ((uint8_t)0U)
#define FO_TYPE_GENERAL                     ((uint8_t)1U)
#define FO_TYPE_VAR                         ((uint8_t)2U)
#define FO_TYPE_VAR_TYPE                    ((uint8_t)3U)
#define FO_TYPE_EVENT_GROUP                 ((uint8_t)4U)
#define FO_TYPE_FUNCTION                    ((uint8_t)5U)
#define FO_TYPE_FUNCTION_GROUP              ((uint8_t)6U)
#define FO_TYPE_FUNCTION_GROUP_TYPE         ((uint8_t)7U)
#define FO_TYPE_DATABUS                     ((uint8_t)8U)
#define FO_TYPE_PROCESS                     ((uint8_t)9U)
#define FO_TYPE_CODE_BLOCK                  ((uint8_t)10U)
#define FO_TYPE_STATE_MACHINE               ((uint8_t)11U)
#define FO_TYPE_CONTAINER                   ((uint8_t)12U)
#define FO_TYPE_FOBJECT_END                 ((uint8_t)13U)

#define FO_TYPE_FOBJECT                     ((uint8_t)14U)
#define FO_TYPE_STATE_MACHINE_STATE         ((uint8_t)15U)
#define FO_TYPE_STATE_MACHINE_TRANSITION    ((uint8_t)16U)
#define FO_TYPE_VAR_TYPE_ENUM_MEMBER        ((uint8_t)17U)
#define FO_TYPE_VAR_TYPE_STRUCT_MEMBER      ((uint8_t)18U)
#define FO_TYPE_ENTITY_NUMERICAL            ((uint8_t)19U)
#define FO_TYPE_ENTITY_EVENT                ((uint8_t)20U)
#define FO_TYPE_FUNCTION_GROUP_TYPE_MEMBER  ((uint8_t)21U)

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
