/**
 ******************************************************************************
 * @file           : faraabin_default_fobjects_wrapper.h
 * @brief          :
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
#ifndef FARAABIN_DEFAULT_FOBJECTS_WRAPPER_H
#define FARAABIN_DEFAULT_FOBJECTS_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_default_fobjects.h"
	
#include "faraabin_config.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifdef FARAABIN_ENABLE

#ifdef FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP

/**
 * @brief Prints a formatted string and sends it to the default event group.
 * 
 * @note This macro works just like a printf() function.
 * 
 */
#define FARAABIN_Printf_(...) \
FARAABIN_PrintfTo_(&DefaultEventGroup, __VA_ARGS__)

/**
 * @brief Prints a formatted string and sends it to the default event group with WARNING severity.
 * 
 * @note This macro works just like a printf() function.
 * 
 */
#define FARAABIN_PrintfWarning_(...) \
FARAABIN_PrintfWarningTo_(&DefaultEventGroup, __VA_ARGS__)

/**
 * @brief Prints a formatted string and sends it to the default event group with ERROR severity.
 * 
 * @note This macro works just like a printf() function.
 * 
 */
#define FARAABIN_PrintfError_(...) \
FARAABIN_PrintfErrorTo_(&DefaultEventGroup, __VA_ARGS__)

/**
 * @brief Generates string type event and sends it to the default event group.
 * 
 * @note This macro works just like fputs() function for printing an stream of string.
 * 
 */
#define FARAABIN_Puts_(...) \
FARAABIN_PutsTo_(&DefaultEventGroup, __VA_ARGS__)

/**
 * @brief Generates string type event with severity of WARNING and sends it to the default event group.
 * 
 * @note This macro works just like fputs() function for printing an stream of string.
 * 
 */
#define FARAABIN_PutsWarning_(...) \
FARAABIN_PutsWarningTo_(&DefaultEventGroup, __VA_ARGS__)

/**
 * @brief Generates string type event with severity of ERROR and sends it to the default event group.
 * 
 * @note This macro works just like fputs() function for printing an stream of string.
 * 
 */
#define FARAABIN_PutsError_(...) \
FARAABIN_PutsErrorTo_(&DefaultEventGroup, __VA_ARGS__)

/**
 * @brief Sends an enumerated event to the default event group.
 * 
 * @param enumName_ Name of the enumeration that has been already defined to Faraabin using varType.
 * @param eventId_ ID of the event.
 */
#define FARAABIN_SendEvent_(enumName_, eventId_) \
FARAABIN_SendEventTo_(&DefaultEventGroup, enumName_, eventId_)

/**
 * @brief Sends an enumerated event to the default event group with WARNING severity.
 * 
 * @param enumName_ Name of the enumeration that has been already defined to Faraabin using varType.
 * @param eventId_ ID of the event.
 */
#define FARAABIN_SendEventWarning_(enumName_, eventId_) \
FARAABIN_SendEventWarningTo_(&DefaultEventGroup, enumName_, eventId_)

/**
 * @brief Sends an enumerated event to the default event group with ERROR severity.
 * 
 * @param enumName_ Name of the enumeration that has been already defined to Faraabin using varType.
 * @param eventId_ ID of the event.
 */
#define FARAABIN_SendEventError_(enumName_, eventId_) \
FARAABIN_SendEventErrorTo_(&DefaultEventGroup, enumName_, eventId_)
#endif

#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_DEFAULT_FOBJECTS_WRAPPER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
