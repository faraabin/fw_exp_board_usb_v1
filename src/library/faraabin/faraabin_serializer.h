/**
 ******************************************************************************
 * @file           : faraabin_serializer.h
 * @brief          : faraabin serializer header file.
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
#ifndef FARAABIN_SERIALIZER_H
#define FARAABIN_SERIALIZER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/

/** @defgroup SERIALIZER_PROPERY_GROUP
 *  @{
 */

#define FB_PROP_GROUP_DICT        ((uint8_t)0x00U)
#define FB_PROP_GROUP_SETTING     ((uint8_t)0x01U)
#define FB_PROP_GROUP_MONITORING  ((uint8_t)0x02U)
#define FB_PROP_GROUP_COMMAND     ((uint8_t)0x03U)
#define FB_PROP_GROUP_EVENT       ((uint8_t)0x04U)

/** @} */ //End of SERIALIZER_PROPERY_GROUP

/** @defgroup SERIALIZER_COMMON_PROPERTY typedefs
 *  @{
 */

/**
 * @ingroup SERIALIZER_COMMON_PROPERTY_SETTING
 * 
 */

#define FB_COMMON_PROP_ID_SETTING_ENABLE  ((uint8_t)0x00U)
#define FB_COMMON_PROP_ID_SETTING_ALL     ((uint8_t)0x01U)

/** @} */ //End of SERIALIZER_COMMON_PROPERTY_SETTING

/**
 * @ingroup SERIALIZER_COMMON_PROPERTY_MONITORING
 * 
 */

#define FB_COMMON_PROP_ID_MONITORING_RESERVE  ((uint8_t)0x00U)

/** @} */ //End of SERIALIZER_COMMON_PROPERTY_MONITORING

/**
 * @ingroup SERIALIZER_COMMON_PROPERTY_COMMAND
 * 
 */

#define FB_COMMON_PROP_ID_COMMAND_RESERVE  ((uint8_t)0x00U)

/** @} */ //End of SERIALIZER_COMMON_PROPERTY_COMMAND

/**
 * @ingroup SERIALIZER_COMMON_PROPERTY_EVENT
 * 
 */

#define FB_COMMON_PROP_ID_EVENT_SW            ((uint8_t)0x00U)
#define FB_COMMON_PROP_ID_EVENT_SW_EXCEPTION  ((uint8_t)0x01U)
#define FB_COMMON_PROP_ID_EVENT_LIB           ((uint8_t)0x02U)
#define FB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION ((uint8_t)0x03U)
#define FB_COMMON_PROP_ID_EVENT_USER_DATA     ((uint8_t)0x04U)
#define FB_COMMON_PROP_ID_EVENT_USER_CODE     ((uint8_t)0x05U)
#define FB_COMMON_PROP_ID_EVENT_USER_TERMINAL ((uint8_t)0x06U)

/** @} */ //End of SERIALIZER_COMMON_PROPERTY_EVENT
/** @} */ //End of SERIALIZER_COMMON_PROPERTY

/** @defgroup SERIALIZER_EVENT_SEVERITY typedefs
 *  @{
 */

#define FO_EVENT_SEVERITY_INFO    ((uint8_t)0x00U)
#define FO_EVENT_SEVERITY_WARNING ((uint8_t)0x01U)
#define FO_EVENT_SEVERITY_ERROR   ((uint8_t)0x02U)

/** @} */ //End of SERIALIZER_EVENT_SEVERITY

/** @defgroup SERIALIZER_MCU_PEROPERTY typedefs
 *  @{
 */

/**
 * @ingroup SERIALIZER_MCU_PEROPERTY_SETTING
 * 
 */

#define FB_MCU_PROP_ID_SETTING_ENABLE               FB_COMMON_PROP_ID_SETTING_ENABLE
#define FB_MCU_PROP_ID_SETTING_ALL                  FB_COMMON_PROP_ID_SETTING_ALL
#define FB_MCU_PROP_ID_SETTING_SEND_PROFILER_ENABLE ((uint8_t)0x02U)

/** @} */ //End of SERIALIZER_MCU_PEROPERTY_SETTING

/**
 * @ingroup SERIALIZER_MCU_PEROPERTY_MONITORING
 * 
 */

#define FB_MCU_PROP_ID_MONITORING_VARIABLE  ((uint8_t)0x00U)
#define FB_MCU_PROP_ID_MONITORING_LIVE      ((uint8_t)0x01U)
#define FB_MCU_PROP_ID_MONITORING_PING      ((uint8_t)0x02U)
#define FB_MCU_PROP_ID_MONITORING_WHOAMI    ((uint8_t)0x03U)
#define FB_MCU_PROP_ID_MONITORING_PROFILER  ((uint8_t)0x04U)

/** @} */ //End of SERIALIZER_MCU_PEROPERTY_MONITORING

/**
 * @ingroup SERIALIZER_MCU_PEROPERTY_COMMAND
 * 
 */

#define FB_MCU_PROP_ID_COMMAND_PING                   ((uint8_t)0x00U)
#define FB_MCU_PROP_ID_COMMAND_SEND_LIVE              ((uint8_t)0x01U)
#define FB_MCU_PROP_ID_COMMAND_SEND_WHOAMI            ((uint8_t)0x02U)
#define FB_MCU_PROP_ID_COMMAND_SEND_ALL_DICT          ((uint8_t)0x03U)
#define FB_MCU_PROP_ID_COMMAND_RESET_CPU              ((uint8_t)0x04U)
#define FB_MCU_PROP_ID_COMMAND_CLEAR_FLAG_BUFFER_OVF  ((uint8_t)0x05U)

/** @} */ //End of SERIALIZER_MCU_PEROPERTY_COMMAND
/** @} */ //End of SERIALIZER_MCU_PEROPERTY

/** @defgroup SERIALIZER_DATABUS_PEROPERTY typedefs
 *  @{
 */

/**
 * @ingroup SERIALIZER_DATABUS_PEROPERTY_SETTING
 * 
 */

#define FB_DB_PROP_ID_SETTING_ENABLE          FB_COMMON_PROP_ID_SETTING_ENABLE
#define FB_DB_PROP_ID_SETTING_ALL             FB_COMMON_PROP_ID_SETTING_ALL
#define FB_DB_PROP_ID_SETTING_ITEM_ENABLE     ((uint8_t)0x02U)
#define FO_DB_PROP_ID_SETTING_API_TRIG_ENABLE ((uint8_t)0x03U)
#define FO_DB_PROP_ID_SETTING_CH_TRIG_ENABLE  ((uint8_t)0x04U)
#define FO_DB_PROP_ID_SETTING_CH_TRIG_CONFIG  ((uint8_t)0x05U)
#define FO_DB_PROP_ID_SETTING_STREAM_DIVIDEBY ((uint8_t)0x06U)
#define FO_DB_PROP_ID_SETTING_TIMER_DIVIDEBY  ((uint8_t)0x07U)
#define FO_DB_PROP_ID_SETTING_TRIG_DIVIDEBY   ((uint8_t)0x08U)

/** @} */ //End of SERIALIZER_DATABUS_PEROPERTY_SETTING

/**
 * @ingroup SERIALIZER_DATABUS_PEROPERTY_MONITORING
 * 
 */

#define FB_DB_PROP_ID_MONITORING_CAPTURE_VALUE  ((uint8_t)0x00U)
#define FB_DB_PROP_ID_MONITORING_STREAM_VALUE   ((uint8_t)0x01U)

/** @} */ //End of SERIALIZER_DATABUS_PEROPERTY_MONITORING

/**
 * @ingroup SERIALIZER_DATABUS_PEROPERTY_COMMAND
 * 
 */

#define FB_DB_PROP_ID_COMMAND_ATTACH            ((uint8_t)0x00U)
#define FB_DB_PROP_ID_COMMAND_DETACH            ((uint8_t)0x01U)
#define FB_DB_PROP_ID_COMMAND_DETACH_ALL        ((uint8_t)0x02U)
#define FB_DB_PROP_ID_COMMAND_START_STREAM      ((uint8_t)0x03U)
#define FB_DB_PROP_ID_COMMAND_START_TRIG        ((uint8_t)0x04U)
#define FB_DB_PROP_ID_COMMAND_START_TIMER       ((uint8_t)0x05U)
#define FB_DB_PROP_ID_COMMAND_STOP              ((uint8_t)0x06U)
#define FB_DB_PROP_ID_COMMAND_CAPTURE_SEND      ((uint8_t)0x07U)
#define FB_DB_PROP_ID_COMMAND_MANUAL_TRIG       ((uint8_t)0x08U)
#define FB_DB_PROP_ID_COMMAND_ENABLE_ALL_ITEM   ((uint8_t)0x09U)
#define FB_DB_PROP_ID_COMMAND_DISABLE_ALL_ITEM  ((uint8_t)0x0AU)

/** @} */ //End of SERIALIZER_DATABUS_PEROPERTY_COMMAND
/** @} */ //End of SERIALIZER_DATABUS_PEROPERTY

/** @defgroup SERIALIZER_CODE_BLOCK_PEROPERTY typedefs
 *  @{
 */

/**
 * @ingroup SERIALIZER_CODE_BLOCK_PEROPERTY_SETTING
 * 
 */

#define FB_CB_PROP_ID_SETTING_ENALBE                  FB_COMMON_PROP_ID_SETTING_ENABLE
#define FB_CB_PROP_ID_SETTING_ALL                     FB_COMMON_PROP_ID_SETTING_ALL
#define FB_CB_PROP_ID_SETTING_LIMIT_PARAM             ((uint8_t)0x02U)
#define FB_CB_PROP_ID_SETTING_EVENT_TO_STREAM_ENABLE  ((uint8_t)0x03U)
#define FB_CB_PROP_ID_SETTING_EVENT_TO_CAPTURE_ENABLE ((uint8_t)0x04U)
#define FB_CB_PROP_ID_SETTING_DATA_TO_STREAM_ENABLE   ((uint8_t)0x05U)

/** @} */ //End of SERIALIZER_CODE_BLOCK_PEROPERTY_SETTING

/**
 * @ingroup SERIALIZER_CODE_BLOCK_PEROPERTY_MONITORING
 * 
 */

#define FB_CB_PROP_ID_MONITORING_VALUE  ((uint8_t)0x00U)

/** @} */ //End of SERIALIZER_CODE_BLOCK_PEROPERTY_MONITORING

/**
 * @ingroup SERIALIZER_CODE_BLOCK_PEROPERTY_COMMAND
 * 
 */

#define FB_CB_PROP_ID_COMMAND_RESET_VALUE ((uint8_t)0x00U)

/** @} */ //End of SERIALIZER_CODE_BLOCK_PEROPERTY_COMMAND
/** @} */ //End of SERIALIZER_CODE_BLOCK_PEROPERTY

/** @defgroup SERIALIZER_STATE_MACHINE_PEROPERTY typedefs
 *  @{
 */

/**
 * @ingroup SERIALIZER_STATE_MACHINE_PEROPERTY_SETTING
 * 
 */

#define FB_SM_PROP_ID_SETTING_ENALBE                        FB_COMMON_PROP_ID_SETTING_ENABLE
#define FB_SM_PROP_ID_SETTING_ALL                           FB_COMMON_PROP_ID_SETTING_ALL
#define FB_SM_PROP_ID_SETTING_ENABLE_ALL_TRANSITIONS        ((uint8_t)0x02U)
#define FB_SM_PROP_ID_SETTING_ENABLE_ALL_STATE_TRANSITIONS  ((uint8_t)0x03U)

/** @} */ //End of SERIALIZER_STATE_MACHINE_PEROPERTY_SETTING

/**
 * @ingroup SERIALIZER_STATE_MACHINE_PEROPERTY_MONITORING
 * 
 */

#define FB_SM_PROP_ID_MONITORING_VALUE  ((uint8_t)0x00U)
/** @} */ //End of SERIALIZER_STATE_MACHINE_PEROPERTY_MONITORING

/**
 * @ingroup SERIALIZER_STATE_MACHINE_PEROPERTY_COMMAND
 * 
 */

#define FB_SM_PROP_ID_COMMAND_MANUAL_TRANSITION   ((uint8_t)0x00U)
#define FB_SM_PROP_ID_COMMAND_VIRTUAL_TRANSITION  ((uint8_t)0x01U)
#define FB_SM_PROP_ID_COMMAND_RESET               ((uint8_t)0x02U)

/** @} */ //End of SERIALIZER_STATE_MACHINE_PEROPERTY_COMMAND
/** @} */ //End of SERIALIZER_STATE_MACHINE_PEROPERTY

/** @defgroup SERIALIZER_FUNCTION_PEROPERTY typedefs
 *  @{
 */

/**
 * @ingroup SERIALIZER_FUNCTION_PEROPERTY_SETTING
 * 
 */

#define FB_FN_PROP_ID_SETTING_ENALBE  FB_COMMON_PROP_ID_SETTING_ENABLE
#define FB_FN_PROP_ID_SETTING_ALL     FB_COMMON_PROP_ID_SETTING_ALL

/** @} */ //End of SERIALIZER_FUNCTION_PEROPERTY_SETTING

/**
 * @ingroup SERIALIZER_FUNCTION_PEROPERTY_MONITORING
 * 
 */

#define FB_FN_PROP_ID_MONITORING_STATUS ((uint8_t)0x00U)
/** @} */ //End of SERIALIZER_FUNCTION_PEROPERTY_MONITORING

/**
 * @ingroup SERIALIZER_FUNCTION_PEROPERTY_COMMAND
 * 
 */

#define FB_FN_PROP_ID_COMMAND_RUN     ((uint8_t)0x00U)
#define FB_FN_PROP_ID_COMMAND_STOP    ((uint8_t)0x01U)
#define FB_FN_PROP_ID_COMMAND_PAUSE   ((uint8_t)0x02U)
#define FB_FN_PROP_ID_COMMAND_RESUME  ((uint8_t)0x03U)

/** @} */ //End of SERIALIZER_FUNCTION_PEROPERTY_COMMAND
/** @} */ //End of SERIALIZER_FUNCTION_PEROPERTY

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/

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
void fFaraabinLink_Serialize_Event(
  uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t *fobjectSeq,
  bool fobjectEnableState,
  uint8_t eventPropId,
  uint8_t eventSeverity,
  uint16_t eventId, 
  void *param, 
  uint16_t paramSize, 
  uint8_t reqSeq,
  bool isResponse, 
  void(*generatePayloadFunc)(uint32_t fobjectPtr, void *param), void *payloadParam, 
  bool isEnd,
  bool isCmdBuffer);

/**
 * @brief This is a special case of fFaraabinLink_Serialize_Event() for printing formatted string events.
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
void fFaraabinLink_Serialize_EventPrintf(uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t *fobjectSeq,
  bool fobjectEnableState,
  uint8_t eventPropId,
  uint8_t severity,
  const char *format, ...);

/**
 * @brief This is a special case of fFaraabinLink_Serialize_Event() for sending string events.
 * 
 * @param fobjectPtr Pointer to the fobject that owns the event.
 * @param extendedFobjectPtr Pointer to the object oriented instance of the main fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param fobjectEnableState Enable status of the fobject.
 * @param eventPropId Property ID of the event.
 * @param severity Severity of the event.
 * @param stringPtr Pointer to the beginning of the string to be sent.
 */
void fFaraabinLink_Serialize_EventPuts(uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t *fobjectSeq,
  bool fobjectEnableState,
  uint8_t eventPropId,
  uint8_t severity,
  char *stringPtr);

/**
 * @brief This is a special case of fFaraabinLink_Serialize_Event() for sending enumerated events.
 * 
 * @note In these events, only an enumerated value is sent to faraabin and faraabin can print the name of the enum introduced in dictionry.
 *       These type of events are more efficient compared to string types if texts are always the same.
 * 
 * @param fobjectPtr Pointer to the fobject that owns the event.
 * @param severity Severity of the event.
 * @param enumPtr Pointer to the enumerated type.
 * @param enumId Value in the enumeration for sending the event.
 */
void fFaraabinLink_Serialize_EventEnumId(uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t *fobjectSeq,
  bool fobjectEnableState,
  uint8_t severity,
  uint32_t enumPtr,
  uint16_t enumId);

void fFaraabinLink_Serialize_McuSendProfiler(uint32_t fobjectPtr);

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
void fFaraabinLink_Serialize_VarTypeStructMemberDict(
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
void fFaraabinLink_Serialize_VarTypeEnumMemberDict(
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
void fFaraabinLink_Serialize_VarDict(  
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
void fFaraabinLink_Serialize_FnGrpTypeMemberDict(uint32_t groupTypePtr, void *fn, uint8_t reqSeq);

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
void fFaraabinLink_Serialize_FnGrpDict(
  char *path,
  char *fileName,
  char *objectName,
  uint32_t objectPtr,
  uint32_t groupTypePtr,
  uint8_t reqSeq);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_SERIALIZER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
