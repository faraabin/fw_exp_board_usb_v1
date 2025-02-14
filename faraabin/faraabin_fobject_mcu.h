/**
 ******************************************************************************
 * @file           : faraabin_fobject_mcu.h
 * @brief          : Faraabin MCU fobject header file.
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
#ifndef FARAABIN_FOBJECT_MCU_H
#define FARAABIN_FOBJECT_MCU_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

#include "chrono.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief MCU Fobject system event types definition.
 * 
 */
typedef enum {
  
	eMCU_EVENT_INFO_USER_DATA_RECEIVED = 0,
	
  eMCU_EVENT_INFO_DICT_END,
  eMCU_EVENT_INFO_CLEAR_FLAG_BUFFER_OVF,
  
  eMCU_EVENT_INFO_BOOT,
  
  eMCU_EVENT_ERROR_RX_FRAME_BIG_SIZE,
  eMCU_EVENT_ERROR_RX_FRAME_BEFORE_END_OF_PREVIOUS_FRAME,
  eMCU_EVENT_ERROR_RX_FRAME_DESCAPE,
  eMCU_EVENT_ERROR_RX_FRAME_CHECKSUM,
  eMCU_EVENT_ERROR_RX_FRAME_SMALL_SIZE,
  eMCU_EVENT_ERROR_TX_FRAME_TIMEOUT,
  eMCU_EVENT_ERROR_TX_FRAME_SEND,
  eMCU_EVENT_ERROR_MAX_PRINTF_REENTRANT,
  eMCU_EVENT_ERROR_UNDEF,
  
  eMCU_EVENT_ERROR_UNSUPPORTED_FOBJECT_TYPE_DICT,
  eMCU_EVENT_ERROR_DICT_INDEX_OVERFLOW,
  
  eMCU_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
  eMCU_EVENT_ERROR_UNSUPPORTED_FOBJECT_TYPE,
  
  eMCU_EVENT_ERROR_PASSWORD,
  
  eMCU_EVENT_ERROR_RESET_FUNC_NOT_IMPLEMENTED,
  
}eFaraabinFobjectMcu_SystemEventId;

/**
 * @brief 
 * 
 */
typedef struct {

  uint8_t _type;                                                            /*!< Type of the fobject. */
  
  bool _init;                                                               /*!< Init status of the fobject. */
	
	bool Enable;                                                              /*!< Enable status of the fobject. */
  
  const char *Name;                                                         /*!< Name given to the fobject. */
  
  const char *Path;                                                         /*!< Path given to the fobject. */
  
  const char *Filename;                                                     /*!< Filename of the fobject. */
  
  uint8_t Seq;                                                              /*!< Fobject sequence counter. */
  
  void(*fpUserTerminalCallback)(uint8_t *userData, uint16_t userDataSize);  /*!< User terminal callback function pointer. */
  
  uint32_t StatisticsRxFramesCnt;                                           /*!< Received frames count. */

  uint32_t StatisticsRxBytesCnt;                                            /*!< Received bytes count. */

  uint32_t StatisticsRxFramesEscapingErrorCnt;                              /*!< Received frames escaping error count. */

  uint32_t StatisticsRxFramesChecksumErrorCnt;                              /*!< Received frames checksum error count. */

  uint32_t StatisticsRxFramesOverrideErrorCnt;                              /*!< Received frames override error count. */

  uint32_t StatisticsRxFramesMinimumSizeErrorCnt;                           /*!< Received frames with minimum size error count. */

  uint32_t StatisticsTxFramesCnt;                                           /*!< Transmitted frames count. */

  uint32_t StatisticsTxBytesCnt;                                            /*!< Transmitted bytes count. */

  sChrono ChronoLiveTimeout;                                                /*!< Chrono for measuring live timeout. */
  
  bool _isHostConnected;                                                    /*!< Host connection status. */
	
	uint16_t SessionId;                                                    		/*!< Session id. */
	
	bool IsSendingCpuProfiler;                                                /*!< Flag for checking the enable status of sending CPU profiler data to Faraabin. */
	
	uint32_t BootTimeMs;                                                      /*!< Time since MCU boot in milliseconds. */
	
	bool BootTimeFirstFlag;                                                   /*!< Flag for starting boot time measurement once. */

}sFaraabinFobjectMcu;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes MCU Fobject.
 * 
 * @return result Returns '0' if success
 */
uint8_t fFaraabinFobjectMcu_Init(void);

/**
 * @brief Runs all statistics in Faraabin related to the MCU Fobject.
 * 
 */
void fFaraabinFobjectMcu_Run(void);

/**
 * @brief Sends system event to Faraabin.
 * 
 * @param eventId ID of the event.
 */
sFaraabinFobjectMcu* fFaraabinFobjectMcu_GetFobject(void);

/**
 * @brief Sends system exception event to Faraabin.
 * 
 * @param eventId ID of the event.
 */
void fFaraabinFobjectMcu_SendEventSystem(uint16_t eventId);

/**
 * @brief Sends system event response.
 * 
 * @param eventId ID of the event.
 * @param reqSeq Request sequence.
 */
void fFaraabinFobjectMcu_SendEventSystemException(uint16_t eventId);

/**
 * @brief Sends system exception event response.
 * 
 * @param eventId 
 * @param reqSeq 
 */
void fFaraabinFobjectMcu_SendEventSystemResponse(uint16_t eventId, uint8_t reqSeq);

/**
 * @brief Gets the pointer to the MCU Fobject.
 * 
 * @return ptr Pointer to the MCU fobject.
 */
void fFaraabinFobjectMcu_SendEventSystemExceptionResponse(uint16_t eventId, uint8_t reqSeq);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_MCU_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
