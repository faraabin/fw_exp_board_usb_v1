/**
 ******************************************************************************
 * @file           : faraabin.h
 * @brief          : faraabin header file.
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
#include "faraabin_dependency.h"

#ifdef FARAABIN_ENABLE
#include "faraabin_internal.h"
#include "faraabin_serializer.h"
#include "faraabin_fobject.h"

#include "faraabin_fobject_var_wrapper.h"
#include "faraabin_fobject_vartype_wrapper.h"
#include "faraabin_fobject_eventgroup_wrapper.h"
#include "faraabin_fobject_container_wrapper.h"
#include "faraabin_fobject_function_wrapper.h"
#include "faraabin_fobject_databus_wrapper.h"

#include "faraabin_default_fobjects_wrapper.h"
#include "faraabin_fobject_system_event_wrapper.h"

/* Exported defines ----------------------------------------------------------*/
/** @defgroup FARAABIN_RET Faraabin return group
 *  @{
 */
 
typedef uint8_t fb_result_t;

#define FARAABIN_OK                                         ((uint8_t)0U)   /*!< No error. */
#define FARAABIN_SERIALIZER_INIT_FAILED                     ((uint8_t)1U)   /*!< Initializing Faraabin serializer has been failed. */
#define FARAABIN_LINK_HANDLER_INIT_FAILED                   ((uint8_t)2U)   /*!< Initializing Faraabin link handler has been failed. */
#define FARAABIN_DATA_BASE_INIT_FAILED                      ((uint8_t)3U)   /*!< Initializing Faraabin database has been failed. */
#define FARAABIN_DEFAULT_FOBJECTS_INIT_FAILED               ((uint8_t)4U)   /*!< Initializing Faraabin default Fobjects have been failed. */
#define FARAABIN_MCU_FOBJECT_INIT_FAILED                    ((uint8_t)5U)   /*!< Initializing Faraabin MCU Fobject has been failed. */
#define FARAABIN_FUNCTION_ENGINE_INIT_FAILED                ((uint8_t)6U)   /*!< Initializing Faraabin Function Engine has been failed. */
#define FARAABIN_CPU_PROFILER_INIT_FAILED  		              ((uint8_t)7U)   /*!< Initializing cpu profiler has been failed. */
#define FARAABIN_UNITY_INIT_FAILED  		                    ((uint8_t)8U)   /*!< Initializing unity has been failed. */

#define FARAABIN_NOT_INITIALIZED_ERROR                      ((uint8_t)9U)   /*!< Faraabin is not initialized and cannot work on fobjects. */
#define FARAABIN_CANNOT_ADD_DICT_ERROR                      ((uint8_t)10U)  /*!< Faraabin cannot add fobject to the database. */

#define FARAABIN_LINK_ALLOCATED_RX_BUFFER_SIZE_ERROR        ((uint8_t)11U)  /*!< Faraabin link handler has RX buffer size smaller than the minimum required. */
#define FARAABIN_LINK_RX_BUFFER_NULL_ERROR                  ((uint8_t)12U)  /*!< Faraabin link handler RX buffer is null. */
#define FARAABIN_LINK_TX_BUFFER_NULL_ERROR                  ((uint8_t)13U)  /*!< Faraabin link data buffer is null. */
#define FARAABIN_LINK_TX_BUFFER_SIZE_ERROR                  ((uint8_t)14U)  /*!< Faraabin link data buffer has wrong size. */

#define FARAABIN_SERIALIZER_DATA_BUFFER_NOT_INITIALIZED     ((uint8_t)15U)  /*!< Faraabin link serializer could not initialize link data buffer. */
#define FARAABIN_SERIALIZER_CMD_BUFFER_NOT_INITIALIZED      ((uint8_t)16U)  /*!< Faraabin link serializer could not initialize link command buffer. */

#define FARAABIN_DESERIALIZE_MINIMUM_FRAME_SIZE_ERROR       ((uint8_t)17U)  /*!< Faraabin deserializer detected a frame smaller with size smaller than minimum frame size. */
#define FARAABIN_DESERIALIZE_CHECKSUM_ERROR                 ((uint8_t)19U)  /*!< Faraabin deserializer detected a checksum error in the frame. */
#define FARAABIN_DESERIALIZE_MEMORY_ERROR                   ((uint8_t)20U)  /*!< Clearing memory error. */

#define FARAABIN_DICT_OBJECT_NULL_REFERENCE_ERROR           ((uint8_t)21U)  /*!< Faraabin dictionary object is null. */
#define FARAABIN_DICT_ALREADY_EXIST_ERROR                   ((uint8_t)22U)  /*!< Faraabin dictionary already added. */
#define FARAABIN_DICT_IS_SENDING_ERROR                      ((uint8_t)23U)  /*!< Faraabin is sending dictionary to the app and cannot add new one to database. */
#define FARAABIN_DICT_DATABASE_IS_FULL_ERROR                ((uint8_t)24U)  /*!< Faraabin database is full and cannot add dictionary anymore. */

#define FARAABIN_DATABUS_NOT_INITIALIZED_ERROR              ((uint8_t)25U)  /*!< Databus not initialized error. */
#define FARAABIN_DATABUS_CHANNEL_INDEX_ERROR                ((uint8_t)26U)  /*!< Databus channel index overflowed. */
#define FARAABIN_DATABUS_IS_FULL_ERROR                      ((uint8_t)27U)  /*!< Databus channels are full. */
#define FARAABIN_DATABUS_ACTION_WITH_NULL_REFERENCE_ERROR   ((uint8_t)28U)  /*!< Databus null reference error. */
#define FARAABIN_DATABUS_OBJECT_NOT_FOUND_ERROR             ((uint8_t)29U)  /*!< Databus object can't be found. */
#define FARAABIN_DATABUS_CODEBLOCK_CALLBACK_NOT_EMPTY_ERROR ((uint8_t)30U)  /*!< Codeblock attached to databus has no callback. */
#define FARAABIN_DATABUS_QUEUE_EMPTY_ERROR                  ((uint8_t)31U)  /*!< Databus queue is empty. */
#define FARAABIN_DATABUS_CHANNEL_SIZE_ERROR                 ((uint8_t)32U)  /*!< Indicates that the number of channels for databus is wrong. */
#define FARAABIN_DATABUS_BUFFER_NULL_ERROR                  ((uint8_t)33U)  /*!< Indicates that databus buffer is null. */

#define FARAABIN_FENGINE_MEMSET_ERROR                       ((uint8_t)34U)  /*!< Function engine error occured during resetting the parameters in initialization. */
#define FARAABIN_FENGINE_FUNCTION_ALREADY_RUNNING_ERROR     ((uint8_t)35U)  /*!< Function engine CMD already running status. */
#define FARAABIN_FENGINE_MAX_CONCURRENT_LIMIT_REACHED_ERROR ((uint8_t)36U)  /*!< Function engine maximum concurrent executions are running. */
#define FARAABIN_FENGINE_FUNCTION_INDEX_ERROR               ((uint8_t)37U)  /*!< Function engine function index error. */
#define FARAABIN_FENGINE_FUNCTION_NOT_IN_LIST_ERROR         ((uint8_t)38U)  /*!< Function engine function is not in the list. */

#define FARAABIN_CMD_LINK_NULL_ERROR                        ((uint8_t)39U)  /*!< Command link of the faraabin is null. */

#define FARAABIN_PORT_INIT_ERROR                            ((uint8_t)40U)  /*!< ---. */

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
void fFaraabin_HandleReceivedBytes(uint8_t *pData, uint16_t size);

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

/**
 * @brief This function checks if the event is allow to send.
 * 
 * @retval IsAllowEvent status.
 */
bool fFaraabin_IsAllowEvent(void);

/**
 * @brief This function flush all data exist in buffer to link.
 * 
 */
void fFaraabin_FlushBuffer(void);

/**
 * @brief This function reset receive buffer.
 * 
 */
void fFaraabin_ResetReceiveBuffer(void);

/**
 * @brief This function is called after sending dictionary compalte.
 *        This function iv define weak in faraabin.c
 *        User should define this function to use it.
 * 
 */
void fFaraabin_DictSendingCpltCallback(void);

/* Exported variables --------------------------------------------------------*/

#endif

#ifdef __cplusplus
}
#endif

#endif // FARAABIN_H

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
