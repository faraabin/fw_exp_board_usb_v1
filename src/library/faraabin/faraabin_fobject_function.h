/**
 ******************************************************************************
 * @file           : faraabin_fobject_function.h
 * @brief          : Faraabin function fobject header file.
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
#ifndef FARAABIN_FOBJECT_FUNCTION_H
#define FARAABIN_FOBJECT_FUNCTION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_dependency.h"

#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
/** @defgroup FUNCTION_FOBJECT_END_STATUS group.
 *  @{
 */

#define FUNCTION_CONTINUE         ((uint8_t)255U)  /*!< Function is in the loop. */
#define FUNCTION_TERMINATE_OK     ((uint8_t)200U)  /*!< Function executed and returned. */
#define FUNCTION_ERROR_PARAM_QTY  ((uint8_t)201U)  /*!< Function error in the parameter quantity. */

/** @} */ //End of FUNCTION_FOBJECT_END_STATUS

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/** @defgroup FN_STATUS Function status group.
 *  @{
 */

#define FN_STATUS_STOP  ((uint8_t)0x00U)
#define FN_STATUS_RUN   ((uint8_t)0x01U)
#define FN_STATUS_PAUSE ((uint8_t)0x02U)

/** @} */ //End of FN_STATUS

/** @defgroup FN_EVENT Function fobject system events group.
 *  @{
 */

#define FN_EVENT_INFO_USER_DATA_RECEIVED            ((uint8_t)0x00U)

#define FN_EVENT_INFO_RUN                           ((uint8_t)0x01U)
#define FN_EVENT_INFO_STOP                          ((uint8_t)0x02U)
#define FN_EVENT_INFO_PAUSE                         ((uint8_t)0x03U)
#define FN_EVENT_INFO_RESUME                        ((uint8_t)0x04U)

#define FN_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY ((uint8_t)0x05U)
#define FN_EVENT_ERROR_CMD_IS_RUNNING_NOW           ((uint8_t)0x06U)
#define FN_EVENT_ERROR_MAX_CONCURRENT_CMD_REACHED   ((uint8_t)0x07U)
#define FN_EVENT_ERROR_CMD_NOT_FOUND                ((uint8_t)0x08U)

/** @} */ //End of eFN_EVENT

/**
 * @brief Faraabin function fobject handler function pointer type.
 * 
 */
typedef uint8_t(*FaraabinFobjectFunctionHandler)(uint32_t objectPtr, char *param, bool isFirstRun);

/**
 * @brief Function group fobject definitions.
 * 
 */
typedef struct {

  uint8_t _type;                                            /*!< Type of the fobject. */
  
  bool _init;                                               /*!< Init status of the fobject. */
  
  char *Name;                                               /*!< Name given to the fobject. */

  char *Path;
  
  char *FileName;                                           /*!< FileName of the fobject. */
  
  uint8_t Seq;                                              /*!< Sequence counter. */
  
  void(*fpSendMember)(uint32_t fobjectPtr, uint8_t reqSeq); /*!< Function pointer used for sending the functions in a function group. */
  
  char *Help;                                               /*!< Help written by user to be shown in PC application for the function group. */

}sFaraabinFobjectFunctionGroupType;

/**
 * @brief Function fobject definitions.
 * 
 */
typedef struct {

  uint8_t _type;                                    /*!< Type of the fobject. */
  
  char *Name;                                       /*!< Name given to the fobject. */
  
  char *Help;                                       /*!< Help written by user to be shown in PC application for the function. */
  
  uint8_t Seq;                                      /*!< Sequence counter. */
  
  sFaraabinFobjectFunctionGroupType* GroupTypePtr;  /*!< Pointer to the functin group that owns this function fobject. */
  
  FaraabinFobjectFunctionHandler FunctionPtr;       /*!< Pointer to the function handler of the fobject. */
  
  sChrono _chrono;                                  /*!< Private chrono used for time measurements. */

}sFaraabinFobjectFunction;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes a function group and adds its dictionary to the database.
 * 
 * @param me Pointer to the function group.
 * @return InitStat Returns '0' if successful, '1' if if fails. 
 */
uint8_t fFaraabinFobjectFunctionGroupType_Init(sFaraabinFobjectFunctionGroupType *me);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_FUNCTION_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
