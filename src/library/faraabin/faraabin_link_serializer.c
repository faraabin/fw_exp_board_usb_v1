/**
 ******************************************************************************
 * @file           : faraabin_link_serializer.c
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

/* Includes ------------------------------------------------------------------*/
#include "faraabin_link_serializer.h"

#include "faraabin.h"
#include "faraabin_port.h"
#include "faraabin_link_buffer.h"

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER 
#include "add_on/cpu_profiler/faraabin_addon_cpu_profiler.h"
#endif

#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
#include "add_on/state_machine/faraabin_addon_state_machine.h"
#endif

#include <stdarg.h>

/* Private define ------------------------------------------------------------*/
/**
 * @brief Faraabin link end of frame identifier.
 * 
 */
#define FB_EOF      0x7EU

/**
 * @brief Faraabin link byte escaping character.
 * 
 */
#define FB_ESC      0x7DU

/**
 * @brief Faraabin link byte escaping xor character.
 * 
 */
#define FB_ESC_XOR  0x20U

/**
 * @brief Serializer ID for common property.
 * 
 */
#define FB_COMMON_PROP_ID_DICT  0U

/* Private macro -------------------------------------------------------------*/
/**
 * @brief In case of null or empty, this macro corrects given path to a fobject.
 * 
 */
#define CorrectPath_(str_)  \
  if(str_ == NULL) {\
    str_ = (char*)RootPath____;\
  } else if(str_[0] == 0x00U) {\
    str_ = (char*)RootPath____;\
  } else {\
    /* Do nothing */\
  }

/**
 * @brief This macro adds control byte to the generated frame.
 * 
 */
#define ADD_CONTROL_BYTE_() \
  do {\
    uint8_t control = 0U;\
    control = me->_type & 0x7FU;\
    if(me->_init) {\
      control |= 0x80U;\
    }\
    fAddToBufferU8(control);\
  }while(0)

/* Private typedef -----------------------------------------------------------*/
/**
 * @brief Frame type identifier of link serializer.
 * 
 */
typedef enum {
  
  eFB_LINK_FRAME_TYPE_EVENT = 0,
  eFB_LINK_FRAME_TYPE_RESPONSE
  
}eFaraabinLinkSerializer_FrameType;

/**
 * @brief Faraabin link serializer typedef.
 * 
 */
typedef struct {

  uint16_t DepthCounter;  /*!< Counter of the nested events for sending. */

  uint8_t TextEventBuffer[TEXT_EVENT_MAX_REENTRANCE + 1U][TEXT_EVENT_BUFFER_SIZE]; /*!< Buffer allocated for saving message of the event. */

  uint8_t CheckSum;       /*!< Calculated checksum of the link serializer. */

  uint8_t NodeSeq;        /*!< Node sequence. */

}sLinkSerializer;

/**
 * @brief Faraabin event parameters in link serializer.
 * 
 */
typedef struct {
  
  eFaraabinLinkSerializer_EventSeverity Severity; /*!< Event severity. */
  
  uint16_t EventId;                               /*!< Event Identifier. */
  
  uint8_t *pParam;                                /*!< Pointer to the event parameters. */
  
  uint16_t ParamSize;                             /*!< Parameter size. */
  
  void(*fpGeneratePayload)(uint32_t fobjectPtr, void *param); /*!< Pointer to the function that is responsible for generating the payload. */
  
  void *pPayloadParam;                            /*!< Pointer to the payload parameters. */
  
}sEventParam;

/**
 * @brief Type of payload parameters for generating dictionary in faraabin.
 * 
 */
typedef struct {
  
  void(*fpGenerateDictPayload)(uint32_t fobjectPtr, void* dictPayloadParam);  /*!< Pointer to the function that generates dictionary payload.*/

  void *pDictPayloadParam;  /*!< Pointer to the payload parameters. */
  
}sGeneratePayloadDict_Param;

/**
 * @brief Dictionary iterator object.
 * 
 */
typedef struct {

  uint16_t TotalSubDicts;       /*!< Number of all sub-dictionaries. */

  uint16_t CurrentSubDictIndex; /*!< Current index of the sub-dictionary. */

  uint16_t CurrentDictIndex;    /*!< Current index of the current dictionary. */

  bool _isInCountingMode;       /*!< Flag for checking that dictionary is in counting mode. */

}sDictIterator;

/**
 * @brief User data parameters for common user data communications.
 * 
 */
typedef struct {
  
  uint8_t *UserData;      /*!< Pointer to the user data. */

  uint16_t UserDataSize;  /*!< User data size. */
  
}sUserDataParam;

/**
 * @brief Dictionary parameters for variable fobjects in faraabin.
 * 
 */
typedef struct {
  
  const char *Name;           /*!< Name of the fobject. */

  const char *Path;           /*!< Path of the fobject.*/

  const char *FileName;       /*!< Filename of the fobject.*/

  uint32_t VarPtr;            /*!< Pointer to the variable. */

  uint32_t VarTypePtr;        /*!< Pointer to the variable type. */

  uint32_t VarArrayQty;       /*!< Size of the array (if variable is defined as an array). */

  uint32_t ExternalInterface; /*!< Pointer to the external interface for reading and writing. */

  uint32_t AccessCallBack;    /*!< Pointer to the access callback function when reading and writing. */

  bool _isPtr;                /*!< Specifies a variable is whether pointer. */
  
}sDictVarPayloadParam;

/**
 * @brief Variable fobject parameters for serializing and sending frames to PC.
 * 
 */
typedef struct {
  
  uint32_t VarPtr;  /*!< Pointer to the variable. */

  uint32_t VarSize; /*!< Variable size. */

  uint32_t DataPtr; /*!< Pointer to the data for sending. */
  
}sVarSendParam;

/**
 * @brief Dictionary payload object for structure members in user defined types.
 * 
 */
typedef struct {
  
  const char *Name;     /*!< Name of the fobject. */

  uint32_t VarPtr;      /*!< Pointer to the variable. */

  uint32_t VarTypePtr;  /*!< Pointer to the vartype fobject. */

  uint32_t VarArrayQty; /*!< Size of the array (if variable is defined as an array). */

  bool _isPtr;          /*!< Specifies a variable is whether pointer. */
  
}sDictStructMemberPayload;

/**
 * @brief Dictionary payload object for enum members in user defined types.
 * 
 */
typedef struct {
  
  const char *Name; /*!< Name of the fobject. */

  uint16_t Id;      /*!< Enum identifier. */
  
}sDictEnumMemberPayload;

/**
 * @brief Dictionary payload object for function groups.
 * 
 */
typedef struct {
  
  const char *ObjectName; /*!< Name of the object. */
  
  const char *Path;       /*!< Path of the object. */
  
  const char *FileName;   /*!< Filename of the object. */
  
  uint32_t ObjectPtr;     /*!< Pointer to the object. */
  
  uint32_t GroupTypePtr;  /*!< Pointer to the function group. */
  
}sDictFunctionGroupPayloadParam;

typedef struct {
	
	sDictIterator DictIterator;     /*!< Handles iterating over all added dictionaries. */

	sLinkSerializer Serializer;     /*!< Manages the serialization of the frames that will be added to main queue of faraabin. */

	sFaraabinFobjectMcu* McuHandle; /*!< Pointer to the MCU fobject. */
	
}sSerializerInternal;

/* Private variables ---------------------------------------------------------*/
static sSerializerInternal _serializer;

/* Private function prototypes -----------------------------------------------*/
static void fSerializeFrame(
  eFaraabinLinkSerializer_FrameType frameType,
  uint8_t *fobjectSeq,
  uint8_t reqSeq,
  bool isEnd,
  uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t clientFrameGroup,
  uint8_t clientFrameId,
  void(*generatePayloadFunc)(uint32_t fobjectPtr, void *param), void *payloadParam);

static void fAddToBufferU8(uint8_t d);
static void fAddToBufferU16(uint16_t d);
static void fAddToBufferU32(uint32_t d);
static void fAddToBufferU64(uint64_t d);
#ifdef __FARAABIN_LINK_SERIALIZER_COMMENT_SECTION_0
static void fAddToBufferF32(float32_t d); // TODO: This function is reserved here for future use.
static void fAddToBufferF64(float64_t d); // TODO: This function is reserved here for future use.
#endif
static void fAddToBufferString(char *string);
static void fAddToBuffer(uint8_t *data, uint32_t size);

static void fCommonEnableStatusGeneratePayload(uint32_t fobjectPtr, void *param);
static void fCommonUserDataGeneratePayload(uint32_t fobjectPtr, void *param);

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
static void fDictGeneratePayloadCodeBlock(uint32_t fobjectPtr, void *param);
static void fCodeBlockValueGeneratePayload(uint32_t fobjectPtr, void *param);
static void fCodeBlockSettingGeneratePayload(uint32_t fobjectPtr, void *param);
static void fMcuProfilerGeneratePayload(uint32_t fobjectPtr, void *param);
#endif

static void fDataBusSettingGeneratePayload(uint32_t fobjectPtr, void *param);
static void fDataBusCaptureValueGeneratePayload(uint32_t fobjectPtr, void *param);
static void fDataBusValueGeneratePayload(uint32_t fobjectPtr, void *param);

static void fMcuPingGeneratePayload(uint32_t fobjectPtr, void *param);
static void fMcuLiveGeneratePayload(uint32_t fobjectPtr, void *param);
static void fMcuWhoAmIGeneratePayload(uint32_t fobjectPtr, void *param);

#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fStateMachineValueGeneratePayload(uint32_t fobjectPtr, void *param);
static void fStateMachineSettingGeneratePayload(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadStateMachine(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadStateMachineState(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadStateMachineTransition(uint32_t fobjectPtr, void *param);  
#endif

static void fVarValueGeneratePayload(uint32_t fobjectPtr, void *param);

static void fDictGeneratePayloadDataBus(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadEventGroup(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadContainer(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadMcu(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadVar(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadVarType(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadVarTypeStructMember(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadVarTypeEnumMember(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadIterator(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadFunctionGroupType(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadFunctionGroupTypeMember(uint32_t fobjectPtr, void *param);
static void fDictGeneratePayloadFunctionGroup(uint32_t fobjectPtr, void *param);

static void fFrameStart(void);
static void fFrameEnd(void);
static void fSerializePayload_Event(uint32_t fobjectPtr, void *param);

static void fDictIteratorIterate(void);
static void fDictIteratorResetCounter(uint16_t dictIndex);
static void fDictIteratorEnableCounting(void);
static void fDictIteratorDisableCounting(void);
static bool fDictIteratorIsInCountingMode(void);
static void fSerializeDict(
  uint32_t fobjectPtr,
  uint8_t *fobjectSeqPtr,
  uint8_t reqSeq,
  void(*fpGenerateDictPayload)(uint32_t, void*),
  void *pDictPayloadParam);
static void fGenerateDict(uint32_t fobjectPtr, uint8_t reqSeq);
static uint32_t fCountAllDictsFrames(void);

/* Variables -----------------------------------------------------------------*/
extern sFaraabinFobjectMcu faraabin_mcu__;
	
/*
===============================================================================
              ##### fb_link_serializer.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes link serializer module.
 * 
 * @note This function is called internally in faraabin.c file.
 * 
 * @param txBuffer Pointer to the TX buffer that serializer needs to keep transmitting data.
 * @param txBufferSize Size of the TX buffer.
 * @return result. '0' if successful and '1' in case of failure.
 */
uint8_t fFaraabinLinkSerializer_Init(uint8_t *txBuffer, uint32_t txBufferSize) {
  
  if(fFaraabinLinkBuffer_Init(txBuffer, txBufferSize) != 0U) {
    return 1;
  }
  
  for(uint16_t i = 0; i < TEXT_EVENT_MAX_REENTRANCE; i++) {

    for(uint16_t j = 0; j < TEXT_EVENT_BUFFER_SIZE; j++) {

      _serializer.Serializer.TextEventBuffer[i][j] = 0x00U;
      
    }
  }
  _serializer.Serializer.DepthCounter = 0U;
  
  _serializer.DictIterator.CurrentSubDictIndex = 0U;
  _serializer.DictIterator.TotalSubDicts = 0U;
  _serializer.DictIterator.CurrentDictIndex = 0U;
  _serializer.DictIterator._isInCountingMode = false;
  
  _serializer.McuHandle = fFaraabinFobjectMcu_GetFobject();

  return 0;
}
  
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
  bool isEnd) {
		
	if((!isResponse) && !fFaraabin_IsAllowEvent()) {
		return;
	}
  
  if(fobjectEnableState == false) {
    return;
  }

  sEventParam eventParam;

  eventParam.Severity = eventSeverity;
  eventParam.EventId = eventId;
  eventParam.pParam = param;
  eventParam.ParamSize = paramSize;
  eventParam.fpGeneratePayload = generatePayloadFunc;
  eventParam.pPayloadParam = payloadParam;

  fSerializeFrame(
    (isResponse) ? eFB_LINK_FRAME_TYPE_RESPONSE : eFB_LINK_FRAME_TYPE_EVENT,
    fobjectSeq,
    reqSeq,
    isEnd,
    fobjectPtr,
    extendedFobjectPtr,
    (uint8_t)eFB_PROP_GROUP_EVENT,
    (uint8_t)eventPropId,
    fSerializePayload_Event, &eventParam);
}

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
  const char *format, ...) {
		
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
  
  if(fobjectEnableState == false) {
    return;
  }
  
  if(format == NULL) {
    return;
  }

  if(_serializer.Serializer.DepthCounter >= (TEXT_EVENT_MAX_REENTRANCE + 1U)) {

    fFaraabinFobjectMcu_SendEventSystem(eMCU_EVENT_ERROR_MAX_PRINTF_REENTRANT);

    return;
  }
  
  _serializer.Serializer.DepthCounter++;

  uint16_t reentranceCounter = _serializer.Serializer.DepthCounter;

  va_list args;
  va_start(args, format);

  int stringLen = vsnprintf((char*)_serializer.Serializer.TextEventBuffer[reentranceCounter - 1U], TEXT_EVENT_BUFFER_SIZE, format, args);
  
  va_end(args);
  
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr,
    extendedFobjectPtr,
    fobjectSeq,
    fobjectEnableState,
    eventPropId,
    severity,
    0,
    _serializer.Serializer.TextEventBuffer[reentranceCounter - 1U],
    stringLen,
    0,
    false,
    NULL,
    NULL,
    true);
  
  _serializer.Serializer.DepthCounter--;
}

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
  const char *stringPtr) {
		
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}

  if(fobjectEnableState == false) {
    return;
  }

  if(stringPtr == NULL) {
    return;
  }

  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr,
    extendedFobjectPtr,
    fobjectSeq,
    fobjectEnableState,
    eventPropId,
    severity,
    0,
    (void*)stringPtr,
    strlen(stringPtr),
    0,
    false,
    NULL,
    NULL,
    true);
}

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
  uint16_t enumId) {
  
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr,
    extendedFobjectPtr,
    fobjectSeq,
    fobjectEnableState,
    eFB_COMMON_PROP_ID_EVENT_USER_CODE,
    severity,
    enumId,
    (uint8_t*)&enumPtr,
    4,
    0,
    false,
    NULL,
    NULL,
    true);
}

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
  uint8_t reqSeq) {
  
  fDictIteratorResetCounter(dictIndex);
  fDictIteratorEnableCounting();
  fGenerateDict(fobjectPtr, reqSeq);
  fDictIteratorDisableCounting();
  fGenerateDict(fobjectPtr, reqSeq);
}

/**
 * @brief Flushes main queue of faraabin.
 * 
 * @param ptrToBuffer Pointer to the pointer of faraabin TX buffer.
 * @return num Number of bytes that have been flushed.
 */
uint16_t fFaraabinLinkSerializer_FlushBuffer(uint8_t **ptrToBuffer) {
  
  uint16_t allowableSize = 0;
  
  fFaraabinLinkBuffer_FlushByPointer_(ptrToBuffer, &allowableSize);
  
  return allowableSize;
}

/*
===============================================================================
              ##### fb_link_serializer.c Helper Functions #####
===============================================================================*/
/**
 * @brief Serializes a faraabin frame for sending.
 * 
 * @param frameType Type of frame.
 * @param reqSeq Request sequence counter of the frame.
 * @param isEnd Flag for checking that this is the last frame.
 * @param fobjectPtr Pointer of the fobject.
 * @param fobjectProperty Property of the fobject.
 * @param generatePayloadFunc Pointer to the function responsible for generating corresponding payload.
 * @param payloadParam Pointer to the payload parameters that will be passed to the GeneratePayload function.
 */
static void fSerializeFrame(
  eFaraabinLinkSerializer_FrameType frameType,
  uint8_t *fobjectSeq,
  uint8_t reqSeq,
  bool isEnd,
  uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t clientFrameGroup,
  uint8_t clientFrameId,
  void(*generatePayloadFunc)(uint32_t fobjectPtr, void *param), void *payloadParam) {
		
	if((frameType == eFB_LINK_FRAME_TYPE_EVENT) && (!fFaraabin_IsAllowEvent())) {
		return;
	}
	
  FARAABIN_CRITICIAL_ENTER_;
    
  fFrameStart();

  uint8_t seq = 0;
  if(fobjectPtr != 0U) {
    (*fobjectSeq)++;
    if((*fobjectSeq) > 15U) {
      *fobjectSeq = 0U;
    }
    seq = *fobjectSeq;
  }
  
  _serializer.Serializer.NodeSeq++;
  if(_serializer.Serializer.NodeSeq > 15U) {
    _serializer.Serializer.NodeSeq = 0U;
  }

  uint16_t control = 0;
  control |= (((uint16_t)frameType & 0x01U) << 0U);
  control |= (((uint16_t)isEnd & 0x01U) << 1U);
  control |= (((uint16_t)reqSeq & 0x0FU) << 2U);
  control |= (((uint16_t)seq & 0x0FU) << 6U);
  control |= (((uint16_t)_serializer.Serializer.NodeSeq & 0x0FU) << 10U);
  uint8_t extPtr = (extendedFobjectPtr != 0U) ? 1U : 0U;
  control |= (((uint16_t)extPtr & 0x01U) << 14U);
  
  fAddToBufferU16(control);
  
  fAddToBufferU32(fChrono_GetTick());
  fAddToBufferU32(fobjectPtr);
  if(extPtr == 1U) {
    fAddToBufferU32(extendedFobjectPtr);
  }

  uint8_t fobjectProp = (uint8_t)(clientFrameGroup << 5U) + (uint8_t)(clientFrameId);
  fAddToBufferU8(fobjectProp);
  
  if(generatePayloadFunc != NULL) {
    generatePayloadFunc(fobjectPtr, payloadParam);
  }

  fFrameEnd();
  
  _serializer.McuHandle->StatisticsTxFramesCnt++;
  
  FARAABIN_CRITICIAL_EXIT_;
}

/**
 * @brief This is a helper function from fSerializeFrame() to generate and send values of CodeBlock fobject.
 * 
 * @param fobjectPtr Pointer to the code block fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for checking if this frame is response or not.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
void fFaraabinLinkSerializer_CodeBlockSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse) {

  fSerializeFrame(
    (isResponse) ? eFB_LINK_FRAME_TYPE_RESPONSE : eFB_LINK_FRAME_TYPE_EVENT,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_MONITORING,
    (uint8_t)eFB_CB_PROP_ID_MONITORING_VALUE,
    fCodeBlockValueGeneratePayload, NULL);
}
#endif

/**
 * @brief This is a helper function from fSerializeFrame() to generate and send values of CodeBlock fobject.
 * 
 * @param fobjectPtr Pointer to the code block fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
void fFaraabinLinkSerializer_CodeBlockSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq) {
  
  fSerializeFrame(
    eFB_LINK_FRAME_TYPE_RESPONSE,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_SETTING,
    (uint8_t)eFB_CB_PROP_ID_SETTING_ALL,
    fCodeBlockSettingGeneratePayload, NULL);
}
#endif

/**
 * @brief This is a helper function from fSerializeFrame() to send common user data via faraabin link.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for checking if this frame is response or not.
 * @param userData Pointer to the user data.
 * @param userDataSize Size of the user data for sending.
 */
void fFaraabinLinkSerializer_CommonSendUserData(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, uint8_t *userData, uint16_t userDataSize) {
  
  sUserDataParam userDataParam;
  userDataParam.UserData = (userData);
  userDataParam.UserDataSize = (userDataSize);

  fSerializeFrame(
    (isResponse) ? eFB_LINK_FRAME_TYPE_RESPONSE : eFB_LINK_FRAME_TYPE_EVENT,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_EVENT,
    (uint8_t)eFB_COMMON_PROP_ID_EVENT_USER_DATA,
    fCommonUserDataGeneratePayload, &userDataParam);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send common user enable status via faraabin link.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for checking if this frame is response or not.
 */
void fFaraabinLinkSerializer_CommonSendEnable(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse) {

  fSerializeFrame(
    (isResponse) ? eFB_LINK_FRAME_TYPE_RESPONSE : eFB_LINK_FRAME_TYPE_EVENT,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_SETTING,
    (uint8_t)eFB_COMMON_PROP_ID_SETTING_ENABLE,
    fCommonEnableStatusGeneratePayload, NULL);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send databus settings via faraabin link.
 * 
 * @param fobjectPtr Pointer to the databus fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 */
void fFaraabinLinkSerializer_DataBusSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq) {

  fSerializeFrame(
    eFB_LINK_FRAME_TYPE_RESPONSE,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_SETTING,
    (uint8_t)eFB_DB_PROP_ID_SETTING_ALL,
    fDataBusSettingGeneratePayload, NULL);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send databus captured values via faraabin link.
 * 
 * @param fobjectPtr Pointer to the databus fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isEnd Flag that indicates this is the last chunk of data.
 * @param captureValue Pointer to the captured value.
 */
void fFaraabinLinkSerializer_DataBusSendCaptureValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isEnd, void* captureValue) {

  fSerializeFrame(
    eFB_LINK_FRAME_TYPE_RESPONSE,
    (fobjectSeq),
    (reqSeq),
    (isEnd),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_MONITORING,
    (uint8_t)eFB_DB_PROP_ID_MONITORING_CAPTURE_VALUE,
    fDataBusCaptureValueGeneratePayload, captureValue);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send databus stream values via faraabin link.
 * 
 * @param fobjectPtr Pointer to the databus fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for indicating that this frame is a response.
 */
void fFaraabinLinkSerializer_DataBusSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse) {

  fSerializeFrame(
    (isResponse) ? eFB_LINK_FRAME_TYPE_RESPONSE : eFB_LINK_FRAME_TYPE_EVENT,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_MONITORING,
    (uint8_t)eFB_DB_PROP_ID_MONITORING_STREAM_VALUE,
    fDataBusValueGeneratePayload, NULL);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send variable values via faraabin link.
 * 
 * @param fobjectPtr Pointer to the variable.
 * @param size Size of the variable.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for indicating that this frame is a response.
 */
void fFaraabinLinkSerializer_VarSendValue(uint32_t fobjectPtr, uint32_t dataPtr, uint32_t size, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse) {
  
  sVarSendParam param;
  param.VarPtr = fobjectPtr;
  param.VarSize = size;
  param.DataPtr = dataPtr;

  fSerializeFrame(
    (isResponse) ? eFB_LINK_FRAME_TYPE_RESPONSE : eFB_LINK_FRAME_TYPE_EVENT,
    (fobjectSeq),
    (reqSeq),
    (true),
    (uint32_t)fFaraabinFobjectMcu_GetFobject(),
    0,
    (uint8_t)eFB_PROP_GROUP_MONITORING,
    (uint8_t)eFB_MCU_PROP_ID_MONITORING_VARIABLE,
    fVarValueGeneratePayload, &param);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send MCU ping results via faraabin link.
 * 
 * @param fobjectPtr Pointer to the MCU fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 * @param param Pointer to the parameters of the ping to be sent to application.
 */
void fFaraabinLinkSerializer_McuSendPing(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, void* param) {

  fSerializeFrame(
    eFB_LINK_FRAME_TYPE_RESPONSE,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_MONITORING,
    (uint8_t)eFB_MCU_PROP_ID_MONITORING_PING,
    fMcuPingGeneratePayload, param);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send MCU live frame via faraabin link.
 * 
 * @param fobjectPtr Pointer to the MCU fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_McuSendLive(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq) {

  fSerializeFrame(
    eFB_LINK_FRAME_TYPE_RESPONSE,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_MONITORING,
    (uint8_t)eFB_MCU_PROP_ID_MONITORING_LIVE,
    fMcuLiveGeneratePayload, NULL);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send MCU WhoAmI frame via faraabin link.
 * 
 * @param fobjectPtr Pointer to the MCU fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_McuSendWhoAmI(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq) {

  fSerializeFrame(
    eFB_LINK_FRAME_TYPE_RESPONSE,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_MONITORING,
    (uint8_t)eFB_MCU_PROP_ID_MONITORING_WHOAMI,
    fMcuWhoAmIGeneratePayload, NULL);
}

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
void fFaraabinLinkSerializer_McuSendProfiler(uint32_t fobjectPtr, uint8_t *fobjectSeq) {
	
	fSerializeFrame(
    eFB_LINK_FRAME_TYPE_EVENT,
    (fobjectSeq),
    (0),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_MONITORING,
    (uint8_t)eFB_MCU_PROP_ID_MONITORING_PROFILER,
    fMcuProfilerGeneratePayload, NULL);
}
#endif

/**
 * @brief This is a helper function from fSerializeFrame() to send state machine values via faraabin link.
 * 
 * @param fobjectPtr Pointer to the state machine fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 * @param isResponse Flag that indicates this frame is a response.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
void fFaraabinLinkSerializer_StateMachineSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse) {

  fSerializeFrame(
      (isResponse) ? eFB_LINK_FRAME_TYPE_RESPONSE : eFB_LINK_FRAME_TYPE_EVENT,
      (fobjectSeq), 
      (reqSeq),
      (true),
      (fobjectPtr),
      0,
      (uint8_t)eFB_PROP_GROUP_MONITORING,
      (uint8_t)eFB_SM_PROP_ID_MONITORING_VALUE,
      fStateMachineValueGeneratePayload, NULL);
}
#endif

/**
 * @brief This is a helper function from fSerializeFrame() to send state machine settings via faraabin link.
 * 
 * @param fobjectPtr Pointer to the state machine fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
void fFaraabinLinkSerializer_StateMachineSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeqPtr, uint8_t reqSeq) {

  fSerializeFrame(
    eFB_LINK_FRAME_TYPE_RESPONSE,
    (fobjectSeqPtr),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)eFB_PROP_GROUP_SETTING,
    (uint8_t)eFB_SM_PROP_ID_SETTING_ALL,
    fStateMachineSettingGeneratePayload, NULL);
}
#endif

/**
 * @brief This is a helper function from fSerializeFrame() to send user defined structure type members via faraabin link.
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
  uint8_t reqSeq) {
    
  sFaraabinFobjectVarType *me = (sFaraabinFobjectVarType*)structPtr;
  sDictStructMemberPayload payLoad;
  
  payLoad.VarPtr = varPtr;
  payLoad.VarTypePtr = varTypePtr;
  payLoad.VarArrayQty = varArrayQty;
  payLoad._isPtr = isPtr;
  payLoad.Name = name;
  
  fSerializeDict(structPtr, &me->Seq, reqSeq, fDictGeneratePayloadVarTypeStructMember, &payLoad);
  
}

/**
 * @brief This is a helper function from fSerializeFrame() to send user defined enumeration type members via faraabin link.
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
  uint8_t reqSeq) {
    
  sFaraabinFobjectVarType *me = (sFaraabinFobjectVarType*)enumPtr;
  sDictEnumMemberPayload payLoad;
  
  payLoad.Name = enumMemberName;
  payLoad.Id = enumMemberId;
  
  fSerializeDict(enumPtr, &me->Seq, reqSeq, fDictGeneratePayloadVarTypeEnumMember, &payLoad);
  
}

/**
 * @brief This is a helper function from fSerializeFrame() to send variable dictionary via faraabin link.
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
  uint8_t reqSeq) {
    
  sFaraabinFobjectMcu *me = (sFaraabinFobjectMcu*)fFaraabinFobjectMcu_GetFobject();
  sDictVarPayloadParam payLoad;
  
  payLoad.VarPtr = varPtr;
  payLoad.VarTypePtr = varTypePtr;
  payLoad.VarArrayQty = varArrayQty;
  payLoad.ExternalInterface = externalInterface;
  payLoad.AccessCallBack = accessCallback;  
  payLoad._isPtr = isPtr;
  payLoad.Name = name;
  payLoad.Path = path;
  CorrectPath_(payLoad.Path);
  payLoad.FileName = fileName;
  
  fSerializeDict(varPtr, &me->Seq, reqSeq, fDictGeneratePayloadVar, &payLoad);
  
}

/**
 * @brief This is a helper function from fSerializeFrame() to send function group dictionary via faraabin link.
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
  uint8_t reqSeq) {
    
  sFaraabinFobjectMcu *me = (sFaraabinFobjectMcu*)fFaraabinFobjectMcu_GetFobject();
  sDictFunctionGroupPayloadParam payLoad;
  
  payLoad.ObjectName = objectName;
  payLoad.Path = path;
  CorrectPath_(payLoad.Path);
  payLoad.FileName = fileName;
  payLoad.ObjectPtr = objectPtr;
  payLoad.GroupTypePtr = groupTypePtr;
  
  fSerializeDict((uint32_t)me, &me->Seq, reqSeq, fDictGeneratePayloadFunctionGroup, &payLoad);
  
}

/**
 * @brief This is a helper function from fSerializeFrame() to send function group member dictionary via faraabin link.
 * 
 * @param groupTypePtr Pointer to the function group type.
 * @param fn Pointer to the function fobject.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLinkSerializer_FunctionGroupTypeMemberDict(uint32_t groupTypePtr, void *fn, uint8_t reqSeq) {
  
  sFaraabinFobjectFunctionGroupType *me = (sFaraabinFobjectFunctionGroupType*)groupTypePtr;
  
  fSerializeDict(groupTypePtr, &me->Seq, reqSeq, fDictGeneratePayloadFunctionGroupTypeMember, fn);
  
}

/**
 * @brief Counts all of the frames for dictionries in database.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @return num Number of frames.
 */
uint32_t fFaraabinLinkSerializer_CountDictsFrames(uint32_t fobjectPtr) {
  
  fDictIteratorResetCounter(0);

  fDictIteratorEnableCounting();
  fGenerateDict(fobjectPtr, 0);

  fDictIteratorDisableCounting();

  return _serializer.DictIterator.TotalSubDicts;
}

/**
 * @brief Gets the RAM usage of the Faraabin link serializer.
 * 
 * @return usage The amount of bytes used by link serializer. 
 */
uint32_t fFaraabinLinkSerializer_GetRamUsage(void) {
	
	return sizeof(sSerializerInternal);
}

/*
===============================================================================
              ##### fb_link_serializer.c Private Functions #####
===============================================================================*/
/**
 * @brief Adds an unsigned 8-bit data to faraabin TX buffer.
 * 
 * @param d Value of data.
 */
static void fAddToBufferU8(uint8_t d) {
  
  uint8_t tmp = 0;

  _serializer.Serializer.CheckSum += d;

  if ((d == FB_EOF) || (d == FB_ESC)) { // If byte escaping is needed
    tmp = FB_ESC;
    fFaraabinLinkBuffer_Put_(&tmp, 1);
    tmp = d ^ FB_ESC_XOR;
    fFaraabinLinkBuffer_Put_(&tmp, 1);
  } else {
    tmp = d;
    fFaraabinLinkBuffer_Put_(&tmp, 1);
  }
}

/**
 * @brief Adds an unsigned 16-bit data to faraabin TX buffer.
 * 
 * @param d Value of data.
 */
static void fAddToBufferU16(uint16_t d) {
  
  uByte2 tmp;
  tmp.U16 = d;

  fAddToBufferU8(tmp.Byte[0]);
  fAddToBufferU8(tmp.Byte[1]);
}

/**
 * @brief Adds an unsigned 32-bit data to faraabin TX buffer.
 * 
 * @param d Value of data.
 */
static void fAddToBufferU32(uint32_t d) {
  
  uByte4 tmp;
  tmp.U32 = d;
  
  fAddToBufferU8(tmp.Byte[0]);
  fAddToBufferU8(tmp.Byte[1]);
  fAddToBufferU8(tmp.Byte[2]);
  fAddToBufferU8(tmp.Byte[3]);
}

/**
 * @brief Adds an unsigned 64-bit data to faraabin TX buffer.
 * 
 * @param d Value of data.
 */
static void fAddToBufferU64(uint64_t d) {
  
  uByte8 tmp;
  tmp.U64 = d;
  
  fAddToBufferU8(tmp.Byte[0]);
  fAddToBufferU8(tmp.Byte[1]);
  fAddToBufferU8(tmp.Byte[2]);
  fAddToBufferU8(tmp.Byte[3]);
  fAddToBufferU8(tmp.Byte[4]);
  fAddToBufferU8(tmp.Byte[5]);
  fAddToBufferU8(tmp.Byte[6]);
  fAddToBufferU8(tmp.Byte[7]);
}

#ifdef __FARAABIN_LINK_SERIALIZER_COMMENT_SECTION_1
/**
 * @brief Adds a float 32-bit data to faraabin TX buffer.
 * 
 * @note This function is reserved here for future use.
 * 
 * @param d Value of data.
 */
static void fAddToBufferF32(float32_t d) {
 
 uByte4 tmp;
 tmp.F32 = d;
 
 fAddToBufferU8(tmp.Byte[0]);
 fAddToBufferU8(tmp.Byte[1]);
 fAddToBufferU8(tmp.Byte[2]);
 fAddToBufferU8(tmp.Byte[3]);
}

/**
 * @brief Adds a float 64-bit data to faraabin TX buffer.
 * 
 * @note This function is reserved here for future use.
 * 
 * @param d Value of data.
 */
static void fAddToBufferF64(float64_t d) {
 
 uByte8 tmp;
 tmp.F64 = d;
 
 fAddToBufferU8(tmp.Byte[0]);
 fAddToBufferU8(tmp.Byte[1]);
 fAddToBufferU8(tmp.Byte[2]);
 fAddToBufferU8(tmp.Byte[3]);
 fAddToBufferU8(tmp.Byte[4]);
 fAddToBufferU8(tmp.Byte[5]);
 fAddToBufferU8(tmp.Byte[6]);
 fAddToBufferU8(tmp.Byte[7]);
}
#endif

/**
 * @brief Adds a string to faraabin TX buffer.
 * 
 * @note Size of string is measured using strlen() function.
 * 
 * @param string Value of data.
 */
static void fAddToBufferString(char *string) {
  
  fAddToBuffer((uint8_t*)string, strlen(string));
}

/**
 * @brief Adds data (in a byte wise manner) to TX buffer of faraabin.
 * 
 * @param data Pointer to the data.
 * @param size Size of data.
 */
static void fAddToBuffer(uint8_t *data, uint32_t size) {

  for(uint32_t i = 0; i < size; i++) {
    fAddToBufferU8(data[i]);
  }
}

/**
 * @brief Generates payload for reporting enable status of common fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fCommonEnableStatusGeneratePayload(uint32_t fobjectPtr, void *param) {

  UNUSED_(param);
  uint8_t *fobjectType = (uint8_t*)fobjectPtr;
  
  switch((eFaraabin_FobjectType)*fobjectType) {
    case eFO_TYPE_STATE_MACHINE: {
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
      fAddToBufferU8(((sStateMachine*)fobjectPtr)->_enable);
#else
			fAddToBufferU8(0);
#endif
      break;
    }

    case eFO_TYPE_STATE_MACHINE_STATE: {
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
      fAddToBufferU8(((sState*)fobjectPtr)->_enable);
#else
			fAddToBufferU8(0);
#endif
      break;
    }

    case eFO_TYPE_STATE_MACHINE_TRANSITION: {
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
      fAddToBufferU8(((sTransition*)fobjectPtr)->_enable);
#else
			fAddToBufferU8(0);
#endif
      break;
    }

    case eFO_TYPE_MCU: {
      fAddToBufferU8(((sFaraabinFobjectMcu*)fobjectPtr)->Enable);
      break;
    }

    case eFO_TYPE_EVENT_GROUP: {
      fAddToBufferU8(((sFaraabinFobjectEventGroup*)fobjectPtr)->Enable);
      break;
    }

    case eFO_TYPE_DATABUS: {
      fAddToBufferU8(((sFaraabinFobjectDataBus*)fobjectPtr)->Enable);
      break;
    }

    case eFO_TYPE_CODE_BLOCK: {
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
      fAddToBufferU8(((sCpuProcess*)fobjectPtr)->_enable);
#else
			fAddToBufferU8(0);
#endif
      break;
    }

    case eFO_TYPE_VAR_TYPE_ENUM_MEMBER:
    case eFO_TYPE_VAR_TYPE_STRUCT_MEMBER:
    case eFO_TYPE_ENTITY_NUMERICAL:
    case eFO_TYPE_ENTITY_EVENT:
    case eFO_TYPE_VAR:
    case eFO_TYPE_VAR_TYPE:
    case eFO_TYPE_CONTAINER:
    default: {
      // Do nothing.
      break;
    }
  }
}

/**
 * @brief Generates payload for sending data of common fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fCommonUserDataGeneratePayload(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);
  sUserDataParam *par = (sUserDataParam*)param;
  
  fAddToBuffer(par->UserData, par->UserDataSize);
}

/**
 * @brief Generates payload for codeblock fobjects .
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
static void fDictGeneratePayloadCodeBlock(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sCpuProcess *me = (sCpuProcess*)fobjectPtr;
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    
    fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->_path, strlen(me->_path));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->_fileName, strlen(me->_fileName));
    fAddToBufferU8('\0');
    
    return;
  }
  
  // Setting
  fCodeBlockSettingGeneratePayload(fobjectPtr, NULL);
  
  // Status
  
  // Dict
  fAddToBufferU32(fCpuProfiler_ProcessGetRamUsage(me));
	fAddToBufferU8(me->MaxSectionQty);
  fAddToBufferU8(me->SectionQty);
	fAddToBufferU8(me->_isCodeblock);
	fAddToBufferU8(me->Priority);
	
	fAddToBufferU32((uint32_t)&(me->CpuUsagePercent));
  
  fAddToBufferU32((uint32_t)&(me->RunCount));
  fAddToBufferU32((uint32_t)&(me->IntervalUs));
  fAddToBufferU32((uint32_t)&(me->IntervalMaximaUs));
  fAddToBufferU32((uint32_t)&(me->IntervalMinimaUs));
  fAddToBufferU32((uint32_t)&(me->IntervalErrorCnt));
  
  fAddToBufferU32((uint32_t)&(me->ExecuteTimeUs));
	fAddToBufferU32((uint32_t)&(me->ExecuteTimeNetUs));
  fAddToBufferU32((uint32_t)&(me->ExecuteMaximaUs));
  fAddToBufferU32((uint32_t)&(me->ExecuteMinimaUs));
  fAddToBufferU32((uint32_t)&(me->ExecuteErrorCnt));
	
	for(uint16_t i = 0; i < me->SectionQty; i++) {
    fAddToBufferU32((uint32_t)&(me->_pBufferSections[i]->ExecuteTimeUs));
		fAddToBufferU32((uint32_t)&(me->_pBufferSections[i]->ExecuteTimeNetUs));
		fAddToBufferU8(me->_pBufferSections[i]->InternalId);
  }
  
  fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->_path, strlen(me->_path));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->_fileName, strlen(me->_fileName));
  
  fAddToBufferU8((uint8_t)':');
  for(uint16_t i = 0; i < me->SectionQty; i++) {
    fAddToBuffer((uint8_t*)me->_pBufferSections[i]->Name, strlen(me->_pBufferSections[i]->Name));
    fAddToBufferU8((uint8_t)':');
  }
  
  fAddToBufferU8('\0');
}
#endif

/**
 * @brief Generates payload for sending values of codeblock fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
static void fCodeBlockValueGeneratePayload(uint32_t fobjectPtr, void *param) {
  
  sCpuProcess *me = (sCpuProcess*)fobjectPtr;
  
  fAddToBufferU32(me->IntervalUs);
  fAddToBufferU32(me->IntervalMaximaUs);
  fAddToBufferU32(me->IntervalMinimaUs);
  fAddToBufferU32(me->IntervalErrorCnt);

  fAddToBufferU32(me->ExecuteTimeUs);
	fAddToBufferU32(me->ExecuteTimeNetUs);
  fAddToBufferU32(me->ExecuteMaximaUs);   
  fAddToBufferU32(me->ExecuteMinimaUs);
  fAddToBufferU32(me->ExecuteErrorCnt);
  
  fAddToBufferU32(me->RunCount);
	
	fAddToBufferU8(me->SectionQty);
  for(uint16_t i = 0; i < me->SectionQty; i++) {
		
		if(me->_pBufferSectionsCall[i] != NULL) {
			
			fAddToBufferU8(me->_pBufferSectionsCall[i]->InternalId);
			fAddToBufferU32(me->_pBufferSectionsCall[i]->ExecuteTimeUs);
			fAddToBufferU32(me->_pBufferSectionsCall[i]->ExecuteTimeNetUs);
			
		} else {
			
			fAddToBufferU8(255);
			fAddToBufferU32(0);
			
		}
  }
}
#endif

/**
 * @brief Generates payload for sending settings of codeblock fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
static void fCodeBlockSettingGeneratePayload(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sCpuProcess *me = (sCpuProcess*)fobjectPtr;
  
  fAddToBufferU8(me->_enable);
    
  fAddToBufferU32(me->IntervalThresholdMaxUs);
  fAddToBufferU32(me->IntervalThresholdMinUs);
  fAddToBufferU32(me->ExecuteThresholdMaxUs);
  fAddToBufferU32(me->ExecuteThresholdMinUs);
  fAddToBufferU8(me->IsSendingEventsToDbStreamEnabled);
  fAddToBufferU8(me->ISSendingEventsToDbCaptureEnabled);
  fAddToBufferU8(me->IsSendingDataToDbStreamEnabled);
}
#endif

/**
 * @brief Generates payload for dictionary of databus fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDictGeneratePayloadDataBus(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectDataBus *me = (sFaraabinFobjectDataBus*)fobjectPtr;
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    
    fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Path, strlen(me->Path));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
    fAddToBufferU8('\0');
    
    return;
  }
  
  // Status
  
  // Setting
  fDataBusSettingGeneratePayload(fobjectPtr, NULL);
  
  // Dict
  fAddToBufferU32(sizeof(sFaraabinFobjectDataBus));
	fAddToBufferU32(sizeof(sFaraabinFobjectDataBus_Channel) * me->ChannelQty);
	fAddToBufferU32(sizeof(sFaraabinFobjectDataBus_CaptureValue) * me->BufferCaptureSize);
  
	fAddToBufferU32((uint32_t)&(me->CycleUs));
  fAddToBufferU32(me->BufferCaptureSize);
  fAddToBufferU16(me->ChannelQty);
  fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->Path, strlen(me->Path));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
  fAddToBufferU8('\0');
}

/**
 * @brief Generates payload for settings of databus fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDataBusSettingGeneratePayload(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectDataBus *me = (sFaraabinFobjectDataBus*)fobjectPtr;
  
  fAddToBufferU8(me->Enable);
  
  fAddToBufferU16(me->AttachedItemsQty);
  fAddToBufferU16(me->StreamDivideBy);
  fAddToBufferU32(me->BufferCaptureSize);
  fAddToBufferU16(me->TimerDivideBy);
  fAddToBufferU32(me->TimerWindowMs);
  fAddToBufferU16(me->TrigDivideBy);
  fAddToBufferU32(me->TimeAfterTrigMs);
  fAddToBufferU8(me->ApiTrigEnable);
  fAddToBufferU8(me->CurrentState);
  fAddToBufferU32(me->CycleUs);
  
  fAddToBufferU8(me->ChTrigEnable);
  fAddToBufferU16(me->ChTrigNo);
  fAddToBufferU8(me->ChTrigType);
  for(uint8_t i = 0; i < 8; i++) {
    fAddToBufferU8(me->ChTrigThreshold.Byte[i]);
  }
  
  fAddToBufferU16(fFaraabinFobjectDataBus_GetAttachCount(me));
    
  if(me->_init) {
    
    for(uint16_t i = 0; i<me->ChannelQty; i++) {
      
      if(me->_pBufferChannels[i].ItemFobjectPtr != 0U) {
        
        fAddToBufferU16(i);
        fAddToBufferU8(me->_pBufferChannels[i].ItemFobjectType);
        fAddToBufferU32(me->_pBufferChannels[i].ItemFobjectPtr);
        fAddToBufferU16(me->_pBufferChannels[i].ItemFobjectParam);
        fAddToBufferU8((uint8_t)me->_pBufferChannels[i].Enable);
        
      }
    }
  }
}

/**
 * @brief Generates payload for sending captured values of databus fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDataBusCaptureValueGeneratePayload(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);
  sFaraabinFobjectDataBus_CaptureValue *value = param;
  
  fAddToBufferU8(value->FobjectType);
  fAddToBufferU32(value->FobjectPtr);
  fAddToBufferU32(value->CapturedTimeStamp);
  fAddToBufferU64(value->CapturedValue);
}

/**
 * @brief Generates payload for sending stream values of databus fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDataBusValueGeneratePayload(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectDataBus *me = (sFaraabinFobjectDataBus*)fobjectPtr;
  
  for(uint16_t i = 0; i < me->ChannelQty; i++) {

    if(me->_pBufferChannels[i].ItemFobjectPtr == 0U) {
      continue;
    }

    if(!me->_pBufferChannels[i].Enable) {
      continue;
    }
      
    switch(me->_pBufferChannels[i].ItemFobjectType) {
      
      case eFO_TYPE_VAR:
      case eFO_TYPE_ENTITY_NUMERICAL: {
        
        fAddToBufferU8(me->_pBufferChannels[i].ItemFobjectType);
        fAddToBufferU32(me->_pBufferChannels[i].ItemFobjectPtr);
        
        fAddToBufferU16(me->_pBufferChannels[i].ItemFobjectParam);
        fAddToBuffer((uint8_t*)me->_pBufferChannels[i].ItemFobjectPtr, me->_pBufferChannels[i].ItemFobjectParam);
    
        break;
      }
      
			#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
      case eFO_TYPE_CODE_BLOCK: {

        sCpuProcess *cb = (sCpuProcess*)me->_pBufferChannels[i].ItemFobjectPtr;

        if(cb->IsSendingDataToDbStreamEnabled && cb->_init) {

          fAddToBufferU8(me->_pBufferChannels[i].ItemFobjectType);
          fAddToBufferU32(me->_pBufferChannels[i].ItemFobjectPtr);
					
					fAddToBufferU8(cb->SectionQty);
          
          fAddToBufferU32(cb->IntervalUs);
          fAddToBufferU32(cb->IntervalMaximaUs);
          fAddToBufferU32(cb->IntervalMinimaUs);
          fAddToBufferU32(cb->IntervalErrorCnt);
          
          fAddToBufferU32(cb->ExecuteTimeUs);
					fAddToBufferU32(cb->ExecuteTimeNetUs);
          fAddToBufferU32(cb->ExecuteMaximaUs);   
          fAddToBufferU32(cb->ExecuteMinimaUs); 
          fAddToBufferU32(cb->ExecuteErrorCnt);
          
          fAddToBufferU32(cb->RunCount);
          
          for(uint16_t j = 0; j < cb->SectionQty; j++) {
						
						if(cb->_pBufferSectionsCall[j] != NULL) {
							
							fAddToBufferU8(cb->_pBufferSectionsCall[j]->InternalId);
							fAddToBufferU32(cb->_pBufferSectionsCall[j]->ExecuteTimeUs);
							fAddToBufferU32(cb->_pBufferSectionsCall[j]->ExecuteTimeNetUs);
							
						} else {
							
							fAddToBufferU8(255);
							fAddToBufferU32(0);
							fAddToBufferU32(0);
							
						}
					}
        }
        
        break;
      }
			#endif

      default: {

        // Do nothing.
        break;
      }
    }
  }
}

/**
 * @brief Generates payload for sending dictionary of eventgroup fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDictGeneratePayloadEventGroup(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectEventGroup *me = (sFaraabinFobjectEventGroup*)fobjectPtr;
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    
    fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Path, strlen(me->Path));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
    fAddToBufferU8('\0');
    
    return;
  }
  
  // Setting
  fAddToBufferU8(me->Enable);
  
  // Status
  
  // Dict
  fAddToBufferU32(sizeof(sFaraabinFobjectEventGroup));
  
  fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->Path, strlen(me->Path));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
  fAddToBufferU8('\0');
}

/**
 * @brief Generates payload for sending dictionary of fobject container.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDictGeneratePayloadContainer(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectContainer *me = (sFaraabinFobjectContainer*)fobjectPtr;
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    
    fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Path, strlen(me->Path));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
    fAddToBufferU8('\0');
    
    return;
  }
  
  // Setting
  
  // Status
  
  // Dict
  fAddToBufferU32(sizeof(sFaraabinFobjectContainer));
  
  fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->Path, strlen(me->Path));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
  fAddToBufferU8('\0');
}

/**
 * @brief Generates payload for sending dictionary of MCU fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDictGeneratePayloadMcu(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectMcu *me = (sFaraabinFobjectMcu*)fobjectPtr;
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    
    fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Path, strlen(me->Path));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
    fAddToBufferU8('\0');
    
    return;
  }
  
  // Setting
  fAddToBufferU8(me->Enable);
  
  // Status
  uint16_t runningFunctions = fFaraabinFunctionEngine_GetQtyOfRunningFunctions();
  fAddToBufferU16(runningFunctions);
  for(uint16_t i = 0; i < runningFunctions; i++) {
    
    uint32_t ptr = 0U;
    uint32_t objectPtr = 0U;
    if(fFaraabinFunctionEngine_GetPtrsOfRunningFunction(i, &ptr, &objectPtr) != 0) {
      // |Not going to happen.
    }
    
    fAddToBufferU32(ptr);
    fAddToBufferU32(objectPtr);
  }
  
  // Dict
  fAddToBufferU32(sizeof(sFaraabinFobjectMcu));
	fAddToBufferU32(fFaraabinFunctionEngine_GetRamUsage());
	fAddToBufferU32(fFaraabinLinkBuffer_GetRamUsage());
	fAddToBufferU32(fFaraabinLinkHandler_GetRamUsage());
	fAddToBufferU32(fFaraabinLinkSerializer_GetRamUsage());
	fAddToBufferU32(fFaraabinDatabase_GetRamUsage());
}

/**
 * @brief Generates payload for sending ping results of MCU fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fMcuPingGeneratePayload(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);

  uint8_t *param_ptr = (uint8_t*)param;

  for(uint16_t i = 0; i < 1U; i++) {
    fAddToBufferU8(param_ptr[i]);
  }
}

/**
 * @brief Generates payload for sending MCU Live frame.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fMcuLiveGeneratePayload(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sFaraabinFobjectMcu* mcuFobject = fFaraabinFobjectMcu_GetFobject();

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
	sCpuProfiler_StatusFlag status = fCpuProfiler_GetStatusFlag();
	FaraabinFlags.Status.Bitfield.ProfilerListOvf = status.ProcessListOvf;
	FaraabinFlags.Status.Bitfield.ProfilerDepthOvf = status.DepthOvf;
	FaraabinFlags.Status.Bitfield.ProfilerDuplicate = status.DuplicateProcess;
#endif
	
  fAddToBufferU32(fFaraabinDatabase_GetAllStatusFlags());
	fAddToBufferU16(mcuFobject->SessionId);
  
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
	fAddToBufferU32((uint32_t)(fCpuProfiler_GetCpuUsage() * 100)); //Cpu usage
#else
	fAddToBufferU32(0); //Cpu usage
#endif
  fAddToBufferU32((uint32_t)(fChrono_GetContinuousTickMs() / 1000U)); //Up time
}

/**
 * @brief Generates payload for sending MCU Live frame.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
static void fMcuProfilerGeneratePayload(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  fAddToBufferU16((uint16_t)(fCpuProfiler_GetCpuUsage() * 100));
	
	sCpuProcess** processes = fCpuProfiler_GetProcessList();
	
	fAddToBufferU16(CPU_PROFILER_MAX_PROCESS);
	for(uint16_t i=0; i < CPU_PROFILER_MAX_PROCESS; i++) {
		
		if(processes[i] != NULL) {
			
			fAddToBufferU32((uint32_t)(processes[i]));
			fAddToBufferU16((uint16_t)(processes[i]->CpuUsagePercent * 100));
			
		} else {
			
			fAddToBufferU32(0);
			fAddToBufferU16(0);
			
		}
	}
}
#endif

/**
 * @brief Generates payload for sending WhoAmI frame.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fMcuWhoAmIGeneratePayload(uint32_t fobjectPtr, void *param) {

  UNUSED_(param);
  UNUSED_(fobjectPtr);
  
  sFaraabinFobjectMcu* mcuFobject = fFaraabinFobjectMcu_GetFobject();
  bool isBigEndian = false;
  
  uByte2 tmp;
  
  tmp.U16 = 0x0001U;
  if(tmp.Byte[0] == 0x00U) {
    isBigEndian =  true;
  } else {
    isBigEndian =  false;
  }
  
  fAddToBufferU8((uint8_t)isBigEndian);
	fAddToBufferU8(0/*Version Major*/);
	fAddToBufferU8(0/*Version Minor*/);
  fAddToBufferU32(fFaraabinDatabase_GetAllFeatureFlags());
  fAddToBufferU32(fFaraabinDatabase_GetNumberOfAddedDicts());
  fAddToBufferU32(fCountAllDictsFrames());
	fAddToBufferU32(fFaraabin_GetRxBufferSize());
	fAddToBufferU32(fChrono_GetTickToNsCoef());
  fAddToBufferU32(fChrono_GetTickTopValue());
	fAddToBufferU32(mcuFobject->BootTimeMs);
  fAddToBufferU16(strlen(fFaraabin_GetFirmwareName()));
  fAddToBufferString(fFaraabin_GetFirmwareName());
  fAddToBufferU16(strlen(fFaraabin_GetFirmwareInfo()));
  fAddToBufferString(fFaraabin_GetFirmwareInfo());
  fAddToBufferU8('\0');
}

/**
 * @brief Generates payload for sending dictionary of state machine fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fDictGeneratePayloadStateMachine(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sStateMachine *me = (sStateMachine*)fobjectPtr;
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    
    fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->_path, strlen(me->_path));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->_fileName, strlen(me->_fileName));
    fAddToBufferU8('\0');
    
    return;
  }
  
  // Setting
  fStateMachineSettingGeneratePayload(fobjectPtr, NULL);
  
  // Status
  fAddToBufferU32((uint32_t)me->_currentState);
  fAddToBufferU32((uint32_t)me->_lastTransitionOccurred);
  
  // Dict
  fAddToBufferU32(fStateMachine_GetRamUsage(me));
  
  fAddToBufferU8((uint8_t)me->_isHierarchical);
  
  fAddToBufferU16(me->_maxStateQty);
  fAddToBufferU16(me->_stateQty);
  fAddToBufferU32((uint32_t)me->_firstState);
  
  fAddToBufferU16(me->_maxTransitionQty);
  fAddToBufferU16(me->_transitionQty);
  
  fAddToBufferU16(me->_maxDepth);
  
  fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->_path, strlen(me->_path));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->_fileName, strlen(me->_fileName));
  fAddToBufferU8('\0');
}
#endif

/**
 * @brief Generates payload for sending dictionary of state machine's states.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fDictGeneratePayloadStateMachineState(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sState *me = (sState*)fobjectPtr;
  
  fAddToBufferU8(eFO_TYPE_STATE_MACHINE_STATE | 0x80U);
  
  // Setting
  fAddToBufferU8(me->_enable);
  fAddToBufferU8(me->AllTransitionEnable);
  
  // Status
  
  // Dict
  fAddToBufferU32(sizeof(sState)); 
  
  fAddToBufferU16(me->_stateId);
  fAddToBufferU8(me->_depth);
  fAddToBufferU8((uint8_t)me->_isFirstChild);
  fAddToBufferU32((uint32_t)me->_stateMachine);
  fAddToBufferU32((uint32_t)me->_parent);
  fAddToBufferU32((uint32_t)me->_firstChild);
  
  fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
  fAddToBufferU8('\0');
}
#endif

/**
 * @brief Generates payload for sending dictionary of state machine's transitions.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fDictGeneratePayloadStateMachineTransition(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sTransition *me = (sTransition*)fobjectPtr;
  sState *srcState = (sState*)me->_srcState;
  
  fAddToBufferU8(eFO_TYPE_STATE_MACHINE_TRANSITION | 0x80U);
  
  // Setting
  fAddToBufferU8(me->_enable);
  
  // Status
  
  // Dict
  fAddToBufferU32(sizeof(sTransition));
  
  fAddToBufferU16(me->_transitionId);
  fAddToBufferU32((uint32_t)srcState->_stateMachine);
  fAddToBufferU32((uint32_t)me->_srcState);
  fAddToBufferU32((uint32_t)me->_dstState);
  
  fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
  fAddToBufferU8('\0');
}
#endif

/**
 * @brief Generates payload for sending values of state machine fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fStateMachineValueGeneratePayload(uint32_t fobjectPtr, void *param) {

  UNUSED_(param);
  
  sStateMachine *me = (sStateMachine*)fobjectPtr;
  
  fAddToBufferU32((uint32_t)me->_currentState);
  fAddToBufferU32((uint32_t)me->_lastTransitionOccurred);
}
#endif

/**
 * @brief Generates payload for sending settings of state machine fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fStateMachineSettingGeneratePayload(uint32_t fobjectPtr, void *param) {

  UNUSED_(param);
  
  sStateMachine *me = (sStateMachine*)fobjectPtr;
  
  fAddToBufferU8(me->_enable);
  
  fAddToBufferU8(me->AllTransitionEnable);
  
  fAddToBufferU16(me->_stateQty);
  for(uint16_t i = 0; i < me->_stateQty; i++) {
    
    sState *state = (sState*)me->_pBufferListState[i];
    
    fAddToBufferU8(state->_enable);
    fAddToBufferU8(state->AllTransitionEnable);
  }
  
  fAddToBufferU16(me->_transitionQty);
  for(uint16_t i = 0; i < me->_transitionQty; i++) {
    
    sTransition *tran = (sTransition*)me->_pBufferListTransition[i];
    
    fAddToBufferU8(tran->_enable);
  }
}
#endif

/**
 * @brief Generates payload for sending values of variable fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fVarValueGeneratePayload(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);

  sVarSendParam *par = (sVarSendParam*)param;
  
  fAddToBufferU32(par->VarPtr);
  fAddToBuffer((uint8_t*)par->DataPtr, par->VarSize);
}

/**
 * @brief Generates payload for sending dictionary of variable fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDictGeneratePayloadVar(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);

  sDictVarPayloadParam *par = (sDictVarPayloadParam*)param;
  
  fAddToBufferU8(eFO_TYPE_VAR | 0x80U);
  
  fAddToBufferU32(par->VarPtr);
  fAddToBufferU32(par->VarTypePtr);
  fAddToBufferU32(par->VarArrayQty);
  
  uint8_t control = 0x00U;
  if(par->ExternalInterface != 0U) {
    control |= 0x01U;
  }
  if(par->AccessCallBack != 0U) {
    control |= 0x02U;
  }
  if(par->_isPtr) {
    control |= 0x04U;
  }
  fAddToBufferU8(control);
  
  if(par->ExternalInterface != 0U) {
    fAddToBufferU32(par->ExternalInterface);
  }
  if(par->AccessCallBack != 0U) {
    fAddToBufferU32(par->AccessCallBack);
  }
  
  fAddToBuffer((uint8_t*)par->Name, strlen(par->Name));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)par->Path, strlen(par->Path));
  fAddToBufferU8('\0');
}

/**
 * @brief Generates payload for sending dictionary of varType fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDictGeneratePayloadVarType(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sFaraabinFobjectVarType *me = (sFaraabinFobjectVarType*)fobjectPtr;
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    
    fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Path, strlen(me->Path));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
    fAddToBufferU8('\0');
    
    return;
  }

  // Setting

  // Status

  // Dict
  fAddToBufferU32(sizeof(sFaraabinFobjectVarType));
  
  fAddToBufferU8((uint8_t)me->DataType);
  fAddToBufferU32(me->Size);
  fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->Path, strlen(me->Path));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
  fAddToBufferU8('\0');
}

/**
 * @brief Generates payload for sending dictionary of structure members.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDictGeneratePayloadVarTypeStructMember(uint32_t fobjectPtr, void *param) {

  UNUSED_(fobjectPtr);
  
  sDictStructMemberPayload *par = (sDictStructMemberPayload*)param;
  
  fAddToBufferU8(eFO_TYPE_VAR_TYPE_STRUCT_MEMBER | 0x80U);
  
  fAddToBufferU32(par->VarPtr);
  fAddToBufferU32(par->VarTypePtr);
  fAddToBufferU32(par->VarArrayQty);
  fAddToBufferU8((uint8_t)par->_isPtr);
  fAddToBuffer((uint8_t*)par->Name, strlen(par->Name));
  fAddToBufferU8('\0');
}

/**
 * @brief Generates payload for sending dictionary of enum members.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDictGeneratePayloadVarTypeEnumMember(uint32_t fobjectPtr, void *param) {

  UNUSED_(fobjectPtr);
  
  sDictEnumMemberPayload *par = (sDictEnumMemberPayload*)param;
  
  fAddToBufferU8(eFO_TYPE_VAR_TYPE_ENUM_MEMBER | 0x80U);
  
  fAddToBufferU16(par->Id);
  fAddToBuffer((uint8_t*)par->Name, strlen(par->Name));
  fAddToBufferU8('\0');
}

/**
 * @brief Generates payload for sending index and total number of faraabin dictionary.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDictGeneratePayloadIterator(uint32_t fobjectPtr, void *param) {

  fAddToBufferU16(_serializer.DictIterator.CurrentDictIndex);
  fAddToBufferU16(_serializer.DictIterator.TotalSubDicts);
  fAddToBufferU16(_serializer.DictIterator.CurrentSubDictIndex);
  
  sGeneratePayloadDict_Param *par = (sGeneratePayloadDict_Param*)param;
  if(par->fpGenerateDictPayload != NULL) {
    par->fpGenerateDictPayload(fobjectPtr, par->pDictPayloadParam);
  }
}

/**
 * @brief Resets Serializer frame for new frame generation.
 * 
 */
static void fFrameStart(void) {
  _serializer.Serializer.CheckSum = 0U;
}

/**
 * @brief Completes serializer frame after adding all data and payloads.
 * 
 */
static void fFrameEnd(void) {
  uint8_t tmp = 0;
  _serializer.Serializer.CheckSum ^= (uint8_t)0xFFU;   /* invert the bits in the checksum */

  if ((_serializer.Serializer.CheckSum == FB_EOF) || (_serializer.Serializer.CheckSum == FB_ESC)) { // If byte escaping is needed
    tmp = FB_ESC;
    fFaraabinLinkBuffer_Put_(&tmp, 1);
    tmp = _serializer.Serializer.CheckSum ^ FB_ESC_XOR;
    fFaraabinLinkBuffer_Put_(&tmp, 1);
  } else {
    tmp = _serializer.Serializer.CheckSum;
    fFaraabinLinkBuffer_Put_(&tmp, 1);
  }

  tmp = FB_EOF;
  fFaraabinLinkBuffer_Put_(&tmp, 1);

}

/**
 * @brief 
 * 
 * @param fobjectPtr 
 * @param param 
 */
static void fSerializePayload_Event(uint32_t fobjectPtr, void *param) {
  
  sEventParam *par = (sEventParam*)param;
  
  fAddToBufferU8((uint8_t)par->Severity);
  fAddToBufferU16(par->EventId);
  if(par->pParam != NULL) {
    fAddToBuffer(par->pParam, par->ParamSize);
  }
  
  if(par->fpGeneratePayload != NULL) {
    par->fpGeneratePayload(fobjectPtr, par->pPayloadParam);
  }
}

/**
 * @brief Iterates over dictionaries in database.
 * 
 */
static void fDictIteratorIterate(void) {

  if(_serializer.DictIterator._isInCountingMode) {
    _serializer.DictIterator.TotalSubDicts++;
  } else {
    _serializer.DictIterator.CurrentSubDictIndex++;
  }
}

/**
 * @brief Resets the internal counter of DictIterator to start over.
 * 
 * @param dictIndex Current dictionary index.
 */
static void fDictIteratorResetCounter(uint16_t dictIndex) {
  _serializer.DictIterator.CurrentDictIndex = dictIndex;
  _serializer.DictIterator.CurrentSubDictIndex = 0U;
  _serializer.DictIterator.TotalSubDicts = 0U;
}

/**
 * @brief Puts dict iterator in counting mode.
 * 
 */
static void fDictIteratorEnableCounting(void) {
  _serializer.DictIterator._isInCountingMode = true;
}

/**
 * @brief Puts dict iterator in sending mode.
 * 
 */
static void fDictIteratorDisableCounting(void) {
  _serializer.DictIterator._isInCountingMode = false;
}

/**
 * @brief Returns current mode of the dict iterator.
 * 
 * @return IsInCountingMode Mode of the dict iterator.
 */
static bool fDictIteratorIsInCountingMode(void) {
  return _serializer.DictIterator._isInCountingMode;
}

/**
 * @brief This is a helper function from fSerializeFrame() to send each dictionary in database via faraabin link.
 * 
 * @note This function can be used to count all dictionaries in database (by putting DictIterator in counting mode) or send dictionaries one by one.
 * 
 * @param fobjectPtr Pointer of the fobject.
 * @param fobjectSeqPtr Pointer of the fobject sequence counter.
 * @param reqSeq Request sequence counter.
 * @param fpGenerateDictPayload Pointer to the function that generates payload of the corresponding fobject.
 * @param dictPayloadParam Pointer to the payload parameters that will be passed to GenerateDictPayload function.
 */
static void fSerializeDict(
  uint32_t fobjectPtr,
  uint8_t *fobjectSeqPtr,
  uint8_t reqSeq,
  void(*fpGenerateDictPayload)(uint32_t, void*),
  void *dictPayloadParam) {

  if(!fDictIteratorIsInCountingMode()) {
    sGeneratePayloadDict_Param param;

    param.fpGenerateDictPayload = fpGenerateDictPayload;
    param.pDictPayloadParam = dictPayloadParam;

    fSerializeFrame(  
      eFB_LINK_FRAME_TYPE_RESPONSE,
      fobjectSeqPtr,
      reqSeq,
      false,
      fobjectPtr,
      0,
      (uint8_t)eFB_PROP_GROUP_DICT,
      (uint8_t)FB_COMMON_PROP_ID_DICT,
      fDictGeneratePayloadIterator, &param);
      
    fFaraabinLinkHandler_FlushBuffer();
  }

  fDictIteratorIterate();

}

/**
 * @brief Generates dictionary for the fobjects based on their type.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param reqSeq Request sequence counter.
 */
static void fGenerateDict(uint32_t fobjectPtr, uint8_t reqSeq) {
  uint8_t *fobjectType = (uint8_t*)fobjectPtr;
  
  switch((eFaraabin_FobjectType)(*fobjectType)) {
    
    case eFO_TYPE_MCU: {
      sFaraabinFobjectMcu *me = (sFaraabinFobjectMcu*)fobjectPtr;

      CorrectPath_(me->Path);

      fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fDictGeneratePayloadMcu, NULL);
      
      break;
    }
    
    case eFO_TYPE_VAR_TYPE: {
      sFaraabinFobjectVarType *me = (sFaraabinFobjectVarType*)fobjectPtr;

      CorrectPath_(me->Path);
  
      switch(me->DataType) {
        
        case eVAR_DATA_TYPE_PRIMITIVE: {

          fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fDictGeneratePayloadVarType, NULL);
          
          break;
        }
        
        case eVAR_DATA_TYPE_USER_DEFINED_ENUM:
        case eVAR_DATA_TYPE_USER_DEFINED_STRUCT:
        case eVAR_DATA_TYPE_USER_DEFINED_UNION: {

          fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fDictGeneratePayloadVarType, NULL);
          
          if(me->fpSendMember != 0U) {
            me->fpSendMember(fobjectPtr, reqSeq);
          }
          
          break;
        }

        default: {

          // Do nothing.
          break;
        }
      }
      
      break;
    }
    
    case eFO_TYPE_FUNCTION_GROUP_TYPE: {
      sFaraabinFobjectFunctionGroupType *me = (sFaraabinFobjectFunctionGroupType*)fobjectPtr;

      fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fDictGeneratePayloadFunctionGroupType, NULL);
          
      if(me->fpSendMember != 0U) {
        me->fpSendMember(fobjectPtr, reqSeq);
      }
      
      break;
    }
    
    case eFO_TYPE_EVENT_GROUP: {
      sFaraabinFobjectEventGroup *me = (sFaraabinFobjectEventGroup*)fobjectPtr;

      CorrectPath_(me->Path);

      fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fDictGeneratePayloadEventGroup, NULL);
      
      break;
    }
    
    case eFO_TYPE_CONTAINER: {
      sFaraabinFobjectContainer *me = (sFaraabinFobjectContainer*)fobjectPtr;
      
      fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fDictGeneratePayloadContainer, NULL);
      
      if(me->fpContainerFunc != NULL) {
        me->fpContainerFunc(reqSeq);
      }

      break;
    }
    
    case eFO_TYPE_DATABUS: {
      sFaraabinFobjectDataBus *me = (sFaraabinFobjectDataBus*)fobjectPtr;

      CorrectPath_(me->Path);

      fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fDictGeneratePayloadDataBus, NULL);
      
      break;
    }
    
		#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
    case eFO_TYPE_CODE_BLOCK: {
      sCpuProcess *me = (sCpuProcess*)fobjectPtr;

      CorrectPath_(me->_path);

      fSerializeDict(fobjectPtr, &me->_seq, reqSeq, fDictGeneratePayloadCodeBlock, NULL);
      
      break;
    }
		#endif
    
		#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
    case eFO_TYPE_STATE_MACHINE: {
      sStateMachine *me = (sStateMachine*)fobjectPtr;

      CorrectPath_(me->_path);
  
      fSerializeDict(fobjectPtr, &me->_seq, reqSeq, fDictGeneratePayloadStateMachine, NULL);
      
      for(uint16_t i = 0; i < me->_stateQty; i++) {
        
        if(me->_pBufferListState[i] != NULL) {
          sState *state = (sState*)me->_pBufferListState[i];
          fSerializeDict((uint32_t)state, &me->_seq, reqSeq, fDictGeneratePayloadStateMachineState, NULL);
        }
      }
      
      for(uint16_t i = 0; i < me->_transitionQty; i++) {
        
        sTransition *tran = (sTransition*)me->_pBufferListTransition[i];
        
        if(tran != NULL) {
          
          fSerializeDict((uint32_t)tran, &me->_seq, reqSeq, fDictGeneratePayloadStateMachineTransition, NULL);

        }
      }
      
      break;
    }
		#endif
    
    default: {
      
      // Do nothing.
      break;
    }
  }
}

/**
 * @brief Counts dictionary frames by setting DictIterator in counting mode.
 * 
 * @return uint32_t Number of frames for all of the dictionaries in database.
 */
static uint32_t fCountAllDictsFrames(void) {
  
  uint16_t dicts = fFaraabinDatabase_GetNumberOfAddedDicts();
  
  fDictIteratorResetCounter(0);

  for(uint16_t i = 0; i < dicts; i++) {
    fDictIteratorEnableCounting();
    fGenerateDict(fFaraabinDatabase_GetFobjectPointerFromDict(i), 0);
  }

  fDictIteratorDisableCounting();

  return _serializer.DictIterator.TotalSubDicts; // It is counted for all dicts, hence all frame counts
}

/**
 * @brief Generates dictionary payload for function groups.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fDictGeneratePayloadFunctionGroupType(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sFaraabinFobjectFunctionGroupType *me = (sFaraabinFobjectFunctionGroupType*)fobjectPtr;
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    
    fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)"type", strlen("type"));
    fAddToBufferU8((uint8_t)':');
    fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
    fAddToBufferU8('\0');
    
    return;
  }

  // Setting

  // Status

  // Dict
  fAddToBufferU32(sizeof(sFaraabinFobjectFunctionGroupType));
  
  fAddToBuffer((uint8_t*)me->Name, strlen(me->Name));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->Filename, strlen(me->Filename));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)me->Help, strlen(me->Help));
  fAddToBufferU8('\0');
}

/**
 * @brief Generates dictionary payload for function group members.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the payload parameters.
 */
static void fDictGeneratePayloadFunctionGroupTypeMember(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);
  
  sFaraabinFobjectFunction *par = (sFaraabinFobjectFunction*)param;
  
  fAddToBufferU8(eFO_TYPE_FUNCTION_GROUP_TYPE_MEMBER | 0x80U);
  fAddToBufferU32(sizeof(sFaraabinFobjectFunction));
  
  fAddToBufferU32((uint32_t)par);
  fAddToBuffer((uint8_t*)par->Name, strlen(par->Name));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)par->Help, strlen(par->Help));
  fAddToBufferU8('\0');
}

/**
 * @brief Generates dictionary payload for function group.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the payload parameters.
 */
static void fDictGeneratePayloadFunctionGroup(uint32_t fobjectPtr, void *param) {
  
  sDictFunctionGroupPayloadParam *par = (sDictFunctionGroupPayloadParam*)param;
  
  fAddToBufferU8(eFO_TYPE_FUNCTION_GROUP | 0x80U);
  
  fAddToBufferU32(par->ObjectPtr);
  fAddToBufferU32(par->GroupTypePtr);
  fAddToBuffer((uint8_t*)par->ObjectName, strlen(par->ObjectName));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)par->Path, strlen(par->Path));
  fAddToBufferU8((uint8_t)':');
  fAddToBuffer((uint8_t*)par->FileName, strlen(par->FileName));
  fAddToBufferU8('\0');
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
