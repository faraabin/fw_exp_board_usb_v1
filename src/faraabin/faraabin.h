/**
 ******************************************************************************
 * @file           : faraabin.h
 * @brief          : faraabin header file.
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
 
   For more information on how to use this module, please refer to the implementation
   of the module in faraabin.c file.
 
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_H
#define FARAABIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

#include "faraabin_defines.h"
#include "faraabin_internal.h"
#include "faraabin_database.h"

#include "faraabin_function_engine.h"

#include "faraabin_fobject_vartype_wrapper.h"
#include "faraabin_fobject_var_wrapper.h"
#include "faraabin_fobject_container_wrapper.h"
#include "faraabin_fobject_eventgroup_wrapper.h"
#include "faraabin_fobject_system_event_wrapper.h"
#include "faraabin_fobject_function_wrapper.h"
#include "faraabin_fobject_databus_wrapper.h"
#include "faraabin_default_fobjects_wrapper.h"

#include "add_on\runtime_scaler\runtime_scaler.h"

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
#include "cpu_profiler_wrapper.h"
#endif

#include "faraabin_fobject_mcu.h"
#include "faraabin_link_handler.h"

/* Exported defines ----------------------------------------------------------*/
/** @defgroup FARAABIN_RET Faraabin return group
 *  @{
 */
 
typedef uint8_t fb_result_t;

#define FARAABIN_OK                           (0U)  /*!< No error. */
#define FARAABIN_SERIALIZER_INIT_FAILED       (1U)  /*!< Initializing Faraabin serializer has been failed. */
#define FARAABIN_LINK_HANDLER_INIT_FAILED     (2U)  /*!< Initializing Faraabin link handler has been failed. */
#define FARAABIN_DATA_BASE_INIT_FAILED        (3U)  /*!< Initializing Faraabin database has been failed. */
#define FARAABIN_DEFAULT_FOBJECTS_INIT_FAILED (4U)  /*!< Initializing Faraabin default Fobjects have been failed. */
#define FARAABIN_MCU_FOBJECT_INIT_FAILED      (5U)  /*!< Initializing Faraabin MCU Fobject has been failed. */
#define FARAABIN_FUNCTION_ENGINE_INIT_FAILED  (6U)  /*!< Initializing Faraabin Function Engine has been failed. */
#define FARAABIN_CPU_PROFILER_INIT_FAILED  		(7U)  /*!< Initializing cpu profiler has been failed. */

/** @} */ //End of FARAABIN_RET

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief This function initializes faraabin.
 * 
 * @return result Can be one of FARAABIN_RET values.
 */
fb_result_t fFaraabin_Init(void);

/**
 * @brief This function enable faraabin.
 */
void fFaraabin_Enable(void);

/**
 * @brief This function disable faraabin.
 */
void fFaraabin_Disable(void);

/**
 * @brief This function executes faraabin main modules. This way probe data are prepared and
 *        can be sent via faraabin link to the PC.
 * 
 */
void fFaraabin_Run(void);

/**
 * @brief This function must be called after receving a byte from the link dedicated to faraabin.
 * 
 * @param c Received character.
 */
void fFaraabin_CharReceived(uint8_t c);

/**
 * @brief Sets a new password for the faraabin.
 * 
 * @param password Password for starting the connection procedure in faraabin application.
 */
void fFaraabin_PasswordSet(char *password);

/**
 * @brief Enables request of the password in faraabin application.
 * 
 */
void fFaraabin_PasswordEnable(void);

/**
 * @brief Disables request of the password in faraabin application.
 * 
 */
void fFaraabin_PasswordDisable(void);

/**
 * @brief This function checks if the host (PC application) is connected to the MCU or not.
 * 
 * @retval IsHostConnected Connection status.
 */
bool fFaraabin_IsHostConnected(void);

bool fFaraabin_IsAllowEvent(void);

void fFaraabin_FlushBuffer(void);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
