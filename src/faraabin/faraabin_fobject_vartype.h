/**
 ******************************************************************************
 * @file           : faraabin_fobject_vartype.h
 * @brief          : Faraabin varType fobject header file.
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
/**
 * @brief Data type of the variable.
 * 
 */
typedef enum {
  
  eVAR_DATA_TYPE_PRIMITIVE = 0,
  eVAR_DATA_TYPE_USER_DEFINED_STRUCT,
  eVAR_DATA_TYPE_USER_DEFINED_ENUM,
  eVAR_DATA_TYPE_USER_DEFINED_UNION
  
}eFaraabinFobjectVarType_DataType;

/**
 * @brief Primitive variable types.
 * 
 */
typedef enum {
  
  eVAR_DATA_TYPE_PRIMITIVE_NONE    = 0,
  eVAR_DATA_TYPE_PRIMITIVE_BOOL    = 1,
  eVAR_DATA_TYPE_PRIMITIVE_UINT8   = 2,
  eVAR_DATA_TYPE_PRIMITIVE_INT8    = 3,
  eVAR_DATA_TYPE_PRIMITIVE_UINT16  = 4,
  eVAR_DATA_TYPE_PRIMITIVE_INT16   = 5,
  eVAR_DATA_TYPE_PRIMITIVE_UINT32  = 6,
  eVAR_DATA_TYPE_PRIMITIVE_INT32   = 7,
  eVAR_DATA_TYPE_PRIMITIVE_UINT64  = 8,
  eVAR_DATA_TYPE_PRIMITIVE_INT64   = 9,
  eVAR_DATA_TYPE_PRIMITIVE_FLOAT32 = 10,
  eVAR_DATA_TYPE_PRIMITIVE_FLOAT64 = 11
  
}eFaraabinFobjectVarType_PrimitiveId;

/**
 * @brief Faraabin variable type fobject.
 * 
 */
typedef struct {
  
  uint8_t _type;                                            /*!< Type of the fobject. */
  
  bool _init;                                               /*!< Initialization status the fobject. */
  
  const char *Name;                                         /*!< Name given to the fobject. */
  
  const char *Path;                                         /*!< Path given to the fobject. */
  
  const char *Filename;                                     /*!< Filename of the fobject. */
  
  uint8_t Seq;                                              /*!< Sequence counter. */
  
  bool Enable;                                              /*!< Enable status of the fobject. */

  eFaraabinFobjectVarType_DataType DataType;                /*!< Fobject data type. */
  
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
