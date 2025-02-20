/**
 ******************************************************************************
 * @file           : faraabin_fobject_function.h
 * @brief          : Faraabin function fobject header file.
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
#ifndef FARAABIN_FOBJECT_FUNCTION_H
#define FARAABIN_FOBJECT_FUNCTION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

#include "chrono.h"

/* Exported defines ----------------------------------------------------------*/
/** @defgroup FUNCTION_FOBJECT_END_STATUS group.
 *  @{
 */

#define FUNCTION_CONTINUE         (255U)  /*!< Function is in the loop. */
#define FUNCTION_TERMINATE_OK     (200U)  /*!< Function executed and returned. */
#define FUNCTION_ERROR_PARAM_QTY  (201U)  /*!< Function error in the parameter quantity. */

/** @} */ //End of FUNCTION_FOBJECT_END_STATUS

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief Function status.
 * 
 */
typedef enum {
  
  eFN_STATUS_STOP = 0,
  eFN_STATUS_RUN,
  eFN_STATUS_PAUSE,
  
}eFaraabinFobjectFunction_StatusId;

/**
 * @brief Function fobject system events.
 * 
 */
typedef enum {
  
	eFN_EVENT_INFO_USER_DATA_RECEIVED = 0,
	
  eFN_EVENT_INFO_RUN,
  eFN_EVENT_INFO_STOP,
  eFN_EVENT_INFO_PAUSE,
  eFN_EVENT_INFO_RESUME,
  
  eFN_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
  eFN_EVENT_ERROR_CMD_IS_RUNNING_NOW,
  eFN_EVENT_ERROR_MAX_CONCURRENT_CMD_REACHED,
  eFN_EVENT_ERROR_CMD_NOT_FOUND,
  
}eFaraabinFobjectFunction_SystemEventId;

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
  
  const char *Name;                                         /*!< Name given to the fobject. */
  
  const char *Filename;                                     /*!< Filename of the fobject. */
  
  uint8_t Seq;                                              /*!< Sequence counter. */
  
  void(*fpSendMember)(uint32_t fobjectPtr, uint8_t reqSeq); /*!< Function pointer used for sending the functions in a function group. */
  
  const char *Help;                                         /*!< Help written by user to be shown in PC application for the function group. */

}sFaraabinFobjectFunctionGroupType;


/**
 * @brief Function fobject definitions.
 * 
 */
typedef struct {

  uint8_t _type;                                    /*!< Type of the fobject. */
  
  const char *Name;                                 /*!< Name given to the fobject. */
  
  const char *Help;                                 /*!< Help written by user to be shown in PC application for the function. */
  
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
