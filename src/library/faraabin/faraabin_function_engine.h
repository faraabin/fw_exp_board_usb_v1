/**
 ******************************************************************************
 * @file           : faraabin_function_engine.h
 * @brief          : Faraabin function engine.
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
#ifndef FARAABIN_FUNCTION_ENGINE_H
#define FARAABIN_FUNCTION_ENGINE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

#include "faraabin_fobject_function.h"

/* Exported defines ----------------------------------------------------------*/
/** @defgroup FUNCTION_ENGINE_RESULT group.
 *  @{
 */

#define FB_FENGINE_FUNCTION_CMD_OK                (0U)  /*!< Function engine CMD OK status. */
#define FB_FENGINE_FUNCTION_ALREADY_RUNNING       (1U)  /*!< Function engine CMD already running status. */
#define FB_FENGINE_MAX_CONCURRENT_LIMIT_REACHED   (2U)  /*!< Function engine maximum concurrent executions are running. */
#define FB_FENGINE_FUNCTION_NOT_IN_LIST           (3U)  /*!< Function engine function is not in the list. */

/** @} */ //End of FUNCTION_ENGINE_RESULT

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes the function engine that executes all functions and function groups in Faraabin.
 * 
 * @return InitResult Returns '0' if successful and '1' if failed.
 */
uint8_t fFaraabinFunctionEngine_Init(void);

/**
 * @brief Runs the function engine.
 * 
 */
void fFaraabinFunctionEngine_Run(void);

/**
 * @brief Starts a function for execution.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @param arg Pointer to the buffer containing the function arguments.
 * @param argSize Size of function arguments.
 * @return result A value form FUNCTION_ENGINE_RESULT group.
 */
uint8_t fFaraabinFunctionEngine_Start(uint32_t functionPtr, uint32_t objectPtr, uint8_t* arg, uint16_t argSize);

/**
 * @brief Stops a function fobject from execution.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @return result A value form FUNCTION_ENGINE_RESULT group.
 */
uint8_t fFaraabinFunctionEngine_Stop(uint32_t functionPtr, uint32_t objectPtr);

/**
 * @brief Pauses function fobject from execution.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @return result A value form FUNCTION_ENGINE_RESULT group.
 */
uint8_t fFaraabinFunctionEngine_Pause(uint32_t functionPtr, uint32_t objectPtr);

/**
 * @brief Resumes an already paused function.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @return result A value form FUNCTION_ENGINE_RESULT group.
 */
uint8_t fFaraabinFunctionEngine_Resume(uint32_t functionPtr, uint32_t objectPtr);

/**
 * @brief Gets the number of concurrent running function fobjects.
 * 
 * @return numberOfFuncs Number of running functions.
 */
uint16_t fFaraabinFunctionEngine_GetQtyOfRunningFunctions(void);

/**
 * @brief Gets the pointer of the function fobjects that are running by the engine.
 * 
 * @param index Index in function engine list.
 * @param ptr Pointer of the function fobject.
 * @param objectPtr Pointer of the object attached to the function group.
 * @return result If function is in the list returns '0', otherwise returns '1'.
 */
uint8_t fFaraabinFunctionEngine_GetPtrsOfRunningFunction(uint16_t index, uint32_t *ptr, uint32_t *objectPtr);

/**
 * @brief Returns RAM usage of the function feature in Faraabin.
 * 
 * @return usage Amount of bytes dedicated to Faraabin function feature. 
 */
uint32_t fFaraabinFunctionEngine_GetRamUsage(void);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FUNCTION_ENGINE_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
