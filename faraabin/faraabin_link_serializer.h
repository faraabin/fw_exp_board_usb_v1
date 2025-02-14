/**
 ******************************************************************************
 * @file           : faraabin_link_serializer.h
 * @brief          :
 * @note           :
 * @copyright      : COPYRIGHT© 2024 FaraabinCo
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
#ifndef FARAABIN_LINK_SERIALIZER_H
#define FARAABIN_LINK_SERIALIZER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#define ClientFrame_GetPropGroup_(byte_)  (uint8_t)(((byte_) >> 5U) & 0x07U)
#define ClientFrame_GetPropId_(byte_)     (uint8_t)((byte_) & 0x1FU)

/* Exported types ------------------------------------------------------------*/

/** @defgroup SERIALIZER_PROPERY_GROUP typedefs
 *  @{
 */
typedef enum {

  eFB_PROP_GROUP_DICT,
  eFB_PROP_GROUP_SETTING,
  eFB_PROP_GROUP_MONITORING,
  eFB_PROP_GROUP_COMMAND,
  eFB_PROP_GROUP_EVENT

}eFaraabinLinkSerializer_PropertyGroup;

/** @} */ //End of SERIALIZER_PROPERY_GROUP

/** @defgroup SERIALIZER_COMMON_PROPERTY typedefs
 *  @{
 */
typedef enum {

  eFB_COMMON_PROP_ID_SETTING_ENABLE,
  eFB_COMMON_PROP_ID_SETTING_ALL

}eFaraabinLinkSerializer_CommonPropertyIdSetting;

typedef enum {

  eFB_COMMON_PROP_ID_MONITORING_RESERVE

}eFaraabinLinkSerializer_CommonPropertyIdMonitoring;

typedef enum {

  eFB_COMMON_PROP_ID_COMMAND_RESERVE

}eFaraabinLinkSerializer_CommonPropertyIdCommand;

typedef enum {

  eFB_COMMON_PROP_ID_EVENT_SW,
  eFB_COMMON_PROP_ID_EVENT_SW_EXCEPTION,
  eFB_COMMON_PROP_ID_EVENT_LIB,
  eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION,
  eFB_COMMON_PROP_ID_EVENT_USER_DATA,
  eFB_COMMON_PROP_ID_EVENT_USER_CODE,
  eFB_COMMON_PROP_ID_EVENT_USER_TERMINAL

}eFaraabinLinkSerializer_CommonPropertyIdEvent;

/** @} */ //End of SERIALIZER_COMMON_PROPERTY

/** @defgroup SERIALIZER_EVENT_SEVERITY typedefs
 *  @{
 */
typedef enum {
  
  eFO_EVENT_SEVERITY_INFO = 0,
  eFO_EVENT_SEVERITY_WARNING = 1,
  eFO_EVENT_SEVERITY_ERROR = 2
  
}eFaraabinLinkSerializer_EventSeverity;

/** @} */ //End of SERIALIZER_EVENT_SEVERITY

/** @defgroup SERIALIZER_MCU_PEROPERTY typedefs
 *  @{
 */
typedef enum {

  eFB_MCU_PROP_ID_SETTING_ENABLE = eFB_COMMON_PROP_ID_SETTING_ENABLE,
  eFB_MCU_PROP_ID_SETTING_ALL = eFB_COMMON_PROP_ID_SETTING_ALL,
	
	eFB_MCU_PROP_ID_SETTING_SEND_PROFILER_ENABLE

}eFaraabinLinkSerializer_McuPropertyIdSetting;

typedef enum {

  eFB_MCU_PROP_ID_MONITORING_VARIABLE,
  eFB_MCU_PROP_ID_MONITORING_LIVE,
  eFB_MCU_PROP_ID_MONITORING_PING,
  eFB_MCU_PROP_ID_MONITORING_WHOAMI,
	eFB_MCU_PROP_ID_MONITORING_PROFILER

}eFaraabinLinkSerializer_McuPropertyIdMonitoring;

typedef enum {

  eFB_MCU_PROP_ID_COMMAND_PING,
  eFB_MCU_PROP_ID_COMMAND_SEND_LIVE,
  eFB_MCU_PROP_ID_COMMAND_SEND_WHOAMI,
  eFB_MCU_PROP_ID_COMMAND_SEND_ALL_DICT,
  eFB_MCU_PROP_ID_COMMAND_RESET_CPU,
  eFB_MCU_PROP_ID_COMMAND_CLEAR_FLAG_BUFFER_OVF

}eFaraabinLinkSerializer_McuProperyIdCommand;

/** @} */ //End of SERIALIZER_MCU_PEROPERTY

/** @defgroup SERIALIZER_DATABUS_PEROPERTY typedefs
 *  @{
 */

typedef enum {
  
  eFB_DB_PROP_ID_SETTING_ENABLE = eFB_COMMON_PROP_ID_SETTING_ENABLE,
  eFB_DB_PROP_ID_SETTING_ALL = eFB_COMMON_PROP_ID_SETTING_ALL,
  eFB_DB_PROP_ID_SETTING_ITEM_ENABLE,
  eFO_DB_PROP_ID_SETTING_API_TRIG_ENABLE,
  eFO_DB_PROP_ID_SETTING_CH_TRIG_ENABLE,
  eFO_DB_PROP_ID_SETTING_CH_TRIG_CONFIG,
	eFO_DB_PROP_ID_SETTING_STREAM_DIVIDEBY,
	eFO_DB_PROP_ID_SETTING_TIMER_DIVIDEBY,
	eFO_DB_PROP_ID_SETTING_TRIG_DIVIDEBY,

}eFaraabinLinkSerializer_DataBusPropertyIdSetting;

typedef enum {

  eFB_DB_PROP_ID_MONITORING_CAPTURE_VALUE,
  eFB_DB_PROP_ID_MONITORING_STREAM_VALUE

}eFaraabinLinkSerializer_DataBusPropertyIdMonitoring;

typedef enum {

  eFB_DB_PROP_ID_COMMAND_ATTACH,
  eFB_DB_PROP_ID_COMMAND_DETACH,
  eFB_DB_PROP_ID_COMMAND_DETACH_ALL,
  eFB_DB_PROP_ID_COMMAND_START_STREAM,
  eFB_DB_PROP_ID_COMMAND_START_TRIG,
  eFB_DB_PROP_ID_COMMAND_START_TIMER,
  eFB_DB_PROP_ID_COMMAND_STOP,
  eFB_DB_PROP_ID_COMMAND_CAPTURE_SEND,
  eFB_DB_PROP_ID_COMMAND_MANUAL_TRIG,
  eFB_DB_PROP_ID_COMMAND_ENABLE_ALL_ITEM,
  eFB_DB_PROP_ID_COMMAND_DISABLE_ALL_ITEM

}eFaraabinLinkSerializer_DataBusPropertyIdCommand;

/** @} */ //End of SERIALIZER_DATABUS_PEROPERTY

/** @defgroup SERIALIZER_CODE_BLOCK_PEROPERTY typedefs
 *  @{
 */
typedef enum {

  eFB_CB_PROP_ID_SETTING_ENALBE = eFB_COMMON_PROP_ID_SETTING_ENABLE,
  eFB_CB_PROP_ID_SETTING_ALL = eFB_COMMON_PROP_ID_SETTING_ALL,
  eFB_CB_PROP_ID_SETTING_LIMIT_PARAM,
  eFB_CB_PROP_ID_SETTING_EVENT_TO_STREAM_ENABLE,
  eFB_CB_PROP_ID_SETTING_EVENT_TO_CAPTURE_ENABLE,
  eFB_CB_PROP_ID_SETTING_DATA_TO_STREAM_ENABLE

}eFaraabinLinkSerializer_CodeBlockPropertyIdSetting;

typedef enum {

  eFB_CB_PROP_ID_MONITORING_VALUE

}eFaraabinLinkSerializer_CodeBlockPropertyIdMonitoring;

typedef enum {

  eFB_CB_PROP_ID_COMMAND_RESET_VALUE

}eFaraabinLinkSerializer_CodeBlockPropertyIdCommand;

/** @} */ //End of SERIALIZER_CODE_BLOCK_PEROPERTY

/** @defgroup SERIALIZER_STATE_MACHINE_PEROPERTY typedefs
 *  @{
 */

typedef enum {

  eFB_SM_PROP_ID_SETTING_ENALBE = eFB_COMMON_PROP_ID_SETTING_ENABLE,
  eFB_SM_PROP_ID_SETTING_ALL = eFB_COMMON_PROP_ID_SETTING_ALL,
  eFB_SM_PROP_ID_SETTING_ENABLE_ALL_TRANSITIONS,
  eFB_SM_PROP_ID_SETTING_ENABLE_ALL_STATE_TRANSITIONS

}eFaraabinLinkSerializer_StateMachinePropertyIdSetting;

typedef enum {

  eFB_SM_PROP_ID_MONITORING_VALUE

}eFaraabinLinkSerializer_StateMachinePropertyIdMonitoring;

typedef enum {

  eFB_SM_PROP_ID_COMMAND_MANUAL_TRANSITION,
  eFB_SM_PROP_ID_COMMAND_VIRTUAL_TRANSITION,
  eFB_SM_PROP_ID_COMMAND_RESET

}eFaraabinLinkSerializer_StateMachinePropertyIdCommand;

/** @} */ //End of SERIALIZER_STATE_MACHINE_PEROPERTY

/** @defgroup SERIALIZER_FUNCTION_PEROPERTY typedefs
 *  @{
 */

typedef enum {
  eFB_FN_PROP_ID_SETTING_ENALBE = eFB_COMMON_PROP_ID_SETTING_ENABLE,
  eFB_FN_PROP_ID_SETTING_ALL = eFB_COMMON_PROP_ID_SETTING_ALL,
}eFaraabinLinkSerializer_FunctionPropertyIdSetting;

typedef enum {

  eFB_FN_PROP_ID_MONITORING_STATUS

}eFaraabinLinkSerializer_FunctionPropertyIdMonitoring;

typedef enum {

  eFB_FN_PROP_ID_COMMAND_RUN,
  eFB_FN_PROP_ID_COMMAND_STOP,
  eFB_FN_PROP_ID_COMMAND_PAUSE,
  eFB_FN_PROP_ID_COMMAND_RESUME

}eFaraabinLinkSerializer_FunctionPropertyIdCommand;

/** @} */ //End of SERIALIZER_FUNCTION_PEROPERTY

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes link serializer module.
 * 
 * @param txBuffer Pointer to the TX buffer that serializer needs to keep transmitting data.
 * @param txBufferSize Size of the TX buffer.
 * @return uint8_t Initialization result. '0' if successful and '1' in case of failure.
 */
uint8_t fFaraabinLinkSerializer_Init(uint8_t *txBuffer, uint32_t txBufferSize);

/**
 * @brief Serializes events that are sent to faraabin application.
 * 
 * @param fobjectPtr Pointer to the fobject that owns the event.
 * @param extendedFobjectPtr Pointer to the object oriented instance of the main fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param fobjectEnableState Enable status of the fobject.
 * @param eventPropId Property ID of the event.
 * @param eventSeverity Severity of the event.
 * @param eventId ID of the event. Each fobject has its own event ID that is defined in corresponding header files.
 * @param param Pointer to the parameters that will be sent with the event. Each fobject can have their own event parameters.
 * @param paramSize Size of the event parameters.
 * @param reqSeq Request sequence counter of the event.
 * @param isResponse Flag for checking if the event is response or not.
 * @param generatePayloadFunc Pointer to the function that generates event payload. Each fobject has their own GeneratePayload function in corresponding files.
 * @param payloadParam Pointer to the payload parameters that will be passed to the GeneratePayload function.
 * @param isEnd IsEnd control field in the serializer.
 */
void fFaraabinLinkSerializer_SerializeEvent(
  uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t *fobjectSeq,
  bool fobjectEnableState,
  eFaraabinLinkSerializer_CommonPropertyIdEvent eventPropId,
  eFaraabinLinkSerializer_EventSeverity eventSeverity,
  uint16_t eventId, 
  void *param, 
  uint16_t paramSize, 
  uint8_t reqSeq,
  bool isResponse, 
  void(*generatePayloadFunc)(uint32_t fobjectPtr, void *param), void *payloadParam, 
  bool isEnd);

/**
 * @brief This is a special case of fFaraabinLinkSerializer_SerializeEvent() for printing formatted string events.
 * 
 * @param fobjectPtr Pointer to the fobject that owns the event.
 * @param extendedFobjectPtr Pointer to the object oriented instance of the main fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param fobjectEnableState Enable status of the fobject.
 * @param eventPropId Property ID of the event.
 * @param severity Severity of the event.
 * @param format format specifier for string based events.
 * @param ... variadic arguments.
 */
void fFaraabinLinkSerializer_SerializeEventPrintf(uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t *fobjectSeq,
  bool fobjectEnableState,
  eFaraabinLinkSerializer_CommonPropertyIdEvent eventPropId,
  eFaraabinLinkSerializer_EventSeverity severity,
  const char *format, ...);

/**
 * @brief This is a special case of fFaraabinLinkSerializer_SerializeEvent() for sending string events.
 * 
 * @param fobjectPtr Pointer to the fobject that owns the event.
 * @param extendedFobjectPtr Pointer to the object oriented instance of the main fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param fobjectEnableState Enable status of the fobject.
 * @param eventPropId Property ID of the event.
 * @param severity Severity of the event.
 * @param stringPtr Pointer to the beginning of the string to be sent.
 */
void fFaraabinLinkSerializer_SerializeEventPuts(uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t *fobjectSeq,
  bool fobjectEnableState,
  eFaraabinLinkSerializer_CommonPropertyIdEvent eventPropId,
  eFaraabinLinkSerializer_EventSeverity severity,
  const char *stringPtr);

/**
 * @brief This is a special case of fFaraabinLinkSerializer_SerializeEvent() for sending enumerated events.
 * 
 * @note In these events, only an enumerated value is sent to faraabin and faraabin can print the name of the enum introduced in dictionry.
 *       These type of events are more efficient compared to string types if texts are always the same.
 * 
 * @param fobjectPtr Pointer to the fobject that owns the event.
 * @param severity Severity of the event.
 * @param enumPtr Pointer to the enumerated type.
 * @param enumId Value in the enumeration for sending the event.
 */
void fFaraabinLinkSerializer_SerializeEventEnumId(uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t *fobjectSeq,
  bool fobjectEnableState,
  eFaraabinLinkSerializer_EventSeverity severity,
  uint32_t enumPtr,
  uint16_t enumId);

/**
 * @brief Serializes dictionary of fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param dictIndex Index of the fobject in database.
 * @param fobjectSeqPtr Pointer to the sequence counter of the fobject.
 * @param reqSeq Request sequence counter.
 * @param dictPayloadParam Pointer to the payload parameters that will be passed to the GeneratePayload function.
 */
void fFaraabinLinkSerializer_SerializeDict(
  uint32_t fobjectPtr,
  uint16_t dictIndex,
  uint8_t reqSeq);

/**
 * @brief Flushes main queue of faraabin.
 * 
 * @param ptrToBuffer Pointer to the pointer of faraabin TX buffer.
 * @return num Number of bytes that have been flushed.
 */
uint16_t fFaraabinLinkSerializer_FlushBuffer(uint8_t **ptrToBuffer);

/* Helper functions ----------------------------------------------------------*/
/**
 * @brief This is a helper function from SerializeFrame() to send common user data via faraabin link.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for checking if this frame is response or not.
 * @param userData Pointer to the user data.
 * @param userDataSize Size of the user data for sending.
 */
void fFaraabinLinkSerializer_CommonSendUserData(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, uint8_t *userData, uint16_t userDataSize);

/**
 * @brief This is a helper function from SerializeFrame() to send common user enable status via faraabin link.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for checking if this frame is response or not.
 */
void fFaraabinLinkSerializer_CommonSendEnable(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse);

/**
 * @brief This is a helper function from SerializeFrame() to generate and send values of CodeBlock fobject.
 * 
 * @param fobjectPtr Pointer to the code block fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for checking if this frame is response or not.
 */
void fFaraabinLinkSerializer_CodeBlockSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse);

/**
 * @brief This is a helper function from SerializeFrame() to generate and send values of CodeBlock fobject.
 * 
 * @param fobjectPtr Pointer to the code block fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 */
void fFaraabinLinkSerializer_CodeBlockSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq);

/**
 * @brief This is a helper function from SerializeFrame() to send databus settings via faraabin link.
 * 
 * @param fobjectPtr Pointer to the databus fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 */
void fFaraabinLinkSerializer_DataBusSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq);

/**
 * @brief This is a helper function from SerializeFrame() to send databus captured values via faraabin link.
 * 
 * @param fobjectPtr Pointer to the databus fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isEnd Flag that indicates this is the last chunk of data.
 * @param captureValue Pointer to the captured value.
 */
void fFaraabinLinkSerializer_DataBusSendCaptureValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isEnd, void* captureValue);

/**
 * @brief This is a helper function from SerializeFrame() to send databus stream values via faraabin link.
 * 
 * @param fobjectPtr Pointer to the databus fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for indicating that this frame is a response.
 */
void fFaraabinLinkSerializer_DataBusSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse);

/**
 * @brief This is a helper function from SerializeFrame() to send variable values via faraabin link.
 * 
 * @param fobjectPtr Pointer to the variable.
 * @param size Size of the variable.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for indicating that this frame is a response.
 */
void fFaraabinLinkSerializer_VarSendValue(uint32_t fobjectPtr, uint32_t dataPtr, uint32_t size, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse);

/**
 * @brief This is a helper function from SerializeFrame() to send MCU ping results via faraabin link.
 * 
 * @param fobjectPtr Pointer to the MCU fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 * @param param Pointer to the parameters of the ping to be sent to application.
 */
void fFaraabinLinkSerializer_McuSendPing(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, void* param);

/**
 * @brief This is a helper function from SerializeFrame() to send MCU live frame via faraabin link.
 * 
 * @param fobjectPtr Pointer to the MCU fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_McuSendLive(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq);

/**
 * @brief This is a helper function from SerializeFrame() to send MCU WhoAmI frame via faraabin link.
 * 
 * @param fobjectPtr Pointer to the MCU fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_McuSendWhoAmI(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq);

void fFaraabinLinkSerializer_McuSendProfiler(uint32_t fobjectPtr, uint8_t *fobjectSeq);

/**
 * @brief This is a helper function from SerializeFrame() to send state machine values via faraabin link.
 * 
 * @param fobjectPtr Pointer to the state machine fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 * @param isResponse Flag that indicates this frame is a response.
 */
void fFaraabinLinkSerializer_StateMachineSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse);

/**
 * @brief This is a helper function from SerializeFrame() to send state machine settings via faraabin link.
 * 
 * @param fobjectPtr Pointer to the state machine fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_StateMachineSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeqPtr, uint8_t reqSeq);

/**
 * @brief This is a helper function from SerializeFrame() to send user defined structure type members via faraabin link.
 * 
 * @param structPtr Pointer to the instance fo the varType fobject of the user defined type.
 * @param varTypePtr Pointer to the member of the structure.
 * @param varPtr Pointer to the variable.
 * @param varArrayQty Number of elements, if the variable is a buffer.
 * @param isPtr Flag that indicates if this variable is a pointer.
 * @param name Pointer to the string that contains the name given for this member of the structure.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_VarTypeStructMemberDict(
  uint32_t structPtr,
  uint32_t varTypePtr,
  uint32_t varPtr,
  uint32_t varArrayQty,
  bool isPtr,
  char *name,
  uint8_t reqSeq);

/**
 * @brief This is a helper function from SerializeFrame() to send user defined enumeration type members via faraabin link.
 * 
 * @param enumPtr Pointer to the enum fobject.
 * @param enumMemberId Id of the member.
 * @param enumMemberName Pointer to the string that contains the name of the member.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_VarTypeEnumMemberDict(
  uint32_t enumPtr,
  uint16_t enumMemberId,
  char *enumMemberName,
  uint8_t reqSeq);

/**
 * @brief This is a helper function from SerializeFrame() to send variable dictionary via faraabin link.
 * 
 * @param varTypePtr Pointer to the varType object.
 * @param varPtr Pointer to the variable.
 * @param varArrayQty Number of elements if the variable is array. If not it is equal to '1'.
 * @param accessCallback Pointer to the callback function of the access to this variable.
 * @param externalInterface Pointer to the interface function if this variable is outside of the internal RAM of the MCU.
 * @param isPtr Flag for checking if this variable is a pointer.
 * @param name Pointer to the string that contains the name of this variable.
 * @param path Pointer to the string that contains the path given to this variable.
 * @param fileName Pointer to the string that contains the filename where variale is declared.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_VarDict(  
  uint32_t varTypePtr,
  uint32_t varPtr,
  uint32_t varArrayQty,
  uint32_t accessCallback,
  uint32_t externalInterface,
  bool isPtr,
  char *name,
  char *path,
  char *fileName,
  uint8_t reqSeq);

/**
 * @brief This is a helper function from SerializeFrame() to send function group member dictionary via faraabin link.
 * 
 * @param groupTypePtr Pointer to the function group type.
 * @param fn Pointer to the function fobject.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_FunctionGroupTypeMemberDict(uint32_t groupTypePtr, void *fn, uint8_t reqSeq);

/**
 * @brief This is a helper function from SerializeFrame() to send function group dictionary via faraabin link.
 * 
 * @param path Pointer to the string that contains the path given to this variable.
 * @param fileName Pointer to the string that contains the filename where variale is declared.
 * @param objectName Pointer to the string that contains the name of this variable.
 * @param objectPtr Pointer to the fobject.
 * @param groupTypePtr Pointer to the group type.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_FunctionGroupDict(
  char *path,
  char *fileName,
  char *objectName,
  uint32_t objectPtr,
  uint32_t groupTypePtr,
  uint8_t reqSeq);

/**
 * @brief Counts all of the frames for dictionries in database.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @return num Number of frames.
 */
uint32_t fFaraabinLinkSerializer_CountDictsFrames(uint32_t fobjectPtr);

/**
 * @brief Gets the RAM usage of the Faraabin link serializer.
 * 
 * @return usage The amount of bytes used by link serializer. 
 */
uint32_t fFaraabinLinkSerializer_GetRamUsage(void);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_LINK_SERIALIZER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
