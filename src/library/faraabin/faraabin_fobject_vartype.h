/**
 ******************************************************************************
 * @file           : faraabin_fobject_vartype.h
 * @brief          : Faraabin varType fobject header file.
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
 * For more information on how to use this fobject, refer to the explanation given
 * in faraabin_fobject_vartype.c file.
 * 
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_FOBJECT_VARTYPE_H
#define FARAABIN_FOBJECT_VARTYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup VAR_DATA_TYPE Data type of the variable fobject group.
 *  @{
 */

#define VAR_DATA_TYPE_PRIMITIVE           ((uint8_t)0x00U)
#define VAR_DATA_TYPE_USER_DEFINED_STRUCT ((uint8_t)0x01U)
#define VAR_DATA_TYPE_USER_DEFINED_ENUM   ((uint8_t)0x02U)
#define VAR_DATA_TYPE_USER_DEFINED_UNION  ((uint8_t)0x03U)

/** @} */ //End of VAR_DATA_TYPE

/** @defgroup VAR_DATA_TYPE_PRIMITIVE Type of the primitive variables group.
 *  @{
 */

#define VAR_DATA_TYPE_PRIMITIVE_NONE    ((uint8_t)0x00U)
#define VAR_DATA_TYPE_PRIMITIVE_BOOL    ((uint8_t)0x01U)
#define VAR_DATA_TYPE_PRIMITIVE_UINT8   ((uint8_t)0x02U)
#define VAR_DATA_TYPE_PRIMITIVE_INT8    ((uint8_t)0x03U)
#define VAR_DATA_TYPE_PRIMITIVE_UINT16  ((uint8_t)0x04U)
#define VAR_DATA_TYPE_PRIMITIVE_INT16   ((uint8_t)0x05U)
#define VAR_DATA_TYPE_PRIMITIVE_UINT32  ((uint8_t)0x06U)
#define VAR_DATA_TYPE_PRIMITIVE_INT32   ((uint8_t)0x07U)
#define VAR_DATA_TYPE_PRIMITIVE_UINT64  ((uint8_t)0x08U)
#define VAR_DATA_TYPE_PRIMITIVE_INT64   ((uint8_t)0x09U)
#define VAR_DATA_TYPE_PRIMITIVE_FLOAT32 ((uint8_t)0x0AU)
#define VAR_DATA_TYPE_PRIMITIVE_FLOAT64 ((uint8_t)0x0BU)

/** @} */ //End of VAR_DATA_TYPE_PRIMITIVE

/**
 * @brief Faraabin variable type fobject.
 * 
 */
typedef struct {
  
  uint8_t _type;                                            /*!< Type of the fobject. */
  
  bool _init;                                               /*!< Initialization status the fobject. */
  
  char *Name;                                               /*!< Name given to the fobject. */
  
  char *Path;                                               /*!< Path given to the fobject. */
  
  char *FileName;                                           /*!< FileName of the fobject. */
  
  uint8_t Seq;                                              /*!< Sequence counter. */
  
  bool Enable;                                              /*!< Enable status of the fobject. */

  uint8_t DataType;                                         /*!< Fobject data type. Could be one of VAR_DATA_TYPE values. */
  
  uint32_t Size;                                            /*!< Size of the fobject object. */
  
  void(*fpSendMember)(uint32_t fobjectPtr, uint8_t reqSeq); /*!< Function pointer for sending each member dictionary of the variable type. */
  
}sFaraabinFobjectVarType;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes the variable type fobject of Faraabin and adds the fobject
 *        to the dictionary.
 * 
 * @param me Pointer to the fobject.
 * @return InitStat Returns zero on success, 1 on failure.
 */
uint8_t fFaraabinFobjectVarType_Init(sFaraabinFobjectVarType *me);
  
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_VARTYPE_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
