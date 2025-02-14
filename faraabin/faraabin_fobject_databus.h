/**
 ******************************************************************************
 * @file           : faraabin_fobject_databus.h
 * @brief          : Faraabin databus fobject header file.
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
#ifndef FARAABIN_FOBJECT_DATABUS_H
#define FARAABIN_FOBJECT_DATABUS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

#include "chrono.h"

/* Exported defines ----------------------------------------------------------*/
/** @defgroup FARAABIN_DB_RET State machine return group
 *  @{
 */

#define FARAABIN_DB_OK                              (uint8_t)(0U) /*!< No error in databus. */
#define FARAABIN_DB_CHANNEL_INDEX_GREATER_THAN_MAX  (uint8_t)(1U) /*!< Databus channel index overflowed. */
#define FARAABIN_DB_IS_FULL                         (uint8_t)(2U) /*!< Databus channels are full. */
#define FARAABIN_DB_ACTION_WITH_NULL_REFERENCE      (uint8_t)(3U) /*!< Databus null reference error. */
#define FARAABIN_DB_OBJECT_NOT_FOUND                (uint8_t)(4U) /*!< Databus object can't be found. */
#define FARAABIN_DB_CODEBLOCK_CALLBACK_NOT_EMPTY    (uint8_t)(5U) /*!< Codeblock attached to databus has no callback. */
#define FARAABIN_DB_NOT_INIT                        (uint8_t)(5U) /*!< Databus not initialized error. */
#define FARAABIN_DB_QUEUE_EMPTY                     (uint8_t)(6U) /*!< Databus queue is empty. */

/** @} */ //End of FARAABIN_DB_RET

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief Databus fobject system events.
 * 
 */
typedef enum {
  
	eDATABUS_EVENT_INFO_USER_DATA_RECEIVED = 0,
	
  eDATABUS_EVENT_INFO_ATTACH,
  eDATABUS_EVENT_INFO_DETACH,
  
  eDATABUS_EVENT_INFO_STATE_CHANGE,
  
  eDATABUS_EVENT_INFO_CAPTURE_END,
  
  eDATABUS_EVENT_INFO_MANUAL_TRIG,
  
  eDATABUS_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
  eDATABUS_EVENT_ERROR_NOT_FREE_LOCATION_FOR_ATTACH,
  eDATABUS_EVENT_ERROR_NO_CAPTURE_DATA_FOR_SEND,
  eDATABUS_EVENT_ERROR_NO_FIND_FOBJECT,
  eDATABUS_EVENT_ERROR_CAPTURE_QUEUE,
  eDATABUS_EVENT_ERROR_CODE_BLOCK_NOT_EMPTY,
  eDATABUS_EVENT_ERROR_PARAM,
  eDATABUS_EVENT_ERROR_DETACH,
  eDATABUS_EVENT_ERROR_ATTACH,
  
}eFaraabinFobjectDataBus_SystemEventId;

/**
 * @brief Databus trigger source.
 * 
 */
typedef enum {
  
  eDATABUS_TRIG_SOURCE_NONE = 0,
  eDATABUS_TRIG_SOURCE_API,
	eDATABUS_TRIG_SOURCE_MANUAL,
  eDATABUS_TRIG_SOURCE_CHANNEL
  
}eFaraabinFobjectDataBus_TrigSource;

/**
 * @brief Databus states.
 * 
 */
typedef enum {
  
  eDATABUS_STATE_OFF = 0,
  eDATABUS_STATE_STREAM,
  eDATABUS_STATE_TIMER,
  eDATABUS_STATE_TRIG_WAIT,
  eDATABUS_STATE_TRIG_WINDOW,
  eDATABUS_STATE_CAPTURE_SEND,
  
}eFaraabinFobjectDataBus_State;

/**
 * @brief Databus trigger type.
 * 
 */
typedef enum {
  
  eDATABUS_CH_TRIG_CHANGE = 0,
  eDATABUS_CH_TRIG_RISING,
  eDATABUS_CH_TRIG_FALLING,
  
}eFaraabinFobjectDataBus_ChTrigType;

/**
 * @brief Databus channel object.
 * 
 */
typedef struct {
  
  uint8_t ItemFobjectType;      /*!< Type of the fobject attached to the databus channel. */
  
  uint32_t ItemFobjectPtr;      /*!< Pointer of the fobject attached to the databus channel. */
  
  uint16_t ItemFobjectParam;    /*!< Fobject's parameters attached to the databus channel. */
  
  uint8_t VariableDataType;     /*!< Type of the attached variable to the databus. */
  
  uint8_t PrimitiveVariableId;  /*!< ID of the primitive type variables. */
  
  bool Enable;                  /*!< Enable status of the channel. */
  
}sFaraabinFobjectDataBus_Channel;

/**
 * @brief Databus captured value object.
 * 
 */
typedef struct {
  
  uint8_t FobjectType;        /*!< Type of fobject. */
  
  uint32_t FobjectPtr;        /*!< Fobject pointer. */
  
  uint32_t CapturedTimeStamp; /*!< Captured value timestamp. */

  uint64_t CapturedValue;     /*!< Captured value. */
  
}sFaraabinFobjectDataBus_CaptureValue;

/**
 * @brief Databus fobject definition.
 * 
 */
typedef struct {

  uint8_t _type;                                                            /*!< Type identifier of the fobject. */
  
  bool _init;                                                               /*!< Init status of the fobject. */
	
	bool Enable;                                                              /*!< Enable status of the fobject. */
  
  const char *Name;                                                         /*!< Name string given to the fobject. */
  
  const char *Path;                                                         /*!< Path string given to the fobject. */
  
  const char *Filename;                                                     /*!< Filename of the fobject. */
  
  uint8_t Seq;                                                              /*!< Sequence counter. */
  
  uint16_t ChannelQty;                                                      /*!< Number of channels dedicated to the databus. */

  sFaraabinFobjectDataBus_Channel *_pBufferChannels;                        /*!< Pointer to the channels of the databus. */
  
  bool _isBufferChannelsStatic;                                             /*!< Memory allocation status of the channels. */
  
  uint16_t AttachedItemsQty;                                                /*!< Number of attached items to databus. */
  
  uint16_t AvailableItemsQty;                                               /*!< Number of avaialable items in databus. */
  
  uint16_t StreamDivideBy;                                                  /*!< Stream prescaler. Starts from 1. */
  
  uint16_t _streamDivbyCnt;                                                 /*!< Internal counter for stream prescaler. */
  
  sFaraabinFobjectDataBus_CaptureValue *_pBufferCapture;                    /*!< Pointer to the capture buffer. */

  bool _isBufferCaptureStatic;                                              /*!< Memory allocation status of the capture buffer. */

  uint32_t BufferCaptureSize;                                              /*!< Allocated size to the capture buffer. */
  
  uint32_t TimerWindowMs;                                                   /*!< Window time in millisecond for capturing data */
  
  uint32_t TimeAfterTrigMs;                                                 /*!< Time to capture data after databus is triggered. */
  
  sChrono _chronoTrigWindow;                                                /*!< Internal chrono for databus in trigger mode. */
  
  uint16_t TimerDivideBy;                                                   /*!< Databus timer prescaler. */
  
  uint16_t _timerDivbyCnt;                                                  /*!< Internal counter for databus timer prescaler. */
  
  uint16_t TrigDivideBy;                                                    /*!< Databus trigger prescaler. */
  
  uint16_t _trigDivbyCnt;                                                   /*!< Internal counter for databus trigger prescaler. */
  
  uint32_t CaptureSendingQty;                                               /*!< Number of captured items to be sent. */
  
  uint32_t CaptureSendingCnt;                                               /*!< Internal counter that holds track of the sent items from capture buffer. */
  
  uint8_t CaptureSendingReqSeq;                                             /*!< Capture sending request sequence. */
  
  uint32_t _trigTimeStamp;                                                  /*!< Timestamp of the triggered instance . */
  
  eFaraabinFobjectDataBus_State CurrentState;                               /*!< Current state of the databus state machine. */
  
  void(*fpUserTerminalCallback)(uint8_t *userData, uint16_t userDataSize);  /*!< User callback in the terminal for databus. */
  
  bool ApiTrigEnable;                                                       /*!< Is databus triggered by lib or application. */
  
  bool _isTriggered;                                                        /*!< Flag that indicates if databus is triggered or not. */
  
  eFaraabinFobjectDataBus_TrigSource LastTrigSource;                        /*!< Last source of databus trigger. */
  
  sChrono _chronoCycle;                                                     /*!< Internal chrono for running databus cycles. */
  
  uint32_t CycleUs;                                                         /*!< Cycle period of the databus in microsecond. */
  
  bool IsCaptureEnd;                                                        /*!< Flag that indicates capture is end. */
  
  bool ChTrigEnable;                                                        /*!< Flag that indicates the channel trigger is enabled. */
  
  uint16_t ChTrigNo;                                                        /*!< Channel source of databus trigger. */
  
  uint8_t ChTrigType;                                                       /*!< Type of trigger. */
  
  uByte8 ChTrigThreshold;                                                   /*!< Threshold set by user for the trigger channel. */
  
  uByte8 _trigChannelPayload;                                               /*!< Payload that contains captured data in triggered channel mode. */
  
  uint32_t QueueItemCount;                                                  /*!< Number of items in databus queue. */
                               
  uint32_t _queueFrontIndex;                                                /*!< Front index of the databus queue. */
                               
  uint32_t _queueRearIndex;                                                 /*!< Rear index of the databus queue. */
  
}sFaraabinFobjectDataBus;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes databus fobject.
 * 
 * @param me Pointer to the databus fobject.
 * @return result Could be one of values in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_Init(sFaraabinFobjectDataBus *me);

/**
 * @brief Deinitializes databus fobject and frees all allocated memory.
 * 
 * @param me Pointer to the databus fobject.
 * @return result Could be one of values in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AdvFeat_DeInit(sFaraabinFobjectDataBus *me);

/**
 * @brief Runs databus state machine.
 * 
 * @param me Pointer to the state machine fobject.
 */
void fFaraabinFobjectDataBus_Run(sFaraabinFobjectDataBus *me);

/**
 * @brief Sends captured data if databus is in eDATABUS_STATE_CAPTURE_SEND state.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_SendCaptureDataRun(sFaraabinFobjectDataBus *me);

/**
 * @brief Gets the number of attached items to the channels of the databus.
 * 
 * @param me Pointer to the databus fobject.
 * @return num Number of attached items.
 */
uint16_t fFaraabinFobjectDataBus_GetAttachCount(sFaraabinFobjectDataBus *me);

/**
 * @brief Resets the trigger level of the databus.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_ResetTrigger(sFaraabinFobjectDataBus *me);

/**
 * @brief Starts the databus in stream mode.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_StartStream(sFaraabinFobjectDataBus *me);

/**
 * @brief Starts the databus in timer mode.
 * 
 * @param me Pointer to the databus fobject.
 * @param timerPrescaler Timer mode prescaler. Starts from 1.
 * @param timerWindow_ms Time window for capturing data in timer mode.
 */
void fFaraabinFobjectDataBus_StartTimer(sFaraabinFobjectDataBus *me);

/**
 * @brief Starts the databus in trigger mode.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_StartTrigger(sFaraabinFobjectDataBus *me);

/**
 * @brief Forces a trig in the user application.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_ForceTrigger(sFaraabinFobjectDataBus *me);

/**
 * @brief Forces a trig manually in the user interface.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_ForceManualTrigger(sFaraabinFobjectDataBus *me);

/**
 * @brief Puts databus in stop state.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_Stop(sFaraabinFobjectDataBus *me);

/**
 * @brief Checks whether capture is end or not.
 * 
 * @param me Pointer to the databus fobject.
 * @return IsCaptureEnd Capture status.
 */
bool fFaraabinFobjectDataBus_IsCaptureEnd(sFaraabinFobjectDataBus *me);

/**
 * @brief Gets captured data quantity.
 * 
 * @param me Pointer to the databus fobject.
 * @return num Number of items in databus capture buffer.
 */
uint32_t fFaraabinFobjectDataBus_GetCaptureDataQty(sFaraabinFobjectDataBus *me);

/**
 * @brief Gets captured data.
 * 
 * @param me Pointer to the databus fobject.
 * @param index Index of the item in databus capture buffer.
 * @param value Pointer to the captured value.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_GetCaptureData(sFaraabinFobjectDataBus *me, uint32_t index, sFaraabinFobjectDataBus_CaptureValue *value);

/**
 * @brief Attaches a variable to a free channel in databus.
 * 
 * @param me Pointer to the databus fobject.
 * @param varPtr Pointer to the variable for attaching.
 * @param varSize Size of the variable.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AttachVariable(sFaraabinFobjectDataBus *me, uint32_t varPtr, uint16_t varSize, uint8_t varTypeArchitecture, uint8_t varPrimitiveId, uint8_t fobjectType);

/**
 * @brief Attaches a variable to a specific channel of the databus.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number of the databus.
 * @param varPtr Pointer to the variable.
 * @param varSize Size of the variable.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AttachVariableToChannel(sFaraabinFobjectDataBus *me, uint16_t channel, uint32_t varPtr, uint16_t varSize, uint8_t varTypeArchitecture, uint8_t varPrimitiveId, uint8_t fobjectType);

/**
 * @brief Attaches an entity item to a channel of the databus.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number of the databus.
 * @param varPtr Pointer to the variable containing the entity value.
 * @param varSize Size of the variable containing the entity value.
 * @param varTypeArchitecture Data type of the entity.
 * @param varPrimitiveId ID of the primitive data type.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AttachCodeBlock(sFaraabinFobjectDataBus *me, uint32_t cbPtr);

/**
 * @brief Attaches a code block fobject to a free channel of the databus so the code block can sends its data and events through it.
 * 
 * @param me Pointer to the databus fobject.
 * @param cbPtr Pointer to the code block.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AttachCodeBlockToChannel(sFaraabinFobjectDataBus *me, uint16_t channel, uint32_t cbPtr);

/**
 * @brief Attaches a code block fobject to a specific channel of the databus so the code block can sends its data and events through it.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number.
 * @param cbPtr Pointer to the code block.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_DetachFromChannel(sFaraabinFobjectDataBus *me, uint16_t channel);

/**
 * @brief Detaches an item from specific channel of the databus.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_DetachAllChannels(sFaraabinFobjectDataBus *me);

/**
 * @brief Detaches all attached items in a databus.
 * 
 * @param me Pointer to the databus fobject.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
void fFaraabinFobjectDataBus_RunCodeBlock(uint32_t cbPtr, uint32_t startTick, uint32_t endTick);

/**
 * @brief Attaches an entity item to a channel of the databus.
 * 
 * @note Entities represent internal objects within our Faraabin fobjects.
 *       For instance, in a code block, they might track the number of times it has run,
 *       execution time exceeding limits, or other relevant information.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number of the databus.
 * @param varPtr Pointer to the variable containing the entity value.
 * @param varSize Size of the variable containing the entity value.
 * @param varTypeArchitecture Data type of the entity.
 * @param varPrimitiveId ID of the primitive data type.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AttachEntityNumericalToChannel(sFaraabinFobjectDataBus *me, uint16_t channel, uint32_t varPtr, uint16_t varSize, uint8_t varTypeArchitecture, uint8_t varPrimitiveId);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_DATABUS_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
