/**
 ******************************************************************************
 * @file           : faraabin.c
 * @brief          : Faraabin library.
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
 @mainpage
 
 @mainpage
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin.h"

#ifdef FARAABIN_ENABLE

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER 
#include "add_on/cpu_profiler/faraabin_addon_cpu_profiler.h"
#endif //FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER

#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
#include "add_on/state_machine/faraabin_addon_state_machine.h"
#endif //FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE

#ifdef FB_ADD_ON_FEATURE_FLAG_UNITY
#include "add_on/unity/faraabin_addon_unity.h"
#endif //FB_ADD_ON_FEATURE_FLAG_UNITY

#include <stdarg.h>
#include <stdint.h>

/* Private define ------------------------------------------------------------*/
#define VERSION_MAJOR 2
#define VERSION_MINOR 2

#if FB_MAX_FOBJECT_QTY >= 65535
  #error "Maximum Fobjects quantity is 65534"
#endif

#if FB_MAX_CONCURRENT_FUNCTION >= 256
  #error "Maximum concurrent functions is 255"
#endif

/**
 * @brief Definition for link frame.
 * 
 */
#define MINIMUM_FRAME_SIZE  ((uint8_t)0x07U)

#define CMD_BUFFER_SIZE     ((uint16_t)200U)

#define HUB_FRAME_MAX_QTY   ((uint16_t)500U)

#define CLIENT_FRAME_CONTROL_OFFSET   0

#define CLIENT_FRAME_PROPERTY_OFFSET  1

#define CLIENT_FRAME_POINTER_OFFSET   2

#define CLIENT_FRAME_PAYLOAD_OFFSET   6

static const uint8_t HUB_EOF_PATTERN[] = {0x01, 0xFD, 0xFD, 0xFE, 0xFE, 0x01};

static const uint8_t EOF_PATTERN[] = {0x02, 0xDF, 0xDF, 0xEF, 0xEF, 0x02};

/**
 * @brief Serializer ID for common property.
 * 
 */
#define FB_COMMON_PROP_ID_DICT  ((uint8_t)0U)

/**
 * @brief MCU Fobject system event types definition.
 * 
 */
#define MCU_EVENT_INFO_USER_DATA_RECEIVED                     ((uint8_t)0x00U)
  
#define MCU_EVENT_INFO_DICT_END                               ((uint8_t)0x01U)
#define MCU_EVENT_INFO_CLEAR_FLAG_BUFFER_OVF                  ((uint8_t)0x02U)

#define MCU_EVENT_INFO_BOOT                                   ((uint8_t)0x03U)

#define MCU_EVENT_ERROR_RX_FRAME_BIG_SIZE                     ((uint8_t)0x04U)
#define MCU_EVENT_ERROR_RX_FRAME_BEFORE_END_OF_PREVIOUS_FRAME ((uint8_t)0x05U)
#define MCU_EVENT_ERROR_RX_FRAME_CHECKSUM                     ((uint8_t)0x07U)
#define MCU_EVENT_ERROR_RX_FRAME_SMALL_SIZE                   ((uint8_t)0x08U)
#define MCU_EVENT_ERROR_TX_FRAME_TIMEOUT                      ((uint8_t)0x09U)
#define MCU_EVENT_ERROR_TX_FRAME_SEND                         ((uint8_t)0x0AU)
#define MCU_EVENT_ERROR_MAX_PRINTF_REENTRANT                  ((uint8_t)0x0BU)
#define MCU_EVENT_ERROR_UNDEF                                 ((uint8_t)0x0CU)

#define MCU_EVENT_ERROR_UNSUPPORTED_FOBJECT_TYPE_DICT         ((uint8_t)0x0DU)
#define MCU_EVENT_ERROR_DICT_INDEX_OVERFLOW                   ((uint8_t)0x0EU)

#define MCU_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY          ((uint8_t)0x0FU)
#define MCU_EVENT_ERROR_UNSUPPORTED_FOBJECT_TYPE              ((uint8_t)0x10U)

#define MCU_EVENT_ERROR_PASSWORD                              ((uint8_t)0x11U)

#define MCU_EVENT_ERROR_RESET_FUNC_NOT_IMPLEMENTED            ((uint8_t)0x12U)

/**
 * @brief Frame type identifier of link serializer.
 * 
 */
#define FRAME_TYPE_EVENT     ((uint8_t)0x00U)
#define FRAME_TYPE_RESPONSE  ((uint8_t)0x01U)

/**
 * @brief Access type of the client frame.
 * 
 */
#define CLIENT_FRAME_ACCESS_TYPE_READ   ((uint8_t)0x00U)
#define CLIENT_FRAME_ACCESS_TYPE_WRITE  ((uint8_t)0x01U)

/* Private macro -------------------------------------------------------------*/
/**
 * @brief Macro's for get field of FobjectProperty.
 * 
 */
#define ClientFrame_PropGroup_(byte_)  (uint8_t)(((byte_) >> ((uint8_t)5U)) & ((uint8_t)0x07U))
#define ClientFrame_PropId_(byte_)     (uint8_t)((byte_) & ((uint8_t)0x1FU))

  /**
 * @brief Macro's for get field of Control byte.
 * 
 */
#define ClientFrame_IsBufferCmdResponse_(byte_)      ((uint8_t)(((byte_) >> ((uint8_t)6U)) & ((uint8_t)0x01U)))
#define ClientFrame_FlushBuffer_(byte_)              ((uint8_t)(((byte_) >> ((uint8_t)5U)) & ((uint8_t)0x01U)))
#define ClientFrame_AccessType_(byte_)               ((uint8_t)(((byte_) >> ((uint8_t)4U)) & ((uint8_t)0x01U)))
#define ClientFrame_RequestSequence_(byte_)          ((uint8_t)((byte_) & ((uint8_t)0x0FU)))

#define fLinkBuffer_IncrementPtr_()  \
  do {\
    if(++(Faraabin.ActiveBuffer->_head) == Faraabin.ActiveBuffer->Size) {\
      Faraabin.ActiveBuffer->_head = 0U;\
    }\
    if(Faraabin.ActiveBuffer->_isFull) {\
      if(++(Faraabin.ActiveBuffer->_tail) == Faraabin.ActiveBuffer->Size) {\
        Faraabin.ActiveBuffer->_tail = 0U;\
      }\
    }\
    Faraabin.ActiveBuffer->_isFull = (Faraabin.ActiveBuffer->_head == Faraabin.ActiveBuffer->_tail);\
  }while(0)

/**
 * @brief Puts an amount of bytes in the buffer,
 * 
 * @param pData_ Pointer to the data.
 * @param size_ Size of the data to put in the buffer.
 */
#define fLinkBuffer_Put_(data_)  \
  Faraabin.ActiveBuffer->Buffer[Faraabin.ActiveBuffer->_head] = (data_);\
  fLinkBuffer_IncrementPtr_()

/**
 * @brief Flushes the buffer.
 * 
 */
#define fLinkBuffer_FlushByPointer_(pBuffer_, ppBuff_, pRetSize_)  \
do {\
  if((pBuffer_)->_isFull || ((pBuffer_)->_head != (pBuffer_)->_tail)) {\
    *(ppBuff_) = &((pBuffer_)->Buffer[(pBuffer_)->_tail]);\
  } else {\
    *(ppBuff_) = NULL;\
    *(pRetSize_) = 0U;\
    break;\
  }\
  if((pBuffer_)->_isFull) {\
    *(pRetSize_) = (pBuffer_)->Size - (pBuffer_)->_tail;\
  } else {\
    if((pBuffer_)->_head > (pBuffer_)->_tail) {\
      *(pRetSize_) = (pBuffer_)->_head - (pBuffer_)->_tail;\
    } else {\
      *(pRetSize_) = (pBuffer_)->Size - (pBuffer_)->_tail;\
    }\
  }\
  if((*(pRetSize_)) >= HUB_FRAME_MAX_QTY) {\
    *(pRetSize_) = HUB_FRAME_MAX_QTY;\
  }\
  (pBuffer_)->_tail += *(pRetSize_);\
  (pBuffer_)->_tail %= (pBuffer_)->Size;\
  (pBuffer_)->_isFull = false;\
}while(0)

/**
 * @brief In case of null or empty, this macro corrects given path to a fobject.
 * 
 */
#define CORRECT_PATH_(str_)  \
  if(str_ == NULL) {\
    str_ = (char*)RootPath____;\
  } else if(str_[0] == '\0') {\
    str_ = (char*)RootPath____;\
  } else {\
    /* Do nothing */\
  }

/**
 * @brief 
 * 
 */
#define ADD_NAME_FIELDS_() \
  do { \
    uint16_t size; \
    size = strlen(me->Name); \
    fAddToBufferU16(size); \
    if(size > 0) { \
      fAddToBuffer((uint8_t*)me->Name, size); \
    } \
    size = strlen(me->Path); \
    fAddToBufferU16(size); \
    if(size > 0) { \
      fAddToBuffer((uint8_t*)me->Path, size); \
    } \
    size = strlen(me->FileName); \
    fAddToBufferU16(size); \
    if(size > 0) { \
      fAddToBuffer((uint8_t*)me->FileName, size); \
    } \
  } while(0)

  /**
 * @brief 
 * 
 */
#define ADD_NAME_FIELDS_WO_FILENAME_() \
  do { \
    uint16_t size; \
    size = strlen(me->Name); \
    fAddToBufferU16(size); \
    if(size > 0) { \
      fAddToBuffer((uint8_t*)me->Name, size); \
    } \
    size = strlen(me->Path); \
    fAddToBufferU16(size); \
    if(size > 0) { \
      fAddToBuffer((uint8_t*)me->Path, size); \
    } \
  } while(0)

/**
 * @brief This macro adds control byte to the generated frame.
 * 
 */
#define ADD_CONTROL_BYTE_() \
  do {\
    uint8_t control = ((uint8_t)0U);\
    control = me->_type & ((uint8_t)0x7FU);\
    if(me->_init) {\
      control |= ((uint8_t)0x80U);\
    }\
    ADD_U8_(control);\
  }while(0)

/**
 * @brief 
 * 
 */
#define ADD_U8_(d_) \
  Faraabin.Serializer.CheckSum += (d_);\
  fLinkBuffer_Put_(d_)

/* Private typedef -----------------------------------------------------------*/
/**
 * @brief Definition of the MCU fobject.
 * 
 */
typedef struct {

  uint8_t _type;                                                            /*!< Type of the fobject. */
  
  bool _init;                                                               /*!< Init status of the fobject. */
  
  bool Enable;                                                              /*!< Enable status of the fobject. */
  
  char *Name;                                                               /*!< Name given to the fobject. */
  
  char *Path;                                                               /*!< Path given to the fobject. */
  
  char *FileName;                                                           /*!< FileName of the fobject. */
  
  uint8_t Seq;                                                              /*!< Fobject sequence counter. */
  
  void(*fpUserTerminalCallback)(uint8_t *userData, uint16_t userDataSize);  /*!< User terminal callback function pointer. */
  
  sChrono ChronoLiveTimeout;                                                /*!< Chrono for measuring live timeout. */
  
  bool _isHostConnected;                                                    /*!< Host connection status. */
  
  uint16_t SessionId;                                                    		/*!< Session id. */
  
  uint32_t BootTimeMs;                                                      /*!< Time since MCU boot in milliseconds. */
  
  bool BootTimeFirstFlag;                                                   /*!< Flag for starting boot time measurement once. */

}sFobjectMcu;

/**
 * @brief Function engine item object.
 * 
 */
typedef struct {

  sFaraabinFobjectFunction* Function;                                       /*!< Pointer to the function fobject. */
  
  uint8_t Param[FB_FUNCTION_PARAM_BUFFER_SIZE];                             /*!< Function arguments buffer. */
  
  bool IsFirstRun;                                                          /*!< Is first run flag. */
  
  sChrono _chrono;                                                          /*!< Internal chrono for time measurement puroposes. */
  
  uint8_t _result;                                                          /*!< Function execution result. */
  
  void(*fpUserTerminalCallback)(uint8_t *userData, uint16_t userDataSize);  /*!< Function pointer of the user terminal callback. */
  
  uint8_t Status;                                                           /*!< Function execution status ID. Could be one of FN_STATUS values. */
  
  uint32_t ObjectPtr;                                                       /*!< Pointer to the object attached to the function. */
  
  bool IsBusy;                                                              /*!< Is busy falg. */
  
  bool IsRunning;                                                           /*!< Is function running flag. */
  
}sFunctionEngine_Item;

/**
 * @brief Client frame object.
 * 
 */
typedef struct {
  
  uint8_t Control;              /*!< Control field of the client frame. */

  uint8_t FobjectProperty;      /*!< Fobject property of the client frame. */

  uint32_t FobjectPtr;          /*!< Pointer to the fobject in client frame. */
  
  uint8_t *Payload;             /*!< Payload of the client frame. */
  
  uint16_t PayloadSize;         /*!< Payload size in client frame. */
  
}sClientFrame;

/**
 * @brief Definition of the link object
 * 
 */
typedef struct {
  
  bool Init;                        /*!< Initialization flag of faraabin link handler. */

  uint8_t *RxCharBuffer;            /*!< Pointer to the value of received character via faraabin link. */

  uint16_t RxCharBufferSize;        /*!< Size of received characters buffer. */
  
  uint8_t RxPatternIndex;
	
	uint16_t RxCharBufferIndex;

  bool IsNewFrameDetected;          /*!< Flag for indicating that new frame is detected. */

  sClientFrame ClientFrame;         /*!< Client frame. */

  bool IsFlushingBuffer;            /*!< Flag for indicating that Faraabin buffer is being flushed. */
  
  sChrono ChronoPortSending;        /*!< Chrono for measuring sending time. */

  bool DictSendFlag;                /*!< Flag for indicating send status. */
  
  uint8_t DictReqSeq;               /*!< Request sequence. */
  
  bool DictIsBlocking;              /*!< Flag for indicating that sending is blocking. */

  const char *Password;             /*!< String of the password that is set by the user for authenticating the loading procedure. */

  uint8_t SendBuffer[HUB_FRAME_MAX_QTY + sizeof(HUB_EOF_PATTERN)/*EOF pattern*/ + 1U/*Control byte*/]; /*!< Send buffer of the link in lower layers. */

  bool FlagReceiveFrameBigSize;

}sLink;

/**
 * @brief Data structure for implementing a ring (circular) buffer.
 * 
 */
typedef struct {
  
  uint8_t *Buffer;  /*!< Pointer to the buffer for saving bytes. */

  uint32_t Size;    /*!< Size of the buffer in bytes. */

  uint32_t _head;   /*!< Index of the head in buffer. */

  uint32_t _tail;   /*!< Index of the tail in buffer. */

  bool _isFull;     /*!< Full flag of the link buffer. */

  bool _isOvf;      /*!< Is overflow flag of the link buffer. */
  
}sBuffer;

/**
 * @brief Union type for the feature flags.
 * 
 */
typedef union {
  
  uint32_t U32;                   /*!< Views feature flags as uint32_t variable. */

  struct sFaraabin_FeatureFlag {
  
    uint32_t DefaultDataBus     : 1;  /*!< Specifies whether the default databus is activated. */
    uint32_t DefaultEventGroup  : 1;  /*!< Specifies whether the default event group is activated. */
    
    uint32_t McuCli             : 1;  /*!< Specifies whether the MCU CLI feature is activated. */
    uint32_t ReservedFlag3      : 1;  /*!< Reserve. */
    uint32_t Password           : 1;  /*!< Specifies whether the Password feature is activated. */
    
    uint32_t CpuProfiler        : 1;  /*!< Specifies whether the CPU profiler addon is activated. */
    uint32_t StateMachine       : 1;  /*!< Specifies whether the state machine addon is activated. */
    uint32_t Unity              : 1;  /*!< Specifies whether the unity library addon is activated. */
    
    uint32_t AllowSendDickBlocking : 1;  /*!< Reserved feature flag for future use. */
    
    uint32_t IsChronoTickUp     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag10     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag11     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag12     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag13     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag14     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag15     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag16     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag17     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag18     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag19     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag20     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag21     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag22     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag23     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag24     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag25     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag26     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag27     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag28     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag29     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag30     : 1;  /*!< Reserved feature flag for future use. */
    uint32_t ReservedFlag31     : 1;  /*!< Reserved feature flag for future use. */
    
  } Bitfield; /*!< Views feature flags as bitfields. */

}uFeatureFlag;

/**
 * @brief Union for status flags.
 * 
 */
typedef union {
  
  uint32_t U32;                   /*!< Views status flags as uint32_t variable. */

  struct sFaraabin_StatusFlag {
  
    uint32_t McuReset           : 1;  /*!< Specifies the capability of resetting the MCU via Faraabin. */
    uint32_t NewDict            : 1;  /*!< Indicates the introduction of a new dictionary to Faraabin. */
    uint32_t DataBufferOverflow : 1;  /*!< Indicates Faraabin data buffer overflow. */
    uint32_t DictOverflow       : 1;  /*!< Indicates dictionary overflow. */
    uint32_t NullDict           : 1;  /*!< Indicates that a dictionary has null reference and is not valid. */
    uint32_t UnexpectedDict     : 1;  /*!< Indicates an unexpected dictionary error has occured. */
    
    uint32_t ProfilerSend       : 1;  /*!< Specifies whether CPU profiler data sending to Faraabin is enabled. */
    uint32_t ProfilerListOvf    : 1;  /*!< Indicates an overflow in the number of processes in CPU profiler. */
    uint32_t ProfilerDepthOvf   : 1;  /*!< Indicates an overflow in the number of nested processes in CPU profiler. */
    uint32_t ProfilerDuplicate  : 1;  /*!< Indicates a repetitive process in CPU profiler. */
    
    uint32_t DictDuplicate      : 1;  /*!< Indicates a repetitive dictionary. */
    
    uint32_t UninitializedFaraabin : 1;  /*!< Specifies that faraabin api called before initialize. */
    
    uint32_t CmdBufferOverflow  : 1;  /*!< Indicates Faraabin cmd buffer overflow. */
    uint32_t ReservedFlag13     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag14     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag15     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag16     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag17     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag18     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag19     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag20     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag21     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag22     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag23     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag24     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag25     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag26     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag27     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag28     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag29     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag30     : 1;  /*!< Reserved status flag for future use. */
    uint32_t ReservedFlag31     : 1;  /*!< Reserved status flag for future use. */
    
  } Bitfield;  /*!< Views status flags as bitfields. */

}uStatusFlag;

/**
 * @brief Faraabin status and feature flags type definition.
 * 
 */
typedef struct {

  uFeatureFlag Features; /*!< Feature flags. */

  uStatusFlag Status;    /*!< Status flags. */

}sFlags;

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
 * @brief Faraabin link serializer typedef.
 * 
 */
typedef struct {

  uint16_t DepthCounter;      /*!< Counter of the nested events for sending. */

  uint8_t TextEventBuffer[TEXT_EVENT_MAX_REENTRANCE + 1U][TEXT_EVENT_BUFFER_SIZE];  /*!< Buffer allocated for saving message of the event. */

  uint8_t CheckSum;           /*!< Calculated checksum of the link serializer. */

  uint8_t NodeSeq;            /*!< Node sequence. */

  sDictIterator DictIterator; /*!< Handles iterating over all added dictionaries. */

}sSerializer;

/**
 * @brief Faraabin event parameters in link serializer.
 * 
 */
typedef struct {
  
  uint8_t Severity;                                           /*!< Event severity. Could be one of SERIALIZER_EVENT_SEVERITY values. */
  
  uint16_t EventId;                                           /*!< Event Identifier. */
  
  uint8_t *pParam;                                            /*!< Pointer to the event parameters. */
  
  uint16_t ParamSize;                                         /*!< Parameter size. */
  
  void(*fpGeneratePayload)(uint32_t fobjectPtr, void *param); /*!< Pointer to the function that is responsible for generating the payload. */
  
  void *pPayloadParam;                                        /*!< Pointer to the payload parameters. */
  
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
  
   char *Name;                /*!< Name of the fobject. */

  char *Path;                 /*!< Path of the fobject.*/

  char *FileName;             /*!< FileName of the fobject.*/

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
  
  char *Name;           /*!< Name of the fobject. */

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
  
  char *Name;       /*!< Name of the fobject. */

  uint16_t Id;      /*!< Enum identifier. */
  
}sDictEnumMemberPayload;

/**
 * @brief Dictionary payload object for function groups.
 * 
 */
typedef struct {
  
  char *Name;       /*!< Name of the object. */
  
  char *Path;             /*!< Path of the object. */
  
  char *FileName;         /*!< FileName of the object. */
  
  uint32_t ObjectPtr;     /*!< Pointer to the object. */
  
  uint32_t GroupTypePtr;  /*!< Pointer to the function group. */
  
}sDictFunctionGroupPayloadParam;

/**
 * @brief Faraabin database object.
 * 
 */
typedef struct {
  
  uint32_t DatabaseArray[FB_MAX_FOBJECT_QTY];
  
  uint16_t DatabaseArrayIndex;
  
}sDatabase;

/**
 * @brief Faraabin object.
 * 
 */
typedef struct {
  
  sLink Link;

  sDatabase Database;

  sSerializer Serializer;

  sFunctionEngine_Item FunctionsList[FB_MAX_CONCURRENT_FUNCTION];

  sFlags Flags;

  sBuffer DataCircularBuffer;

  sBuffer CmdCircularBuffer;

  sBuffer *ActiveBuffer;

  uint8_t CmdBuffer[CMD_BUFFER_SIZE];

  sFobjectMcu Mcu; /*!< Singleton MCU Fobject of the Faraabin. */

  bool Init;
  
}sFaraabin;

/* Private variables ---------------------------------------------------------*/
#if defined(FB_FEATURE_FLAG_DEFAULT_DATABUS) || defined(FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP)
static const char* DefaultFobjectPath = "System"; /*!< Default fobject path. */
#endif

static sFaraabin Faraabin = {
  .Init = false,
};

/* Private function prototypes -----------------------------------------------*/

/**
 * @brief Serializer function's.
 * 
 */
static uint8_t fLinkSerializer_Init(uint8_t *txBuffer, size_t txBufferSize);
static void fLinkSerializer_SerializeDict(uint32_t fobjectPtr, uint16_t dictIndex, uint8_t reqSeq);
static uint16_t fLinkSerializer_FlushDataBuffer(uint8_t **ptrToBuffer);
static uint16_t fLinkSerializer_FlushCmdBuffer(uint8_t **ptrToBuffer);
//static void fLinkSerializer_CommonSendUserData(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, uint8_t *userData, uint16_t userDataSize);
static void fLinkSerializer_CommonSendEnable(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, bool isCmdBuffer);

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
static void fLinkSerializer_CodeBlockSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, bool isCmdBuffer);
static void fLinkSerializer_CodeBlockSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isCmdBuffer);
#endif

static void fLinkSerializer_DataBusSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isCmdBuffer);
static void fLinkSerializer_DataBusSendCaptureValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isEnd, void* captureValue);
static void fLinkSerializer_DataBusSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse);

static void fLinkSerializer_VarSendValue(uint32_t fobjectPtr, uint32_t dataPtr, uint32_t size, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, bool isCmdBuffer);

static void fLinkSerializer_McuSendPing(uint32_t fobjectPtr, uint8_t reqSeq, void* param, bool isCmdBuffer);
static void fLinkSerializer_McuSendLive(uint32_t fobjectPtr, uint8_t reqSeq, bool isEnd, bool isCmdBuffer);
static void fLinkSerializer_McuSendWhoAmI(uint32_t fobjectPtr, uint8_t reqSeq, bool isCmdBuffer);

#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fLinkSerializer_StateMachineSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, bool isCmdBuffer);
static void fLinkSerializer_StateMachineSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeqPtr, uint8_t reqSeq, bool isCmdBuffer);
#endif

/**
 * @brief MCU function's.
 * 
 */
static uint8_t fFobjectMcu_Init(void);
static void fFobjectMcu_Run(void);
static void fFobjectMcu_SendEventSystem(uint16_t eventId);
static void fFobjectMcu_SendEventSystemException(uint16_t eventId);
static void fFobjectMcu_SendEventSystemResponse(uint16_t eventId, uint8_t reqSeq, bool isCmdBuffer);
static void fFobjectMcu_SendEventSystemExceptionResponse(uint16_t eventId, uint8_t reqSeq, bool isCmdBuffer);

/**
 * @brief Link function's.
 * 
 */
static uint8_t fLinkHandler_Init(uint8_t *rxBuffer, size_t rxBufferSize);
static void fLinkHandler_Run(void);
static uint8_t fFaraaninLinkDeserializer_Deserialize(uint8_t * const buffer, uint16_t size, sClientFrame *deserializedFrame);

/**
 * @brief Link buffer function's.
 * 
 */
static uint8_t fLinkDataBuffer_Init(uint8_t *buffer, uint32_t size);
static void fLinkDataBuffer_Clear(void);
static uint8_t fLinkCmdBuffer_Init(void);
static void fLinkCmdBuffer_Clear(void);

/**
 * @brief Function engine function's.
 * 
 */
static uint8_t fFunctionEngine_Init(void);
static void fFunctionEngine_Run(void);
static uint8_t fFunctionEngine_Start(uint32_t functionPtr, uint32_t objectPtr, uint8_t* arg, uint16_t argSize);
static uint8_t fFunctionEngine_Stop(uint32_t functionPtr, uint32_t objectPtr);
static uint8_t fFunctionEngine_Pause(uint32_t functionPtr, uint32_t objectPtr);
static uint8_t fFunctionEngine_Resume(uint32_t functionPtr, uint32_t objectPtr);
static uint16_t fFunctionEngine_GetQtyOfRunningFunctions(void);
static uint8_t fFunctionEngine_GetPtrsOfRunningFunction(uint16_t index, uint32_t *ptr, uint32_t *objectPtr);

/**
 * @brief Database function's.
 * 
 */
static uint8_t fDatabase_Init(void);
static bool fDatabase_IsDictExist(uint32_t fobjectPtr);

/**
 * @brief Serialize function's.
 * 
 */
static void fSerializeFrame(
  uint8_t frameType,
  uint8_t *fobjectSeq,
  uint8_t reqSeq,
  bool isEnd,
  uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t clientFrameGroup,
  uint8_t clientFrameId,
  void(*generatePayloadFunc)(uint32_t fobjectPtr, void *param), void *payloadParam,
  bool isCmdBuffer);

//static void fAddToBufferU8(uint8_t d);
static void fAddToBufferU16(uint16_t d);
static void fAddToBufferU32(uint32_t d);
static void fAddToBufferU64(uint64_t d);
#ifdef __FARAABIN_LINK_SERIALIZER_COMMENT_SECTION_0
static void fAddToBufferF32(float32_t d); // TODO: This function is reserved here for future use.
static void fAddToBufferF64(float64_t d); // TODO: This function is reserved here for future use.
#endif
static void fAddToBufferString(char *string);
static void fAddToBuffer(uint8_t *data, uint32_t size);

static void fGeneratePayload_CommonEnableStatus(uint32_t fobjectPtr, void *param);

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
static void fGeneratePayload_CodeBlockDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_CodeBlockValue(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_CodeBlockSetting(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_McuProfiler(uint32_t fobjectPtr, void *param);
#endif

static void fGeneratePayload_DataBusSetting(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_DataBusCaptureValue(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_DataBusValue(uint32_t fobjectPtr, void *param);

static void fGeneratePayload_McuPing(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_McuLive(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_McuWhoAmI(uint32_t fobjectPtr, void *param);

#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fGeneratePayload_StateMachineValue(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_StateMachineSetting(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_StateMachineDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_StateMachineDictState(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_StateMachineDictTransition(uint32_t fobjectPtr, void *param);  
#endif

static void fGeneratePayload_VarValue(uint32_t fobjectPtr, void *param);

static void fGeneratePayload_DataBusDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_EventGroupDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_ContainerDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_McuDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_VarDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_VarTypeDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_VarTypeStructMemberDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_VarTypeEnumMemberDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_FunctionGroupTypeDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_FunctionGroupTypeMemberDict(uint32_t fobjectPtr, void *param);
static void fGeneratePayload_FunctionGroupDict(uint32_t fobjectPtr, void *param);

static void fGeneratePayload_DictIterator(uint32_t fobjectPtr, void *param);

static void fSerializePayload_Event(uint32_t fobjectPtr, void *param);

static void fDictIterator_Iterate(void);
static void fDictIterator_ResetCounter(uint16_t dictIndex);

static void fSerializeDict(
  uint32_t fobjectPtr,
  uint8_t *fobjectSeqPtr,
  uint8_t reqSeq,
  void(*fpGenerateDictPayload)(uint32_t, void*),
  void *pDictPayloadParam);
static void fGenerateDict(uint32_t fobjectPtr, uint8_t reqSeq);
static uint32_t fCountAllDictsFrames(void);

static void fFrameHandler(sClientFrame* clientFrame);

static void fFrameHandler_Mcu(sClientFrame* clientFrame);
static void fFrameHandler_DataBus(sClientFrame* clientFrame);
static void fFrameHandler_CodeBlock(sClientFrame* clientFrame);
static void fFrameHandler_StateMachine(sClientFrame* clientFrame);
static void fFrameHandler_StateMachineState(sClientFrame* clientFrame);
static void fFrameHandler_StateMachineTransition(sClientFrame* clientFrame);
static void fFrameHandler_Function(sClientFrame* clientFrame);
static void fFrameHandler_EventGroup(sClientFrame* clientFrame);

static void fSendCircularBuffer(bool flush);

static bool fIsChecksumOk(uint8_t *buffer, uint16_t size);

static void fQueueClear(sFaraabinFobjectDataBus * const me);
static void fQueueInsert(sFaraabinFobjectDataBus * const me, uint8_t *item);
static uint8_t fQueueRead(sFaraabinFobjectDataBus * const me, uint32_t num, uint8_t *item);

static void fRunCapture(sFaraabinFobjectDataBus *me);
static void fDetectChannelTrig(sFaraabinFobjectDataBus *me);

static sFunctionEngine_Item* fFn_Find(uint32_t functionPtr, uint32_t objectPtr);
static sFunctionEngine_Item* fFn_FindRunningFunction(uint32_t functionPtr, uint32_t objectPtr);
static uint8_t fFn_GetFreeIndex(void);

static uint8_t fDefaultFobjects_Init(void);
static void fDefaultFobjects_Run(void);

/* Variables -----------------------------------------------------------------*/
#if defined(FB_FEATURE_FLAG_DEFAULT_DATABUS)

/**
 * @brief Default databus declaration.
 * 
 */
FARAABIN_DATABUS_DEF_(DefaultDatabus);
static sFaraabinFobjectDataBus_Channel DefaultDatabusChannelBuffer[FB_DEFAULT_DATABUS_CHANNEL_QTY];

/**
 * @brief Default databus chrono for managing its runtime.
 * 
 */
static sChrono DefaultDatabusRunner;
#endif

#if defined(FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP)
/**
 * @brief Default event group declartion.
 * 
 */
FARAABIN_EVENT_GROUP_DEF_(DefaultEventGroup);
#endif

/**
 * @brief Notification event group declartion.
 * 
 */
FARAABIN_EVENT_GROUP_DEF_(NotificationEventGroup);

/**
 * @brief Defining primitive variable types in C/C++, so that faraabin can detect
 *        variables of these types.
 * 
 */
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(bool_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(uint8_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(int8_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(uint16_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(int16_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(uint32_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(int32_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(uint64_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(int64_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(float32_t);
FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(float64_t);

/**
 * @brief All fobjects that do not have a specified path in their declaration will
 *        use "root" as their default path.
 * 
 */
char* RootPath____ = "root";

/*
===============================================================================
                      ##### faraabin.c Exported Functions #####
===============================================================================*/
/**
 * @brief This function initializes faraabin.
 * 
 * @note User need to call this function once in the initialization procedure of the software.
 * 
 * @return result Can be one of FARAABIN_RET values.
 */
fb_result_t fFaraabin_Init(void) {

  if(fFaraabin_PortInit() != 0) {
    return FARAABIN_PORT_INIT_ERROR;
  }

  if(fLinkSerializer_Init(fFaraabin_GetTxBufferPointer(), fFaraabin_GetTxBufferSize()) != FARAABIN_OK) {
    return FARAABIN_SERIALIZER_INIT_FAILED;       /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(fLinkHandler_Init(fFaraabin_GetRxBufferPointer(), fFaraabin_GetRxBufferSize()) != FARAABIN_OK) {
    return FARAABIN_LINK_HANDLER_INIT_FAILED;     /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(fDatabase_Init() != FARAABIN_OK) {
    return FARAABIN_DATA_BASE_INIT_FAILED;        /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(fFobjectMcu_Init() != FARAABIN_OK) {
    return FARAABIN_MCU_FOBJECT_INIT_FAILED;      /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(fFunctionEngine_Init() != FARAABIN_OK) {
    return FARAABIN_FUNCTION_ENGINE_INIT_FAILED;  /* MISRA C:2012 Rule 15.5 deviation */
  }

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  if(fCpuProfiler_Init() != CPU_PROFILER_OK) {
    return FARAABIN_CPU_PROFILER_INIT_FAILED;     /* MISRA C:2012 Rule 15.5 deviation */
  }
#endif
  
  Faraabin.Init = true;
  
#ifdef FB_ADD_ON_FEATURE_FLAG_UNITY
  if(fFaraabinAddOn_Unity_Init() != FARAABIN_UNITY_OK) {
    return FARAABIN_UNITY_INIT_FAILED;            /* MISRA C:2012 Rule 15.5 deviation */
  }
#endif
  
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(bool_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(uint8_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(int8_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(uint16_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(int16_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(uint32_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(int32_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(uint64_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(int64_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(float32_t);
  FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(float64_t);
  
  if(fDefaultFobjects_Init() != FARAABIN_OK) {
    return FARAABIN_DEFAULT_FOBJECTS_INIT_FAILED; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  fFobjectMcu_SendEventSystem(MCU_EVENT_INFO_BOOT);
  
  return FARAABIN_OK;
}

/**
 * @brief This function enable faraabin.
 */
void fFaraabin_Enable(void) {
  
  Faraabin.Mcu.Enable = TRUE;
}

/**
 * @brief This function disable faraabin.
 */
void fFaraabin_Disable(void) {
  
  Faraabin.Mcu.Enable = FALSE;
}

/**
 * @brief This function executes faraabin main modules. This way probe data are prepared and
 *        can be sent via faraabin link to the PC.
 * 
 * @note This function can be executed in a low priority runtime of the software.
 * 
 * @attention Faraabin uses a queue to hold incoming probe data from various parts of the software.
 *            To prevent data mixing, faraabin utilizes a critical section mechanism.
 *            For more details, refer to the user manual.
 * 
 */
void fFaraabin_Run(void) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
  }
  
  fFobjectMcu_Run();
  fFunctionEngine_Run();
  fLinkHandler_Run();
  fDefaultFobjects_Run();
  
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  fCpuProfiler_Run();
#endif

  // Looping over all dictionaries to find databus pointers.
  // After finding databus pointer, captured data of that databus is sent.
  for(uint16_t i = 0U; i < Faraabin.Database.DatabaseArrayIndex; i++) {

    uint32_t fobjectPtr = Faraabin.Database.DatabaseArray[i];
    uint8_t *fobjectType = (uint8_t*)fobjectPtr;  /* MISRA C:2012 Rule 11.4 deviation */

    if(*fobjectType == FO_TYPE_DATABUS) {
      
      fFaraabinFobjectDataBus_SendCaptureDataRun((sFaraabinFobjectDataBus*)fobjectPtr); /* MISRA C:2012 Rule 11.4 deviation */

    }
  }

  fFaraabin_PortRun();
}

/**
 * @brief This function must be called after receving a byte from the link dedicated to faraabin.
 * 
 * @note If the user has a link that can fetch chunks of data, in the receive callback of their link,
 *       they should loop over all the fetched data and call this function for each received character.
 * 
 * @param c Received byte buffer.
 * @param c Received byte size.
 */
void fFaraabin_HandleReceivedBytes(uint8_t *pData, uint16_t size) {
  
  if(!Faraabin.Init) {

    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return;
  }

  if((Faraabin.Link.RxCharBufferIndex + size) >= Faraabin.Link.RxCharBufferSize) {
      
    Faraabin.Link.RxCharBufferIndex = ((uint16_t)0U);
    Faraabin.Link.FlagReceiveFrameBigSize = true;
    return;
  }
  
  for(uint16_t i = 0; i < size; i++) {
  
    Faraabin.Link.RxCharBuffer[Faraabin.Link.RxCharBufferIndex] = pData[i];
    Faraabin.Link.RxCharBufferIndex++;
  }
}

/**
 * @brief Sets a new password for the faraabin.
 * 
 * @note This password is requested from user in the faraabin application (on PC) to initiate the connection procedure.
 *       Password is requested only if fFaraabin_PasswordEnable() is called.
 * 
 * @param password Password for starting the connection procedure in faraabin application.
 */
void fFaraabin_PasswordSet(char *password) {
  
  // This line is for suppressing unused variable compiler warning.
  (void)Faraabin.Link.Password;
  
  Faraabin.Link.Password = password;
  
}

/**
 * @brief Enables request of the password in faraabin application.
 * 
 */
void fFaraabin_PasswordEnable(void) {
  
  Faraabin.Flags.Features.Bitfield.Password = 1U;

}

/**
 * @brief Disables request of the password in faraabin application.
 * 
 */
void fFaraabin_PasswordDisable(void) {
  
  Faraabin.Flags.Features.Bitfield.Password = 0U;
  
}

/**
 * @brief This function checks if the host (PC application) is connected to the MCU or not.
 * 
 * @retval IsHostConnected Connection status.
 */
bool fFaraabin_IsHostConnected(void) {
  
  return Faraabin.Mcu._isHostConnected;
}

/**
 * @brief This function checks if the event is allow to send.
 * 
 * @retval IsAllowEvent status.
 */
bool fFaraabin_IsAllowEvent(void) {
  
  if(!Faraabin.Mcu.Enable) {
    return false; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(Faraabin.Link.DictSendFlag) {
    return false; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  return true;
}

/**
 * @brief This function flush all data exist in buffer to link.
 * 
 */
void fFaraabin_FlushBuffer(void) {
  
  fSendCircularBuffer(true);
}

/**
 * @brief This function reset receive buffer.
 * 
 */
void fFaraabin_ResetReceiveBuffer(void) {
	
	Faraabin.Link.RxCharBufferIndex = ((uint16_t)0U);
	Faraabin.Link.RxPatternIndex = 0;
	Faraabin.Link.IsNewFrameDetected = 0;
}

/**
 * @brief 
 * 
 */
__attribute__((weak)) void fFaraabin_DictSendingCpltCallback(void) {
}

/*
fobject_vartype
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*
@verbatim
   
  This fobject can be used to declare user-defined types to Faraabin.
  Follow these steps for the declaration:
  1. Define a user-defined type (either a structure or an enumeration).
  2. Use FARAABIN_VAR_TYPE_DEF_() to define a fobject for that variable type.
  3. Faraabin needs to capture the data structure of the user-defined type.
     To do this, users need to provide the information in a dictionary function as follows:
  - For structures:
    - Declare FARAABIN_VAR_TYPE_STRUCT_FUNC_().
    - Inside the function body, call FARAABIN_VAR_TYPE_STRUCT_FUNC_START_().
    - Declare each member of the structure (in order) using FARAABIN_VAR_TYPE_STRUCT_MEMBER_().
    - When all members are declared, use FARAABIN_VAR_TYPE_STRUCT_FUNC_END_() to finish the function implementation.
  - For enumerations:
      - Implement FARAABIN_VAR_TYPE_ENUM_FUNC_().
      - Inside this function, call FARAABIN_VAR_TYPE_ENUM_FUNC_START_().
      - Define each member (in order) using FARAABIN_VAR_TYPE_ENUM_MEMBER_().
      - Use FARAABIN_VAR_TYPE_ENUM_FUNC_END_() to end the function implementation.
  4. When all members of the user-defined type are declared, initialize the variable type using 
     FARAABIN_VAR_TYPE_STRUCT_INIT_() for structures or FARAABIN_VAR_TYPE_ENUM_INIT_() for enumerations.
  
  Below is an example of how to use this module to introduce a structure to Faraabin.

  @code

  #include "faraabin.h"

  // User defined type definition.
  typedef struct {
  
    float32_t a;
    uint8_t b[10];
    float64_t c;

  }sUserDefinedType;

  // Defining variable type fobject for this structure.
  FARAABIN_VAR_TYPE_DEF_(sUserDefinedType);

  // Implementing dictionary function for introducing this type to faraabin.
  FARAABIN_VAR_TYPE_STRUCT_FUNC_(sUserDefinedType) {
    FARAABIN_VAR_TYPE_STRUCT_FUNC_START_(sUserDefinedType);

    FARAABIN_VAR_TYPE_STRUCT_MEMBER_(float32_t, a, 1);
    FARAABIN_VAR_TYPE_STRUCT_MEMBER_(uint8_t, b, 10);
    FARAABIN_VAR_TYPE_STRUCT_MEMBER_(float64_t, c, 1);

    FARAABIN_VAR_TYPE_STRUCT_FUNC_END_(sUserDefinedType);
  }
  
  int main(void) {

    // Initializing variable type fobject.
    FARAABIN_VAR_TYPE_STRUCT_INIT_(sUserDefinedType);
    
    //...

  }

  @endcode

  @attention
  Never declare primitive types as they have been defined internally in Faraabin.
  All primitive types are introduced to Faraabin as follows:
  
  - 'bool' for booleans.
  - 'uint8_t' for unsigned byte.
  - 'uint16_t' for unsigned 16-bit variables.
  - 'uint32_t' for unsigned 32-bit variables.
  - 'uint64_t' for unsigned 64-bit variables.
  - 'int8_t' for signed byte.
  - 'int16_t' for signed 16-bit variables.
  - 'int32_t' for signed 32-bit variables.
  - 'int64_t' for signed 64-bit variables.
  - 'float32_t' for 32-bit floating point values.
  - 'float64_t' for 64-bit floating point values.
  
  To declare the type of a member in structures, users need to use the exact keywords listed above.

  @endverbatim
*/
/*
===============================================================================
            ##### faraabin_fobject_var_type.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes the variable type fobject of Faraabin and adds the fobject
 *        to the dictionary.
 * 
 * @note Users are advised to use wrappers in faraabin_fobject_vartype_wrapper.h to work with
 *       vartype fobjects.
 * 
 * @param me Pointer to the fobject.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectVarType_Init(sFaraabinFobjectVarType *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR;           /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(fDatabase_AddDict((uint32_t)me) != FARAABIN_OK) {  /* MISRA C:2012 Rule 11.4 deviation */
    return FARAABIN_CANNOT_ADD_DICT_ERROR;              /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  me->_init = false;
  me->_type = (uint8_t)FO_TYPE_VAR_TYPE;
  
  me->Enable = true;
  me->_init = true;
  return FARAABIN_OK;
}

/*
fobject_eventgroup
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*
@verbatim

  This fobject is used to generate and send events to the Faraabin application.
  Events can be:
  - System events, which are only used by faraabin library and user does not need to 
    utilize them.
  - Text events, that can send user defined strings to Faraabin.
  - Enumerated events, that are defined both for faraabin lib and faraabin app and are
    more efficient that sending text to the PC.
  
  In Faraabin, a default eventgroup is instantiated in fFaraabin_Init(). But users can
  declare their own eventgroups and send events specifically to those groups.

  To send events to faraabin, use macroes defined in faraabin_fobject_eventgroup_wrapper.h
  file.

  @note In Faraabin, each fobject has its own event group in addition to user-defined event groups.
        This allows users to send messages not only to event groups but also directly to the fobject.

  @endverbatim
*/
/*
===============================================================================
          ##### faraabin_fobject_event_group.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes Faraabin event group fobject and adds its dictionary to database.
 * 
 * @param me Pointer to the event group fobject.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectEventGroup_Init(sFaraabinFobjectEventGroup *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR;           /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(fDatabase_AddDict((uint32_t)me) != FARAABIN_OK) {  /* MISRA C:2012 Rule 11.4 deviation */
    return FARAABIN_CANNOT_ADD_DICT_ERROR;              /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  me->_type = (uint8_t)FO_TYPE_EVENT_GROUP;
  
  me->Enable = true;
  me->_init = true;
  return FARAABIN_OK;
}

/*
fobject_container
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*
@verbatim

  The main purpose of this fobject is to assist users in introducing special
  fobjects to Faraabin.
  
  Fobjects in Faraabin contain information that defines them completely to Faraabin. 
  Databus, event groups, codeblocks, and variable types generate this information and send it 
  to Faraabin during the connection phase. This is possible because these fobjects are already 
  known to the Faraabin application.
  
  On the other hand, variables and function groups are fobjects whose definitions are entirely 
  dependent on the user.
  
  For these fobjects, users must provide some information to Faraabin so that Faraabin understands
  their nature. For example, for variables, the name of the variable, its length 
  (if it is an array), and its type (either primitive or user-defined) are the needed information to
  send to Faraabin.
  
  The container is the medium through which Faraabin obtains additional information for variables 
  and functions to introduce them to Faraabin.

  Below is an example on how to define a variable and a function group to Faraabin using Container.

  @code 

  #include "faraabin.h"
  #include "my_function_group.h"

  // Define a container.
  FARAABIN_CONTAINER_DEF_(Container);

  // Declare a float variable.
  static float32_t TestVariable;

  // Add dictionary information to Container.
  FARAABIN_CONTAINER_FUNC_(Container) {

    FARAABIN_FUNCTION_GROUP_OBJECT_DICT_(MyFunctionGroup, void*);
    FARAABIN_VAR_F32_DICT_(TestVariable);

  }

  int main(void) {

    // Initialize Faraabin.
    fFaraabin_Init();

    // Initialize Container
    FARAABIN_Container_Init_(&Container);
    FARAABIN_FunctionGroupType_Init_(&MyFunctionGroup)

    while(true) {
      fFaraabin_Run();
    }
  }

  @endcode

  @endverbatim
 */
/*
===============================================================================
            ##### faraabin_fobject_container.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes a container fobject.
 * 
 * @param me Pointer to the Container.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectContainer_Init(sFaraabinFobjectContainer *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR;           /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(fDatabase_AddDict((uint32_t)me) != FARAABIN_OK) {  /* MISRA C:2012 Rule 11.4 deviation */
    return FARAABIN_CANNOT_ADD_DICT_ERROR;              /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  me->_init = false;
  me->_type = (uint8_t)FO_TYPE_CONTAINER;
  
  me->Enable = true;
  me->_init = true;
  
  return FARAABIN_OK;
}

/*
fobject_function
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/*
@verbatim

  This fobject is used for declaring functions whose execution is managed within Faraabin.
  To facilitate using this fobject, users are advised to utilize all wrappers in faraabin_fobject_function_wrapper.h.
  
  Follow these steps for using a Faraabin function fobject:
  
  1. Create a header file and a C file for the declaration of the function group and its functions.
  2. In the header file, use FARAABIN_FUNCTION_GROUP_PROTOTYPE_() to declare the function group globally.
  3. If your function group can be used in an object-oriented manner, use FARAABIN_FUNCTION_GROUP_TYPE_DEF_() to
     define the type of the object that the function group can be attached to.
  4. Declare your functions using FARAABIN_FUNCTION_() and fill in the function body.
     If the function has a loop and you want to run it in the Faraabin app, use FARAABIN_FUNCTION_LOOP_() instead.
  5. In the function body, you can use FARAABIN_Function_GetParam_() to get parameters from Faraabin. This macro works
     just like the scanf() function.
  6. If you want to send an event to Faraabin inside a function, use FARAABIN_PrintfToFunction().
  7. Start the loop part of the function using the FARAABIN_Function_LoopStartMs_() macro. Add FARAABIN_Function_LoopEnd_() at the end of the loop.
  8. At the end of the function implementation, use FARAABIN_FUNCTION_END_() to signal to Faraabin that the function executed to completion or,
     if it is a loop function, Faraabin will know that the next iteration must be executed from the FARAABIN_Function_LoopStartMs_() part
     of the function.
  9. Add all functions to the corresponding function group using FARAABIN_FUNCTION_GROUP_ADD_().
  
  Now that your function group with its functions is created successfully, you should instantiate a function group fobject in
  your software. To do this, follow these steps:
  
  1. Create a Container in your app. If you don't know how to do this, refer to the faraabin_fobject_container.c file.
  2. Inside the container function, add dictionary information of the function group using FARAABIN_FUNCTION_GROUP_DICT_WP_()
     or FARAABIN_FUNCTION_GROUP_OBJECT_DICT_WP_() for object-oriented functions.
  3. Initialize the function group fobject using FARAABIN_FunctionGroupType_Init_().
  4. In the Faraabin application, your functions will be listed under FunctionGroup.
  5. Run your function with the Faraabin UI or API on a PC.
  
  Below is an example of how to implement a function group. Let's assume that the user wants to create the function group and its function in my_function_group.c file.
  Remember to include "faraabin.h" in my_function_group.h file and declare FARAABIN_FUNCTION_GROUP_PROTOTYPE_(MyFunctionGroup) in this file to create a new function
  group called "MyFunctionGroup". In my_function_group.c file, do the following:
  @code

  #include "faraabin.h"
  #include "my_function_group.h"

  // Define the function group to be executed on objects of type "float32_t".
  FARAABIN_FUNCTION_GROUP_TYPE_DEF_(float32_t);

  // Implement a run-to-completion function.
  FARAABIN_FUNCTION_(MyFunctionGroup, TEST_FUNCTION, "Tests function fobject in Faraabin") {

    uint8_t a;
    uint8_t b;

    // Print an event to the function.
    FARAABIN_PrintfToFunction_("\r\n This is an event from TEST_FUNCTION.");

    // Get parameters from PC and assign to the 'a' and 'b' variables.
    FARAABIN_Function_GetParam_(2, "%u %u", a, b);

    // Increment the value of object attached to function group.
    (*me)++;
    FARAABIN_PrintfToFunction_("\r\n New value of object is %f.", me);
    
    FARAABIN_FUNCTION_END_();
  }

  // Implement a looping function.
  FARAABIN_FUNCTION_LOOP_(MyFunctionGroup, TEST_LOOP_FUNCTION, "Tests function loop fobject in Faraabin") {

    uint32_t interval;

    

    // Get parameters from PC and assign to the 'a' and 'b' variables.
    FARAABIN_Function_GetParam_(1, "%u", interval);

    FARAABIN_Function_LoopStartMs_(interval) {

      // Print an event to the function.
      FARAABIN_PrintfToFunction_("\r\n This is an event from TEST_LOOP_FUNCTION.");

      // Increment the value of object attached to function group.
      (*me)++;
      FARAABIN_PrintfToFunction_("\r\n New value of object is %f.", me);

      FARAABIN_Function_LoopEnd_();
    }
    
    FARAABIN_FUNCTION_END_();
  }

  FARAABIN_FUNCTION_GROUP_(MyFunctionGroup, "This is help for the function group") {

    FARAABIN_FUNCTION_GROUP_ADD_(MyFunctionGroup, TEST_FUNCTION);
    FARAABIN_FUNCTION_GROUP_ADD_(MyFunctionGroup, TEST_LOOP_FUNCTION);

  }

  @endcode

  Now that the function group is created, the user can instantiate it in their application. Imagine this will happen in user_app.c.

  @code
  
  #include "my_function_group.h"

  // Define a container.
  FARAABIN_CONTAINER_DEF_(Container);

  // Declare a float variable to pass to the function group.
  static float32_t TestVariable;

  // Add dictionary information of the function group to Container.
  FARAABIN_CONTAINER_FUNC_(Container) {

    FARAABIN_FUNCTION_GROUP_OBJECT_DICT_(MyFunctionGroup, TestVariable);

  }

  int main(void) {

    // Initialize Faraabin.
    fFaraabin_Init();

    // Initialize Container and FunctionGroup.
    FARAABIN_Container_Init_(&Container);
    FARAABIN_FunctionGroupType_Init_(&MyFunctionGroup)

    while(true) {
      fFaraabin_Run();
    }
  }

  @attention
  Never declare an objective function group with a NULL object. In this case, whenever your function wants to access
  the object, it will access the 0x00000000 address and may cause your application to crash.
  Always handle NULL pointer access in your functions.

  @endverbatim
 */
/*
===============================================================================
            ##### faraabin_fobject_function.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes a function group and adds its dictionary to the database.
 * 
 * @param me Pointer to the function group.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectFunctionGroupType_Init(sFaraabinFobjectFunctionGroupType *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR;           /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(fDatabase_AddDict((uint32_t)me) != FARAABIN_OK) {  /* MISRA C:2012 Rule 11.4 deviation */
    return FARAABIN_CANNOT_ADD_DICT_ERROR;              /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  me->_type = (uint8_t)FO_TYPE_FUNCTION_GROUP_TYPE;
  
  me->_init = true;
  return FARAABIN_OK;
}

/*
fobject_databus
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/** @verbatim
 * 
 * Databus is a streaming fobject used to send variables and codeblocks data
 * to the Faraabin application.
 * 
 * Users can control the data transmission rate using this fobject in streaming mode.
 * For data rates higher than the link bandwidth capabilities, Databus offers capture
 * mode, where data are stored for a defined amount of time in MCU RAM and then sent to
 * the PC. This permits very high-speed logging of data.
 * 
 * To use this fobject, define it using FARAABIN_DATABUS_DEF_().
 * You can set channel quantity, capture buffer size, etc., using FARAABIN_DATABUS_SET_VALUE_().
 * Initialize the databus using FARAABIN_DataBus_Init_() and set the mode using FARAABIN_DataBus_StartStreamMode_() or 
 * FARAABIN_DataBus_StartTriggerMode_().
 * You can enable or disable the databus using FARAABIN_DataBus_Enable_() or FARAABIN_DataBus_Disable_() macros.
 * There are macros in faraabin_fobject_databus_wrapper.h for attaching variables or codeblocks to the databus.
 * Call FARAABIN_DataBus_Run_() in the preferred runtime for executing the databus. Change the runtime prescaler in Faraabin
 * or by setting StreamDivideBy, TimerDivideBy, or TrigDivideBy to preferred values correspondingly.
 * 
 * @endverbatim

===============================================================================
              ##### faraabin_fobject_databus.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes databus fobject.
 * 
 * @param me Pointer to the databus fobject.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectDataBus_Init(sFaraabinFobjectDataBus *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR;       /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(fDatabase_AddDict((uint32_t)me) != 0U) {       /* MISRA C:2012 Rule 11.4 deviation */
    return FARAABIN_CANNOT_ADD_DICT_ERROR;          /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  me->_init = false;
  me->_type = (uint8_t)FO_TYPE_DATABUS;
  
  if(me->ChannelQty == ((uint16_t)0U)) {
    return FARAABIN_DATABUS_CHANNEL_SIZE_ERROR;     /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(me->pBufferChannels == NULL) {
    return FARAABIN_DATABUS_BUFFER_NULL_ERROR;    /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  for(uint16_t i = 0; i < me->ChannelQty; i++) {
    
    me->pBufferChannels[i].ItemFobjectType = ((uint8_t)0U);
    me->pBufferChannels[i].ItemFobjectPtr = 0U;
    me->pBufferChannels[i].ItemFobjectParam = ((uint16_t)0U);
    me->pBufferChannels[i].VariableDataType = ((uint8_t)0U);
    me->pBufferChannels[i].PrimitiveVariableId = ((uint8_t)0U);
    me->pBufferChannels[i].Enable = false;
    
  }
  
	if(me->BufferCaptureSize > 0) {
		if(me->pBufferCapture == NULL) {
			return FARAABIN_DATABUS_BUFFER_NULL_ERROR;    /* MISRA C:2012 Rule 15.5 deviation */
		}
	}
  
  // Initialize offline queue
  me->QueueItemCount = 0U;
  me->_queueFrontIndex = 0U;
  me->_queueRearIndex = 0U;
  
  me->CurrentState = DATABUS_STATE_OFF;
  
  me->ApiTrigEnable = true;
  me->LastTrigSource = DATABUS_TRIG_SOURCE_API;
  
  me->AttachedItemsQty = ((uint16_t)0U);
  me->AvailableItemsQty = ((uint16_t)0U);
  me->CaptureSendingQty = 0U;
  
  fChrono_Start(&me->_chronoCycle);
  
  me->Enable = TRUE;
  
  me->_init = true;
  return FARAABIN_OK;
}

/**
 * @brief Deinitializes databus fobject and frees all allocated memory.
 * 
 * @param me Pointer to the databus fobject.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectDataBus_AdvFeat_DeInit(sFaraabinFobjectDataBus *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }

  me->_init = false;

  return FARAABIN_OK;
}

/**
 * @brief Runs databus state machine.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_Run(sFaraabinFobjectDataBus *me)  {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(!fFaraabin_IsAllowEvent()) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  me->CycleUs = fChrono_IntervalUs(&(me->_chronoCycle));

  if(!me->Enable) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(me->AvailableItemsQty == 0U) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  switch(me->CurrentState) {
    
    case DATABUS_STATE_OFF: {
      break;
    }
    
    case DATABUS_STATE_STREAM: {
      
      RUN_EVERY_QTY_OBJ_(me->StreamDivideBy, me->_streamDivbyCnt) {
        
        fLinkSerializer_DataBusSendValue((uint32_t)me, &me->Seq, ((uint8_t)0U), false); /* MISRA C:2012 Rule 11.4 deviation */
        
        RUN_END_;
      }
      
      break;
    }
    
    case DATABUS_STATE_TIMER: {
      
      RUN_EVERY_QTY_OBJ_(me->TimerDivideBy, me->_timerDivbyCnt) {
        
        fRunCapture(me);
        
        RUN_END_;
      }
      
      if(fChrono_IsTimeout(&(me->_chronoTrigWindow)) == true) {
        
        me->CurrentState = DATABUS_STATE_OFF;
        Faraabin_EventSystem_End_((uint32_t)me, &me->Seq, me->Enable, DATABUS_EVENT_INFO_CAPTURE_END); /* MISRA C:2012 Rule 11.4 deviation */
        
      }
      
      break;
    }
        
    case DATABUS_STATE_TRIG_WAIT: {
      
      RUN_EVERY_QTY_OBJ_(me->TrigDivideBy, me->_trigDivbyCnt) {
        
        fRunCapture(me);
        fDetectChannelTrig(me);
        
        RUN_END_;
      }
        
      if(me->_isTriggered == true) {
        
        fChrono_StartTimeoutMs(&(me->_chronoTrigWindow), me->TimeAfterTrigMs);
        
        me->CurrentState = DATABUS_STATE_TRIG_WINDOW;
        
        uint8_t eventParam[10];
        
        eventParam[0] = (uint8_t)me->CurrentState;
        eventParam[1] = (uint8_t)me->LastTrigSource;
        
        uByte4 trigTimestamp;
        trigTimestamp.U32 = me->_trigTimeStamp;
        eventParam[2] = trigTimestamp.Byte[0];
        eventParam[3] = trigTimestamp.Byte[1];
        eventParam[4] = trigTimestamp.Byte[2];
        eventParam[5] = trigTimestamp.Byte[3];
        
        uByte4 cycleUs;
        cycleUs.U32 = me->CycleUs;
        eventParam[6] = cycleUs.Byte[0];
        eventParam[7] = cycleUs.Byte[1];
        eventParam[8] = cycleUs.Byte[2];
        eventParam[9] = cycleUs.Byte[3];
        
        Faraabin_EventSystem_ParamEnd_((uint32_t)me, &me->Seq, me->Enable, DATABUS_EVENT_INFO_STATE_CHANGE, eventParam, 10U);  /* MISRA C:2012 Rule 11.4 deviation */
        
      }
    
      break;
    }
    
    case DATABUS_STATE_TRIG_WINDOW: {
      
      RUN_EVERY_QTY_OBJ_(me->TrigDivideBy, me->_trigDivbyCnt) {
            
        fRunCapture(me);
        
        RUN_END_;
      }
      
      if(fChrono_IsTimeout(&(me->_chronoTrigWindow)) == true) {
        
        me->IsCaptureEnd = true;
        me->CurrentState = DATABUS_STATE_OFF;
        Faraabin_EventSystem_End_((uint32_t)me, &me->Seq, me->Enable, DATABUS_EVENT_INFO_CAPTURE_END); /* MISRA C:2012 Rule 11.4 deviation */
        
      }
      
      break;
    }
    
    default: {

      // Do nothing.
      break;
    }
  }
}

/**
 * @brief Sends captured data if databus is in DATABUS_STATE_CAPTURE_SEND state.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_SendCaptureDataRun(sFaraabinFobjectDataBus *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  switch(me->CurrentState) {

    case DATABUS_STATE_CAPTURE_SEND: {

      for(uint16_t i = 0; i < FB_DATABUS_CAPTURE_SEND_ITEMS; i++) {

        sFaraabinFobjectDataBus_CaptureValue tmp_capture;
        
        uint8_t ret = fQueueRead(me, me->CaptureSendingCnt, (uint8_t*)&tmp_capture);
        if(ret == 0U) {
          
          fLinkSerializer_DataBusSendCaptureValue(
            (uint32_t)me, /* MISRA C:2012 Rule 11.4 deviation */
            &me->Seq,
            me->CaptureSendingReqSeq, 
            false,
            &tmp_capture);
            
          fFaraabin_FlushBuffer();
          
        } else {
          
          Faraabin_EventSystem_EndResponse_((uint32_t)me, &me->Seq, me->Enable, DATABUS_EVENT_ERROR_CAPTURE_QUEUE, me->CaptureSendingReqSeq, false); /* MISRA C:2012 Rule 11.4 deviation */
          break;  /* MISRA C:2012 Rule 15.4 deviation */
        }
        
        me->CaptureSendingCnt++;
        if(me->CaptureSendingCnt >= me->CaptureSendingQty) {
          
          me->CurrentState = DATABUS_STATE_OFF;
          Faraabin_EventSystem_ParamEndResponse_((uint32_t)me, &me->Seq, me->Enable, DATABUS_EVENT_INFO_STATE_CHANGE, (uint8_t*)&me->CurrentState, 1, me->CaptureSendingReqSeq, false);  /* MISRA C:2012 Rule 11.4 deviation */

          break;  /* MISRA C:2012 Rule 15.4 deviation */
        }
      }

      break;
    }

    default: {

      /* Do nothing! */
      break;
    }
  }
}

/**
 * @brief Gets the number of attached items to the channels of the databus.
 * 
 * @param me Pointer to the databus fobject.
 * @return num Number of attached items.
 */
uint16_t fFaraabinFobjectDataBus_GetAttachCount(sFaraabinFobjectDataBus *me) {

  if(!me->_init) {
    return 0; /* MISRA C:2012 Rule 15.5 deviation */
  }

  uint16_t cnt = ((uint16_t)0U);
  
  for(uint16_t i = 0; i<me->ChannelQty; i++) {
    
    if(me->pBufferChannels[i].ItemFobjectPtr != 0U) {
      cnt++;
    }
  }
  
  return cnt;
}

/**
 * @brief Resets the trigger level of the databus.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_ResetTrigger(sFaraabinFobjectDataBus *me) {

  if(!me->_init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(!me->ChTrigEnable) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(me->ChTrigNo >= me->ChannelQty) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(!me->pBufferChannels[me->ChTrigNo].Enable) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  uint8_t *ptr = (uint8_t*)me->pBufferChannels[me->ChTrigNo].ItemFobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */

  if(ptr == NULL) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  uByte8 tmp = {(uint8_t)(0x00U)};

  switch((uint8_t)me->pBufferChannels[me->ChTrigNo].PrimitiveVariableId) {
    
    case VAR_DATA_TYPE_PRIMITIVE_BOOL: {
      
      me->_trigChannelPayload.Byte[0] = ptr[0];
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_UINT8: {
      
      me->_trigChannelPayload.Byte[0] = ptr[0];
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_INT8: {
      
      me->_trigChannelPayload.Byte[0] = ptr[0];
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_UINT16: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      me->_trigChannelPayload.U16[0] = tmp.U16[0];
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_INT16: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      me->_trigChannelPayload.I16[0] = tmp.I16[0];
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_UINT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      me->_trigChannelPayload.U32[0] = tmp.U32[0];
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_INT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      me->_trigChannelPayload.I32[0] = tmp.I32[0];
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_UINT64: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      tmp.Byte[4] = ptr[4];
      tmp.Byte[5] = ptr[5];
      tmp.Byte[6] = ptr[6];
      tmp.Byte[7] = ptr[7];
      me->_trigChannelPayload.U64 = tmp.U64;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_INT64: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      tmp.Byte[4] = ptr[4];
      tmp.Byte[5] = ptr[5];
      tmp.Byte[6] = ptr[6];
      tmp.Byte[7] = ptr[7];
      me->_trigChannelPayload.I64 = tmp.I64;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_FLOAT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      me->_trigChannelPayload.F32[0] = tmp.F32[0];
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_FLOAT64: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      tmp.Byte[4] = ptr[4];
      tmp.Byte[5] = ptr[5];
      tmp.Byte[6] = ptr[6];
      tmp.Byte[7] = ptr[7];
      me->_trigChannelPayload.F64 = tmp.F64;
      
      break;
    }
    
    default: {

      // Do nothing.
      break;
    }
  }
}

/**
 * @brief Starts the databus in stream mode.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_StartStream(sFaraabinFobjectDataBus *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  me->CurrentState = DATABUS_STATE_STREAM;
  
}

/**
 * @brief Starts the databus in timer mode.
 * 
 * @param me Pointer to the databus fobject.
 * @param timerPrescaler Timer mode prescaler. Starts from 1.
 * @param timerWindow_ms Time window for capturing data in timer mode.
 */
void fFaraabinFobjectDataBus_StartTimer(sFaraabinFobjectDataBus *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  fQueueClear(me);

  fChrono_StartTimeoutMs(&me->_chronoTrigWindow, me->TimerWindowMs);
  
  me->CurrentState = DATABUS_STATE_TIMER;
}

/**
 * @brief Starts the databus in trigger mode.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_StartTrigger(sFaraabinFobjectDataBus *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  me->_isTriggered = false;
  me->IsCaptureEnd = false;
  fFaraabinFobjectDataBus_ResetTrigger(me);

  fQueueClear(me);
  me->CurrentState = DATABUS_STATE_TRIG_WAIT;

}

/**
 * @brief Forces a trig in the user application.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_ForceTrigger(sFaraabinFobjectDataBus *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if((me->CurrentState == DATABUS_STATE_TRIG_WAIT) && (me->ApiTrigEnable == true)) {
    
    me->_trigTimeStamp = fChrono_GetTick();
    me->LastTrigSource = DATABUS_TRIG_SOURCE_API;
    me->_isTriggered = true;
    
  }
}

/**
 * @brief Forces a trig manually in the user interface.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_ForceManualTrigger(sFaraabinFobjectDataBus *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(me->CurrentState == DATABUS_STATE_TRIG_WAIT) {
    
    me->_trigTimeStamp = fChrono_GetTick();
    me->LastTrigSource = DATABUS_TRIG_SOURCE_MANUAL;
    me->_isTriggered = true;
    
  }
}

/**
 * @brief Puts databus in stop state.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_Stop(sFaraabinFobjectDataBus *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  me->CurrentState = DATABUS_STATE_OFF;

}

/**
 * @brief Checks whether capture is end or not.
 * 
 * @param me Pointer to the databus fobject.
 * @return isCaptureEnd Capture status.
 */
bool fFaraabinFobjectDataBus_IsCaptureEnd(sFaraabinFobjectDataBus *me) {

  if(!me->_init) {
    return true;  /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  return me->IsCaptureEnd;
}

/**
 * @brief Gets captured data quantity.
 * 
 * @param me Pointer to the databus fobject.
 * @return num Number of items in databus capture buffer.
 */
uint32_t fFaraabinFobjectDataBus_GetCaptureDataQty(sFaraabinFobjectDataBus *me) {

  if(!me->_init) {
    return 0U;  /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  return me->QueueItemCount;
}

/**
 * @brief Gets captured data.
 * 
 * @param me Pointer to the databus fobject.
 * @param index Index of the item in databus capture buffer.
 * @param value Pointer to the captured value.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectDataBus_GetCaptureData(sFaraabinFobjectDataBus *me, uint32_t index, sFaraabinFobjectDataBus_CaptureValue *value) {

  if(!me->_init) {
    return FARAABIN_DATABUS_NOT_INITIALIZED_ERROR;  /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(index >= me->QueueItemCount) {
    return FARAABIN_DATABUS_CHANNEL_INDEX_ERROR;    /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  return fQueueRead(me, index, (uint8_t*)&value);
}

/**
 * @brief Attaches a variable to a free channel in databus.
 * 
 * @param me Pointer to the databus fobject.
 * @param varPtr Pointer to the variable for attaching.
 * @param varSize Size of the variable.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectDataBus_AttachVariable(sFaraabinFobjectDataBus *me, uint32_t varPtr, uint16_t varSize, uint8_t varTypeArchitecture, uint8_t varPrimitiveId, uint8_t fobjectType) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return FARAABIN_DATABUS_NOT_INITIALIZED_ERROR;  /* MISRA C:2012 Rule 15.5 deviation */
  }

  for(uint16_t i = 0U; i < me->ChannelQty; i++) {

    if(me->pBufferChannels[i].ItemFobjectPtr == 0U) {

      return fFaraabinFobjectDataBus_AttachVariableToChannel(me, i, varPtr, varSize, varTypeArchitecture, varPrimitiveId, fobjectType); /* MISRA C:2012 Rule 15.5 deviation */
    }
  }

  return FARAABIN_DATABUS_IS_FULL_ERROR;
}

/**
 * @brief Attaches a variable to a specific channel of the databus.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number of the databus.
 * @param varPtr Pointer to the variable.
 * @param varSize Size of the variable.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectDataBus_AttachVariableToChannel(
  sFaraabinFobjectDataBus *me,
  uint16_t channel,
  uint32_t varPtr,
  uint16_t varSize,
  uint8_t varTypeArchitecture,
  uint8_t varPrimitiveId,
  uint8_t fobjectType) {
    
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR;                 /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return FARAABIN_DATABUS_NOT_INITIALIZED_ERROR;            /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(channel >= me->ChannelQty) {
    return FARAABIN_DATABUS_CHANNEL_INDEX_ERROR;              /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(varPtr == 0U) {
    return FARAABIN_DATABUS_ACTION_WITH_NULL_REFERENCE_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }

  me->pBufferChannels[channel].ItemFobjectType = fobjectType;
  me->pBufferChannels[channel].ItemFobjectPtr = varPtr;
  me->pBufferChannels[channel].ItemFobjectParam = varSize;
  me->pBufferChannels[channel].VariableDataType = varTypeArchitecture;
  me->pBufferChannels[channel].PrimitiveVariableId = varPrimitiveId;
  me->pBufferChannels[channel].Enable = true;

  me->AttachedItemsQty++;
  me->AvailableItemsQty++;

  return FARAABIN_OK;
}

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
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectDataBus_AttachEntityNumericalToChannel(sFaraabinFobjectDataBus *me, uint16_t channel, uint32_t varPtr, uint16_t varSize, uint8_t varTypeArchitecture, uint8_t varPrimitiveId) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR;                 /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return FARAABIN_DATABUS_NOT_INITIALIZED_ERROR;            /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(channel >= me->ChannelQty) {
    return FARAABIN_DATABUS_CHANNEL_INDEX_ERROR;              /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(varPtr == 0U) {
    return FARAABIN_DATABUS_ACTION_WITH_NULL_REFERENCE_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }

  me->pBufferChannels[channel].ItemFobjectType = (uint8_t)FO_TYPE_ENTITY_NUMERICAL;
  me->pBufferChannels[channel].ItemFobjectPtr = varPtr;
  me->pBufferChannels[channel].ItemFobjectParam = varSize;
  me->pBufferChannels[channel].VariableDataType = varTypeArchitecture;
  me->pBufferChannels[channel].PrimitiveVariableId = varPrimitiveId;
  me->pBufferChannels[channel].Enable = true;

  me->AttachedItemsQty++;
  me->AvailableItemsQty++;

  return FARAABIN_OK;
}

/**
 * @brief Attaches a code block fobject to a free channel of the databus so the code block can sends its data and events through it.
 * 
 * @param me Pointer to the databus fobject.
 * @param cbPtr Pointer to the code block.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectDataBus_AttachCodeBlock(sFaraabinFobjectDataBus *me, uint32_t cbPtr) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR;       /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return FARAABIN_DATABUS_NOT_INITIALIZED_ERROR;  /* MISRA C:2012 Rule 15.5 deviation */
  }

  for(uint16_t i = 0U; i < me->ChannelQty; i++) {

    if(me->pBufferChannels[i].ItemFobjectPtr == 0U) {

      return fFaraabinFobjectDataBus_AttachCodeBlockToChannel(me, i, cbPtr);  /* MISRA C:2012 Rule 15.5 deviation */
    }
  }

  return FARAABIN_DATABUS_IS_FULL_ERROR;
}

/**
 * @brief Attaches a code block fobject to a specific channel of the databus so the code block can sends its data and events through it.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number.
 * @param cbPtr Pointer to the code block.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectDataBus_AttachCodeBlockToChannel(sFaraabinFobjectDataBus *me, uint16_t channel, uint32_t cbPtr) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR;                   /* MISRA C:2012 Rule 15.5 deviation */
  }

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  if(!me->_init) {
    return FARAABIN_DATABUS_NOT_INITIALIZED_ERROR;              /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(channel >= me->ChannelQty) {
    return FARAABIN_DATABUS_CHANNEL_INDEX_ERROR;                /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(cbPtr == 0U) {
    return FARAABIN_DATABUS_ACTION_WITH_NULL_REFERENCE_ERROR;   /* MISRA C:2012 Rule 15.5 deviation */
  }

  sCpuProcess *cb = (sCpuProcess*)cbPtr;                        /* MISRA C:2012 Rule 11.4 deviation */
  if(cb->DataBusPtr != 0U) {
            
    return FARAABIN_DATABUS_CODEBLOCK_CALLBACK_NOT_EMPTY_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }

  cb->DataBusChannel = channel;
  cb->DataBusPtr = (uint32_t)me;                                /* MISRA C:2012 Rule 11.4 deviation */

  me->pBufferChannels[channel].ItemFobjectPtr = cbPtr;
  me->pBufferChannels[channel].ItemFobjectType = (uint8_t)FO_TYPE_CODE_BLOCK;
  me->pBufferChannels[channel].ItemFobjectParam = 0U;
  me->pBufferChannels[channel].Enable = true;

  me->AttachedItemsQty++;
  me->AvailableItemsQty++;

  return FARAABIN_OK;
#else
  return FARAABIN_DATABUS_NOT_INITIALIZED_ERROR;
#endif
}

/**
 * @brief Detaches an item from specific channel of the databus.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectDataBus_DetachFromChannel(sFaraabinFobjectDataBus *me, uint16_t channel) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1;
    return FARAABIN_NOT_INITIALIZED_ERROR;                                       /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return FARAABIN_DATABUS_NOT_INITIALIZED_ERROR;                                  /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(channel >= me->ChannelQty) {
    return FARAABIN_DATABUS_CHANNEL_INDEX_ERROR;                                    /* MISRA C:2012 Rule 15.5 deviation */
  }

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  if(me->pBufferChannels[channel].ItemFobjectType == (uint8_t)FO_TYPE_CODE_BLOCK) {
    
    sCpuProcess *cb = (sCpuProcess*)(me->pBufferChannels[channel].ItemFobjectPtr); /* MISRA C:2012 Rule 11.4 deviation */
    cb->DataBusPtr = 0U;

  }
#endif

  me->pBufferChannels[channel].ItemFobjectParam = 0U;
  me->pBufferChannels[channel].ItemFobjectPtr = 0U;
  me->pBufferChannels[channel].ItemFobjectType = 0U;
  me->pBufferChannels[channel].Enable = false;

  me->AttachedItemsQty--;
  me->AvailableItemsQty--;

  return FARAABIN_OK;
}

/**
 * @brief Detaches all attached items in a databus.
 * 
 * @param me Pointer to the databus fobject.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fFaraabinFobjectDataBus_DetachAllChannels(sFaraabinFobjectDataBus *me) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR;       /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!me->_init) {
    return FARAABIN_DATABUS_NOT_INITIALIZED_ERROR;  /* MISRA C:2012 Rule 15.5 deviation */
  }

  uint8_t result = FARAABIN_OK;

  for(uint16_t i = 0; i < me->ChannelQty; i++) {

    result = fFaraabinFobjectDataBus_DetachFromChannel(me, i);

    if(result != FARAABIN_OK) {
      return result;                                /* MISRA C:2012 Rule 15.5 deviation */
    }
  }

  me->AttachedItemsQty = 0U;
  me->AvailableItemsQty = 0U;

  return FARAABIN_OK;
}

/**
 * @brief Runs the interface for sending codeblock data through databus.
 * 
 * @note This interface is used to send the high speed info of the code block.
 * 
 * @param cbPtr Pointer to the code block.
 * @param startTick Start tick of the last entrance to the code block.
 * @param endTick End tick of the last exit from the code block.
 */
void fFaraabinFobjectDataBus_RunCodeBlock(uint32_t cbPtr, uint32_t startTick, uint32_t endTick) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return;                                                               /* MISRA C:2012 Rule 15.5 deviation */
  }

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  
  sCpuProcess *cb = (sCpuProcess*)cbPtr;                                  /* MISRA C:2012 Rule 11.4 deviation */
  sFaraabinFobjectDataBus *db = (sFaraabinFobjectDataBus*)cb->DataBusPtr; /* MISRA C:2012 Rule 11.4 deviation */

  if(db->Enable == false) {
    return;                                                               /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(!db->pBufferChannels[cb->DataBusChannel].Enable) {
    return;                                                               /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  switch(db->CurrentState) {
    
    case DATABUS_STATE_STREAM: {

      if(cb->IsSendingEventsToDbStreamEnabled) {

        uint32_t tmp[2];
        tmp[0] = startTick;
        tmp[1] = endTick;
        
        Faraabin_EventSystem_ParamEnd_(cbPtr, &cb->_seq, cb->_enable, eCB_EVENT_INFO_RUN, (uint8_t*)&tmp, 8);

      }
      
      break;
    }
    
    case DATABUS_STATE_TIMER:
    case DATABUS_STATE_TRIG_WAIT:
    case DATABUS_STATE_TRIG_WINDOW: {

      if(cb->ISSendingEventsToDbCaptureEnabled) {

        FARAABIN_CRITICAL_ENTER_;
        
        sFaraabinFobjectDataBus_CaptureValue tmp_capture;
            
        tmp_capture.FobjectType = (uint8_t)FO_TYPE_CODE_BLOCK;
        tmp_capture.FobjectPtr = cbPtr;
        tmp_capture.CapturedTimeStamp = fChrono_GetTick();
        uByte8 tmp;
        tmp.U32[0] = startTick;
        tmp.U32[1] = endTick;
        tmp_capture.CapturedValue = tmp.U64;
        
        fQueueInsert(db, (uint8_t*)&(tmp_capture));
        
        FARAABIN_CRITICAL_EXIT_;

      }
      
      break;
    }
    
    default: {

      // Do nothing.
      break;
    }
  }
#endif
}

/**
 * @brief Detects if the channel has been trigged.
 * 
 * @param me Pointer to the databus fobject.
 */
static void fDetectChannelTrig(sFaraabinFobjectDataBus *me) {
  
  if(!me->ChTrigEnable) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(me->ChTrigNo >= me->ChannelQty) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(!me->pBufferChannels[me->ChTrigNo].Enable) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  uint8_t *ptr = (uint8_t*)me->pBufferChannels[me->ChTrigNo].ItemFobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  uByte8 tmp = {(uint8_t)(0x00U)};
  
  switch((uint8_t)me->pBufferChannels[me->ChTrigNo].PrimitiveVariableId) {
    
    case VAR_DATA_TYPE_PRIMITIVE_BOOL: {
      
      bool value = (bool)ptr[0];
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.Byte[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value != me->_trigChannelPayload.Byte[0]) && (me->_trigChannelPayload.Byte[0] == false)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value != me->_trigChannelPayload.Byte[0]) && (me->_trigChannelPayload.Byte[0] == true)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.Byte[0] = (uint8_t)value;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_UINT8: {
      
      uint8_t value = (uint8_t)ptr[0];
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.Byte[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.Byte[0]) && (me->_trigChannelPayload.Byte[0] <= me->ChTrigThreshold.Byte[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.Byte[0]) && (me->_trigChannelPayload.Byte[0] >= me->ChTrigThreshold.Byte[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.Byte[0] = value;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_INT8: {
      
      int8_t value = (int8_t)ptr[0];
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_CHANGE: {
          
          if(value != (int8_t)me->_trigChannelPayload.Byte[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value > (int8_t)me->ChTrigThreshold.Byte[0]) && ((int8_t)me->_trigChannelPayload.Byte[0] <= (int8_t)me->ChTrigThreshold.Byte[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value < (int8_t)me->ChTrigThreshold.Byte[0]) && ((int8_t)me->_trigChannelPayload.Byte[0] >= (int8_t)me->ChTrigThreshold.Byte[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.Byte[0] = value;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_UINT16: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      uint16_t value = tmp.U16[0];
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.U16[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.U16[0]) && (me->_trigChannelPayload.U16[0] <= me->ChTrigThreshold.U16[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.U16[0]) && (me->_trigChannelPayload.U16[0] >= me->ChTrigThreshold.U16[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.U16[0] = value;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_INT16: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      int16_t value = tmp.I16[0];
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.I16[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.I16[0]) && (me->_trigChannelPayload.I16[0] <= me->ChTrigThreshold.I16[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.I16[0]) && (me->_trigChannelPayload.I16[0] >= me->ChTrigThreshold.I16[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.I16[0] = value;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_UINT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      uint32_t value = tmp.U32[0];
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.U32[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.U32[0]) && (me->_trigChannelPayload.U32[0] <= me->ChTrigThreshold.U32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.U32[0]) && (me->_trigChannelPayload.U32[0] >= me->ChTrigThreshold.U32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.U32[0] = value;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_INT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      int32_t value = tmp.I32[0];
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.I32[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.I32[0]) && (me->_trigChannelPayload.I32[0] <= me->ChTrigThreshold.I32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.I32[0]) && (me->_trigChannelPayload.I32[0] >= me->ChTrigThreshold.I32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.I32[0] = value;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_UINT64: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      tmp.Byte[4] = ptr[4];
      tmp.Byte[5] = ptr[5];
      tmp.Byte[6] = ptr[6];
      tmp.Byte[7] = ptr[7];
      uint64_t value = tmp.U64;
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.U64) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.U64) && (me->_trigChannelPayload.U64 <= me->ChTrigThreshold.U64)) {
            
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.U64) && (me->_trigChannelPayload.U64 >= me->ChTrigThreshold.U64)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.U64 = value;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_INT64: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      tmp.Byte[4] = ptr[4];
      tmp.Byte[5] = ptr[5];
      tmp.Byte[6] = ptr[6];
      tmp.Byte[7] = ptr[7];
      int64_t value = tmp.I64;
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.I64) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.I64) && (me->_trigChannelPayload.I64 <= me->ChTrigThreshold.I64)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.I64) && (me->_trigChannelPayload.I64 >= me->ChTrigThreshold.I64)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.I64 = value;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_FLOAT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      float32_t value = tmp.F32[0];
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.F32[0]) && (me->_trigChannelPayload.F32[0] <= me->ChTrigThreshold.F32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.F32[0]) && (me->_trigChannelPayload.F32[0] >= me->ChTrigThreshold.F32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.F32[0] = value;
      
      break;
    }
    
    case VAR_DATA_TYPE_PRIMITIVE_FLOAT64: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      tmp.Byte[4] = ptr[4];
      tmp.Byte[5] = ptr[5];
      tmp.Byte[6] = ptr[6];
      tmp.Byte[7] = ptr[7];
      float64_t value = tmp.F64;
      
      switch(me->ChTrigType) {
        
        case DATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.F64) && (me->_trigChannelPayload.F64 <= me->ChTrigThreshold.F64)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case DATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.F64) && (me->_trigChannelPayload.F64 >= me->ChTrigThreshold.F64)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = DATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        default: {
          // Do nothing.
          break;
        }
      }
      
      me->_trigChannelPayload.F64 = value;
      
      break;
    }
    
    default: {

      // Do nothing.
      break;
    }
  }
}

/**
 * @brief Runs the databus engine in capture state.
 * 
 * @param me Pointer to the databus fobject.
 */
static void fRunCapture(sFaraabinFobjectDataBus *me) {
  
  FARAABIN_CRITICAL_ENTER_;
  
  // Send value
  for(uint16_t i = 0U; i < me->ChannelQty; i++) {

    if(me->pBufferChannels[i].ItemFobjectPtr == 0U) {
      continue;
    }

    if(!me->pBufferChannels[i].Enable) {
      continue;
    }
      
    switch(me->pBufferChannels[i].ItemFobjectType) {
      
      case FO_TYPE_VAR:
      case FO_TYPE_ENTITY_NUMERICAL: {
        
        sFaraabinFobjectDataBus_CaptureValue tmp_capture;
        
        if(me->pBufferChannels[i].PrimitiveVariableId != 0U) {
        
          tmp_capture.FobjectType = me->pBufferChannels[i].ItemFobjectType;
          tmp_capture.FobjectPtr = me->pBufferChannels[i].ItemFobjectPtr;
          tmp_capture.CapturedTimeStamp = fChrono_GetTick();
          
          uint8_t *src = (uint8_t*)me->pBufferChannels[i].ItemFobjectPtr;  /* MISRA C:2012 Rule 11.4 deviation */
          uint8_t *dst = (uint8_t*)&(tmp_capture.CapturedValue);
          for(uint16_t j = 0; j < me->pBufferChannels[i].ItemFobjectParam; j++) {
            dst[j] = src[j];
          }
          fQueueInsert(me, (uint8_t*)&(tmp_capture));
        }
    
        break;
      }
      
      default: {
        
        // Do nothing.
        break;
      }
    }
  }
  
  FARAABIN_CRITICAL_EXIT_;
}

/**
 * @brief Clears the databus queue.
 * 
 * @param me Pointer to the databus fobject.
 */
static void fQueueClear(sFaraabinFobjectDataBus * const me) {
  
  me->QueueItemCount = 0U;
  me->_queueFrontIndex = 0U;
  me->_queueRearIndex = 0U;

}

/**
 * @brief Inserts an item to the databus queue.
 * 
 * @param me Pointer to the databus fobject.
 * @param item Item to be inserted.
 */
static void fQueueInsert(sFaraabinFobjectDataBus * const me, uint8_t *item) {
  
  uint32_t offset;
  uint8_t over_write = 0;
  
  if(me->QueueItemCount >= me->BufferCaptureSize) {
    over_write = 1U;
  }
  
  //Write item
  offset = me->_queueRearIndex * sizeof(sFaraabinFobjectDataBus_CaptureValue);
  for(uint32_t i=0; i<sizeof(sFaraabinFobjectDataBus_CaptureValue); i++) {
    uint8_t *tmp = (uint8_t*)me->pBufferCapture;
    tmp[offset + i] = item[i];
  }
      
  //Increment rear index
  me->_queueRearIndex = (me->_queueRearIndex + 1) % me->BufferCaptureSize;
  
  if(over_write == 1U) {

    //Increment front index
    me->_queueFrontIndex   = (me->_queueFrontIndex + 1) % me->BufferCaptureSize;
  } else {
    me->QueueItemCount++;
  }
}

/**
 * @brief Reads items from the databus queue.
 * 
 * @param me Pointer to the databus fobject.
 * @param num Number of items to read.
 * @param item Pointer to a buffer for copying the read item.
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fQueueRead(sFaraabinFobjectDataBus * const me, uint32_t num, uint8_t *item) {
  
  uint32_t offset;
  uint32_t index;
  
  if(me->QueueItemCount == 0U) {
    return FARAABIN_DATABUS_QUEUE_EMPTY_ERROR;    /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(num >= me->QueueItemCount) {
    return FARAABIN_DATABUS_CHANNEL_INDEX_ERROR;  /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  index = (num + me->_queueFrontIndex) % me->BufferCaptureSize;

  //Read item
  offset = index * sizeof(sFaraabinFobjectDataBus_CaptureValue); \
  for(uint32_t i=0; i<sizeof(sFaraabinFobjectDataBus_CaptureValue); i++) { \
    uint8_t *tmp = (uint8_t*)me->pBufferCapture;\
    item[i] = tmp[offset + i]; \
  }

  return FARAABIN_OK;
}

/*
default_fobject
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief Initializes default fobjects.
 * 
 * @note This function is called internally and user does not need to call it manually.
 * 
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fDefaultFobjects_Init(void) {
  
  #if defined(FB_FEATURE_FLAG_DEFAULT_DATABUS)
    
		FARAABIN_DATABUS_SET_VALUE_(DefaultDatabus.ChannelQty, FB_DEFAULT_DATABUS_CHANNEL_QTY);
		FARAABIN_DATABUS_SET_VALUE_(DefaultDatabus.pBufferChannels, DefaultDatabusChannelBuffer);
		FARAABIN_DataBus_Init_WithPath_(&DefaultDatabus, DefaultFobjectPath);
    
    FARAABIN_DataBus_StartStreamMode_(&DefaultDatabus);
    FARAABIN_DataBus_Enable_(&DefaultDatabus);
    
    fChrono_StartTimeoutMs(&DefaultDatabusRunner, FB_DEFAULT_DATABUS_CYCLE_MS);
    
  #endif
    
  #if defined(FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP)
    
    FARAABIN_EventGroup_Init_WithPath_(&DefaultEventGroup, DefaultFobjectPath);
    FARAABIN_EventGroup_Enable_(&DefaultEventGroup);
    
  #endif

    FARAABIN_EventGroup_Init_(&NotificationEventGroup);
    FARAABIN_EventGroup_Enable_(&NotificationEventGroup);
  
    return FARAABIN_OK;
  }
  
/**
 * @brief Runs default databus.
 * 
 */
static void fDefaultFobjects_Run(void) {
  
#if defined(FB_FEATURE_FLAG_DEFAULT_DATABUS)
  
  if(fChrono_IsTimeout(&DefaultDatabusRunner)) {
    
    fChrono_StartTimeoutMs(&DefaultDatabusRunner, FB_DEFAULT_DATABUS_CYCLE_MS);
    
    FARAABIN_DataBus_Run_(&DefaultDatabus);
    
  }
  
#endif
}

/*
fobject_mcu
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief Initializes MCU Fobject.
 * 
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fFobjectMcu_Init(void) {
  
  Faraabin.Init = true;
  if(fDatabase_AddDict((uint32_t)&Faraabin.Mcu) != 0U) {  /* MISRA C:2012 Rule 11.4 deviation */
    return FARAABIN_CANNOT_ADD_DICT_ERROR;                /* MISRA C:2012 Rule 15.5 deviation */
  }
  Faraabin.Init = false;
  
  Faraabin.Mcu._type = (uint8_t)FO_TYPE_MCU;
  Faraabin.Mcu._init = FALSE;
  
  Faraabin.Mcu.Enable = TRUE;
  Faraabin.Mcu.Path = "System";

  Faraabin.Mcu.Seq = 0U;
  
  Faraabin.Mcu._isHostConnected = FALSE;
  
  fChrono_StartTimeoutMs(&(Faraabin.Mcu.ChronoLiveTimeout), FB_LIVE_TIMEOUT_MS);
  
  Faraabin.Mcu.BootTimeMs = 0U;
  Faraabin.Mcu.BootTimeFirstFlag = TRUE;
  
  Faraabin.Mcu._init = TRUE;
  return FARAABIN_OK;
}

/**
 * @brief Runs all statistics in Faraabin related to the MCU Fobject.
 * 
 */
static void fFobjectMcu_Run(void) {

  if(!Faraabin.Mcu._init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(Faraabin.Mcu.BootTimeFirstFlag) {
    
    Faraabin.Mcu.BootTimeFirstFlag = false;
    
    Faraabin.Mcu.BootTimeMs = fChrono_TimeSpanMs(fChrono_GetTickInitValue(), fChrono_GetTick());
  }
  
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  Faraabin.Flags.Status.Bitfield.ProfilerSend = fFaraabinAddOn_CpuProfiler_IsSending();
#else
  Faraabin.Flags.Status.Bitfield.ProfilerSend = false;
#endif
  
  if(fChrono_IsTimeout(&(Faraabin.Mcu.ChronoLiveTimeout)) == true) {
    
    fChrono_Start(&(Faraabin.Mcu.ChronoLiveTimeout));
    
    Faraabin.Mcu._isHostConnected = false;
  }
}

/**
 * @brief Sends system event to Faraabin.
 * 
 * @param eventId ID of the event.
 */
static void fFobjectMcu_SendEventSystem(uint16_t eventId) {
  
  Faraabin_EventSystem_End_((uint32_t)0xFFFFFFFFU, &(Faraabin.Mcu.Seq), Faraabin.Mcu.Enable, eventId);
}

/**
 * @brief Sends system exception event to Faraabin.
 * 
 * @param eventId ID of the event.
 */
static void fFobjectMcu_SendEventSystemException(uint16_t eventId) {
  
  Faraabin_EventSystemException_End_((uint32_t)0xFFFFFFFFU, &(Faraabin.Mcu.Seq), Faraabin.Mcu.Enable, eventId);
}

/**
 * @brief Sends system event response.
 * 
 * @param eventId ID of the event.
 * @param reqSeq Request sequence.
 */
static void fFobjectMcu_SendEventSystemResponse(uint16_t eventId, uint8_t reqSeq, bool isCmdBuffer) {
  
  Faraabin_EventSystem_EndResponse_((uint32_t)0xFFFFFFFFU, &(Faraabin.Mcu.Seq), true, eventId, reqSeq, isCmdBuffer);
}

/**
 * @brief Sends system exception event response.
 * 
 * @param eventId 
 * @param reqSeq 
 */
static void fFobjectMcu_SendEventSystemExceptionResponse(uint16_t eventId, uint8_t reqSeq, bool isCmdBuffer) {
  
  Faraabin_EventSystemException_EndResponse_((uint32_t)0xFFFFFFFFU, &(Faraabin.Mcu.Seq), true, eventId, reqSeq, isCmdBuffer);
}

/*
database
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief Initializes Faraabin database
 * 
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fDatabase_Init(void) {
  
  for(uint16_t i = 1; i < FB_MAX_FOBJECT_QTY; i++) {
    
    Faraabin.Database.DatabaseArray[i] = 0U;
  }
  
  Faraabin.Database.DatabaseArrayIndex = 0U;

  Faraabin.Flags.Features.U32 = 0U;
  Faraabin.Flags.Status.U32 = 0U;
  
#ifdef FB_FEATURE_FLAG_DEFAULT_DATABUS
  Faraabin.Flags.Features.Bitfield.DefaultDataBus = 1U;
#endif
#ifdef FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP
  Faraabin.Flags.Features.Bitfield.DefaultEventGroup = 1U;
#endif

#ifdef FB_FEATURE_FLAG_MCU_CLI
  Faraabin.Flags.Features.Bitfield.McuCli = 1U;
#endif

#ifdef FB_ADD_ON_FEATURE_FLAG_UNITY
  Faraabin.Flags.Features.Bitfield.Unity = 1U;
#endif
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  Faraabin.Flags.Features.Bitfield.CpuProfiler = 1U;
#endif
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
  Faraabin.Flags.Features.Bitfield.StateMachine = 1U;
#endif

#ifdef FB_FEATURE_FLAG_ALLOW_SEND_DICT_BLOCKING
  Flags.Features.Bitfield.AllowSendDickBlocking = 1U;
#endif

  Faraabin.Flags.Status.Bitfield.McuReset = 1U;
  
  Faraabin.Flags.Features.Bitfield.IsChronoTickUp = (CHRONO_TICK_COUNTERMODE == TICK_COUNTERMODE_UP);
  
  return FARAABIN_OK;
}

/**
 * @brief Adds a new dictionary to Faraabin database.
 * 
 * @param fobjectPtr Pointer of the fobject.
 * @return result Can be one of FARAABIN_RET values.
 */
uint8_t fDatabase_AddDict(uint32_t fobjectPtr) {
  
  if(!Faraabin.Init) {
    Faraabin.Flags.Status.Bitfield.UninitializedFaraabin = 1U;
    return FARAABIN_NOT_INITIALIZED_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(fobjectPtr == 0U) {

    Faraabin.Flags.Status.Bitfield.NullDict = 1U;
    Faraabin.Mcu._init = false;
    
    return FARAABIN_DICT_OBJECT_NULL_REFERENCE_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(fDatabase_IsDictExist(fobjectPtr)) {
    
    Faraabin.Flags.Status.Bitfield.DictDuplicate = 1U;
    return FARAABIN_DICT_ALREADY_EXIST_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(Faraabin.Link.DictSendFlag) {
    
    Faraabin.Flags.Status.Bitfield.UnexpectedDict = 1U;
    Faraabin.Mcu._init = false;
    
    return 1; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(Faraabin.Database.DatabaseArrayIndex >= (uint16_t)((uint16_t)FB_MAX_FOBJECT_QTY - (uint16_t)1)) {
    
    Faraabin.Flags.Status.Bitfield.DictOverflow = 1U;
    Faraabin.Mcu._init = false;
    
    return FARAABIN_DICT_DATABASE_IS_FULL_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  Faraabin.Database.DatabaseArray[Faraabin.Database.DatabaseArrayIndex] = fobjectPtr;
  Faraabin.Flags.Status.Bitfield.NewDict = 1U;
  
  Faraabin.Database.DatabaseArrayIndex++;
  
  return FARAABIN_OK;
}

/**
 * @brief Check for existance of dict in list
 * 
 * @param fobjectPtr Pointer of the fobject.
 * @return result Can be one of FARAABIN_RET values.
 */
static bool fDatabase_IsDictExist(uint32_t fobjectPtr) {
  
  for(uint32_t i = 0; i < FB_MAX_FOBJECT_QTY; i++) {\
    
    if(Faraabin.Database.DatabaseArray[i] == fobjectPtr) {
      return true;  /* MISRA C:2012 Rule 15.5 deviation */
    }
  }
  
  return false;
}

/*
function_engine
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief Initializes the function engine that executes all functions and function groups in Faraabin.
 * 
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fFunctionEngine_Init(void) {

  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    Faraabin.FunctionsList[i].Function = NULL;
    if(memset(Faraabin.FunctionsList[i].Param, 0x00, sizeof(Faraabin.FunctionsList[i].Param)) == NULL) {
      return FARAABIN_FENGINE_MEMSET_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
    }
    Faraabin.FunctionsList[i].IsFirstRun = false;
    Faraabin.FunctionsList[i]._result = 0U;
    Faraabin.FunctionsList[i].fpUserTerminalCallback = NULL;
    Faraabin.FunctionsList[i].Status = FN_STATUS_STOP;
    Faraabin.FunctionsList[i].ObjectPtr = 0U;
    Faraabin.FunctionsList[i].IsBusy = false;
    Faraabin.FunctionsList[i].IsRunning = false;
    
    fChrono_Stop(&(Faraabin.FunctionsList[i]._chrono));

  }
  
  return FARAABIN_OK;
}

/**
 * @brief Runs Faraabin function engine.
 * 
 */
static void fFunctionEngine_Run(void) {
  
  // Run Function
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    sFunctionEngine_Item *item = &(Faraabin.FunctionsList[i]);
    
    if(item->IsRunning == false) {
      continue;
    }
    
    if(item->Function->FunctionPtr != NULL) {
      
      if(item->IsFirstRun) {
        fChrono_Start(&(item->_chrono));
      }
      uint8_t res = item->Function->FunctionPtr(item->ObjectPtr, (char*)item->Param, item->IsFirstRun);
      item->IsFirstRun = false;
      
      if(res != FUNCTION_CONTINUE) {
        
        item->IsBusy = false;
        item->IsRunning = false;
          
        uByte4 executeUs;
        executeUs.U32 = fChrono_ElapsedUs(&(item->_chrono));
        
        uint8_t arg[5];
        arg[0] = res;
        arg[1] = executeUs.Byte[0];
        arg[2] = executeUs.Byte[1];
        arg[3] = executeUs.Byte[2];
        arg[4] = executeUs.Byte[3];
        
        Faraabin_EventSystemExtended_ParamEnd_((uint32_t)item->Function, item->ObjectPtr, &(item->Function->Seq), true, FN_EVENT_INFO_STOP, arg, sizeof(arg)); /* MISRA C:2012 Rule 11.4 deviation */
        
      }
    }
  }
}

/**
 * @brief Starts a function for execution.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @param arg Pointer to the buffer containing the function arguments.
 * @param argSize Size of function arguments.
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fFunctionEngine_Start(uint32_t functionPtr, uint32_t objectPtr, uint8_t* arg, uint16_t argSize) {

  if(fFn_FindRunningFunction(functionPtr, objectPtr) != NULL) {
    
    return FARAABIN_FENGINE_FUNCTION_ALREADY_RUNNING_ERROR;     /* MISRA C:2012 Rule 15.5 deviation */
  }

  uint8_t freeIndex = fFn_GetFreeIndex();
  if(freeIndex == 255U) {

    return FARAABIN_FENGINE_MAX_CONCURRENT_LIMIT_REACHED_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }

  for(uint16_t i = 0; i < argSize; i++) {
    
    Faraabin.FunctionsList[freeIndex].Param[i] = arg[i];
  }

  Faraabin.FunctionsList[freeIndex].Param[argSize] = 0x00U;
  
  Faraabin.FunctionsList[freeIndex].ObjectPtr = objectPtr;
  Faraabin.FunctionsList[freeIndex].IsFirstRun = true;
  
  Faraabin.FunctionsList[freeIndex].Function = (sFaraabinFobjectFunction*)functionPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  Faraabin.FunctionsList[freeIndex].IsBusy = true;
  Faraabin.FunctionsList[freeIndex].IsRunning = true;

  return FARAABIN_OK;
}

/**
 * @brief Stops a function fobject from execution.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fFunctionEngine_Stop(uint32_t functionPtr, uint32_t objectPtr) {

  sFunctionEngine_Item* item = (sFunctionEngine_Item*)fFn_Find(functionPtr, objectPtr);

  if(item == NULL) {
    return FARAABIN_FENGINE_FUNCTION_NOT_IN_LIST_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }

  item->IsBusy = false;
  item->IsRunning = false;

  return FARAABIN_OK;
}

/**
 * @brief Pauses function fobject from execution.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fFunctionEngine_Pause(uint32_t functionPtr, uint32_t objectPtr) {

  sFunctionEngine_Item* item = (sFunctionEngine_Item*)fFn_Find(functionPtr, objectPtr);

  if(item == NULL) {
    return FARAABIN_FENGINE_FUNCTION_NOT_IN_LIST_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }

  item->IsBusy = true;
  item->IsRunning = false;

  return FARAABIN_OK;
}

/**
 * @brief Resumes an already paused function.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fFunctionEngine_Resume(uint32_t functionPtr, uint32_t objectPtr) {

  sFunctionEngine_Item* item = (sFunctionEngine_Item*)fFn_Find(functionPtr, objectPtr);

  if(item == NULL) {
    return FARAABIN_FENGINE_FUNCTION_NOT_IN_LIST_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }

  item->IsBusy = true;
  item->IsRunning = true;

  return FARAABIN_OK;
}

/**
 * @brief Gets the number of concurrent running function fobjects.
 * 
 * @return numberOfFuncs Number of running functions.
 */
static uint16_t fFunctionEngine_GetQtyOfRunningFunctions(void) {
  
  uint16_t cnt = 0U;
  
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    if(Faraabin.FunctionsList[i].IsRunning == true) {
      
      cnt++;
    }
  }
  
  return cnt;
}

/**
 * @brief Gets the pointer of the function fobjects that are running by the engine.
 * 
 * @param index Index in function engine list.
 * @param ptr Pointer of the function fobject.
 * @param objectPtr Pointer of the object attached to the function group.
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fFunctionEngine_GetPtrsOfRunningFunction(uint16_t index, uint32_t *ptr, uint32_t *objectPtr) {
  
  if(index >= fFunctionEngine_GetQtyOfRunningFunctions()) {
    return FARAABIN_FENGINE_FUNCTION_INDEX_ERROR;             /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  uint16_t cnt = 0U;
  
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    if(Faraabin.FunctionsList[i].IsRunning == true) {
      
      if(cnt == index) {
        
        *ptr = (uint32_t)Faraabin.FunctionsList[i].Function;  /* MISRA C:2012 Rule 11.4 deviation */
        *objectPtr = Faraabin.FunctionsList[i].ObjectPtr;     /* MISRA C:2012 Rule 11.4 deviation */
        
        break;
      }
      
      cnt++;
    }
  }
  
  return FARAABIN_OK;
}

/**
 * @brief Finds a function in engine's list.
 * 
 * @param functionPtr Pointer of the function.
 * @return objectPtr Pointer of the object attached to function group.
 */
static sFunctionEngine_Item* fFn_Find(uint32_t functionPtr, uint32_t objectPtr) {
  
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    if(
      ((uint32_t)(Faraabin.FunctionsList[i].Function) == functionPtr) &&  /* MISRA C:2012 Rule 11.4 deviation */
      ((uint32_t)(Faraabin.FunctionsList[i].ObjectPtr) == objectPtr)) {   /* MISRA C:2012 Rule 11.4 deviation */
      return &(Faraabin.FunctionsList[i]);                                /* MISRA C:2012 Rule 15.5 deviation */
    }
  }
  
  return NULL;
}

/**
 * @brief Finds a running function in the list.
 * 
 * @param functionPtr Pointer of the function.
 * @return objectPtr Pointer of the object attached to function group.
 */
static sFunctionEngine_Item* fFn_FindRunningFunction(uint32_t functionPtr, uint32_t objectPtr) {
  
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    if(
      ((uint32_t)(Faraabin.FunctionsList[i].Function) == functionPtr) &&  /* MISRA C:2012 Rule 11.4 deviation */
      ((uint32_t)(Faraabin.FunctionsList[i].ObjectPtr) == objectPtr)) {   /* MISRA C:2012 Rule 11.4 deviation */
      
      if(Faraabin.FunctionsList[i].IsRunning) {
        return &(Faraabin.FunctionsList[i]);                              /* MISRA C:2012 Rule 15.5 deviation */
      }
    }
  }
  
  return NULL;
}

/**
 * @brief Gets the index of free place in engine's list.
 * 
 * @return numOfFree Free place index in the list. If list is full, '255' is returned.
 */
static uint8_t fFn_GetFreeIndex(void) {

  uint8_t ret = (uint8_t)0xFFU;
  
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    if(!Faraabin.FunctionsList[i].IsBusy) {
      
      ret = i;
      break;
    }
  }
  
  return ret;
}

/*
link_buffer
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief Initializes faraabin link buffer.
 * 
 * @param txBuffer Pointer to the TX buffer.
 * @param size Size allocated for the buffer.
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fLinkDataBuffer_Init(uint8_t *txBuffer, uint32_t size) {

  if(size == 0U) {
    return FARAABIN_LINK_TX_BUFFER_SIZE_ERROR;                                      /* MISRA C:2012 Rule 15.5 deviation */
  }
  Faraabin.DataCircularBuffer.Size = size;
  
  if(txBuffer != NULL) {
    Faraabin.DataCircularBuffer.Buffer = txBuffer;
  } else {

    return FARAABIN_LINK_TX_BUFFER_NULL_ERROR;                                    /* MISRA C:2012 Rule 15.5 deviation */
  }

  fLinkDataBuffer_Clear();
  
  return FARAABIN_OK;
}

/**
 * @brief Clear data buffer of the Faraabin link.
 * 
 */
static void fLinkDataBuffer_Clear(void) {
  
  Faraabin.DataCircularBuffer._head = 0U;
  Faraabin.DataCircularBuffer._tail = 0U;
  Faraabin.DataCircularBuffer._isFull = false;
  
  for(uint32_t i = 0; i < Faraabin.DataCircularBuffer.Size; i++) {
    Faraabin.DataCircularBuffer.Buffer[i] = 0x00U;
  }
}

/**
 * @brief Initalize Faraabin link command buffer.
 * 
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fLinkCmdBuffer_Init(void) {

  /*
  //This condition is always false, but can be used in the future.
  if(Faraabin.CmdBuffer == NULL) {
    return FARAABIN_CMD_LINK_NULL_ERROR;
  }
  */

  Faraabin.CmdCircularBuffer.Size = CMD_BUFFER_SIZE;
  Faraabin.CmdCircularBuffer.Buffer = Faraabin.CmdBuffer;

  fLinkCmdBuffer_Clear();
  
  return FARAABIN_OK;
}

/**
 * @brief Clear Faraabin link command buffer.
 * 
 */
static void fLinkCmdBuffer_Clear(void) {
  
  Faraabin.CmdCircularBuffer._head = 0U;
  Faraabin.CmdCircularBuffer._tail = 0U;
  Faraabin.CmdCircularBuffer._isFull = false;
  
  for(uint32_t i = 0; i < CMD_BUFFER_SIZE; i++) {
    Faraabin.CmdCircularBuffer.Buffer[i] = 0x00U;
  }
}

/*
link_deserializer
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief Deserilizes a frame.
 * 
 * @param buffer Pointer to the buffer that contains the data of the frame.
 * @param size Size of the frame.
 * @param deserializedFrame Pointer to the deserialized frame.
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fFaraaninLinkDeserializer_Deserialize(uint8_t * const buffer, uint16_t size, sClientFrame *deserializedFrame) {
  
  if(size < MINIMUM_FRAME_SIZE) {
    return FARAABIN_DESERIALIZE_MINIMUM_FRAME_SIZE_ERROR; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!fIsChecksumOk(buffer, size)) {
    return FARAABIN_DESERIALIZE_CHECKSUM_ERROR;           /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  deserializedFrame->Control = buffer[CLIENT_FRAME_CONTROL_OFFSET];
  deserializedFrame->FobjectProperty = buffer[CLIENT_FRAME_PROPERTY_OFFSET];
  if(memcpy(&deserializedFrame->FobjectPtr, &buffer[CLIENT_FRAME_POINTER_OFFSET], sizeof(deserializedFrame->FobjectPtr)) == NULL) {
    return FARAABIN_DESERIALIZE_MEMORY_ERROR;             /* MISRA C:2012 Rule 15.5 deviation */
  }
  deserializedFrame->Payload = &buffer[CLIENT_FRAME_PAYLOAD_OFFSET];
  deserializedFrame->PayloadSize = size - MINIMUM_FRAME_SIZE;

  return FARAABIN_OK;
}

/**
 * @brief Calculates the checksum of the frame and checks if it is correct. 
 * 
 * @param buffer Pointer to the buffer of the frame.
 * @param size Size of the frame.
 * @return ChecksuIsValid true if valid and false if it is incorrect.
 */
static bool fIsChecksumOk(uint8_t *buffer, uint16_t size) {
  
  uint8_t checksum = 0;
  
  for (uint16_t i = 0; i < size; i++) {
    
    checksum += buffer[i];
    
  }

  return (checksum != (uint8_t)(0xFFU)) ? false : true;
}

/*
link_handler
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief Link.Initializes link handler of faraabin.
 * 
 * @note This function is called internally in faraabin.c file.
 * 
 * @param rxBuffer Pointer to the RX buffer that serializer needs to keep transmitting data.
 * @param rxBufferSize Size of the RX buffer.
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fLinkHandler_Init(uint8_t *rxBuffer, size_t rxBufferSize) {
  
  Faraabin.Link.Init = false;
  
  Faraabin.Link.Password = NULL;
	Faraabin.Link.RxCharBufferIndex = ((uint16_t)0U);
  
  if(rxBufferSize < 20U) {
    return FARAABIN_LINK_ALLOCATED_RX_BUFFER_SIZE_ERROR;  /* MISRA C:2012 Rule 15.5 deviation */
  }

  Faraabin.Link.RxCharBufferSize = rxBufferSize;

  if(rxBuffer != NULL) {
    Faraabin.Link.RxCharBuffer = rxBuffer;
  } else {
    return FARAABIN_LINK_RX_BUFFER_NULL_ERROR;          /* MISRA C:2012 Rule 15.5 deviation */
  }

  for(size_t i = 0; i < rxBufferSize; i++) {
    Faraabin.Link.RxCharBuffer[i] = 0x00U;
  }

  Faraabin.Link.IsNewFrameDetected = false;
  Faraabin.Link.IsFlushingBuffer = false;
  Faraabin.Link.DictReqSeq = 0U;
  Faraabin.Link.DictSendFlag = false;

  Faraabin.Link.Init = true;
  return FARAABIN_OK;
}

/**
 * @brief Runs the link handler priodically and handles frame if new one is detected.
 * 
 */
static void fLinkHandler_Run(void) {

  if(!Faraabin.Link.Init) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(Faraabin.Link.FlagReceiveFrameBigSize) {
    Faraabin.Link.FlagReceiveFrameBigSize = false;

    fFobjectMcu_SendEventSystemException(MCU_EVENT_ERROR_RX_FRAME_BIG_SIZE);
  }

  //Detect end of frame pattern
  Faraabin.Link.RxPatternIndex = 0;
  for(uint16_t i = 0; i < Faraabin.Link.RxCharBufferIndex; i++) {
  
    uint8_t c = Faraabin.Link.RxCharBuffer[i];
    if(c == EOF_PATTERN[Faraabin.Link.RxPatternIndex]) {
      Faraabin.Link.RxPatternIndex++;

      if(Faraabin.Link.RxPatternIndex >= sizeof(EOF_PATTERN)) {
          Faraabin.Link.RxPatternIndex = 0;
          
          Faraabin.Link.IsNewFrameDetected = true;
      }
    } else {
      Faraabin.Link.RxPatternIndex = 0;
      if(c == EOF_PATTERN[Faraabin.Link.RxPatternIndex]) {
        Faraabin.Link.RxPatternIndex++;
      }
    }  
  }

  // Frame handling
  if(Faraabin.Link.IsNewFrameDetected) {

    uint8_t ret = fFaraaninLinkDeserializer_Deserialize(Faraabin.Link.RxCharBuffer, Faraabin.Link.RxCharBufferIndex - sizeof(EOF_PATTERN), &(Faraabin.Link.ClientFrame));
      
    switch(ret) {
      
      case FARAABIN_OK: {
        
        fFrameHandler(&(Faraabin.Link.ClientFrame));
        break;
      }
  
      case FARAABIN_DESERIALIZE_CHECKSUM_ERROR: {
        
        fFobjectMcu_SendEventSystemException(MCU_EVENT_ERROR_RX_FRAME_CHECKSUM);
        break;
      }
  
      case FARAABIN_DESERIALIZE_MINIMUM_FRAME_SIZE_ERROR: {
        
        fFobjectMcu_SendEventSystemException(MCU_EVENT_ERROR_RX_FRAME_SMALL_SIZE);
        break;
      }
  
      default: {
        fFobjectMcu_SendEventSystemException(MCU_EVENT_ERROR_UNDEF);
        break;
      }
    }
          
    Faraabin.Link.RxCharBufferIndex = ((uint16_t)0U);
    
    Faraabin.Link.IsNewFrameDetected = false;
  }

  fSendCircularBuffer(false);
  
  // Dict handling
  if(Faraabin.Link.DictSendFlag == true) {
    
    if(Faraabin.Link.DictIsBlocking && Faraabin.Flags.Features.Bitfield.AllowSendDickBlocking) {

      for(uint16_t i = 0; i < Faraabin.Database.DatabaseArrayIndex; i++) {

        fLinkSerializer_SerializeDict(Faraabin.Database.DatabaseArray[i], i, Faraabin.Link.DictReqSeq);
        fSendCircularBuffer(true);

      }
      Faraabin.Link.DictSendFlag = false;
      fFobjectMcu_SendEventSystemResponse(MCU_EVENT_INFO_DICT_END, Faraabin.Link.DictReqSeq, false);
      Faraabin.Flags.Status.Bitfield.NewDict = 0U;

      fFaraabin_DictSendingCpltCallback();
      
    } else {
      
      static uint16_t dictIndex = 0;

       fLinkSerializer_SerializeDict(Faraabin.Database.DatabaseArray[dictIndex], dictIndex, Faraabin.Link.DictReqSeq);
       fSendCircularBuffer(true);
       dictIndex++;

       if(dictIndex >= Faraabin.Database.DatabaseArrayIndex) {

         dictIndex = 0U;
         Faraabin.Link.DictSendFlag = false;
         fFobjectMcu_SendEventSystemResponse(MCU_EVENT_INFO_DICT_END, Faraabin.Link.DictReqSeq, false);
         Faraabin.Flags.Status.Bitfield.NewDict = 0U;

         fFaraabin_DictSendingCpltCallback();
       }
     }
  }
}

/**
 * @brief Handles new valid frame.
 * 
 * @param clientFrame Pointer to the client frame.
 */
static void fFrameHandler(sClientFrame *clientFrame) {
  
  uint8_t *fobjectType = 
    (clientFrame->FobjectPtr == 0xFFFFFFFFU) ? (uint8_t*)&(Faraabin.Mcu) : (uint8_t*)clientFrame->FobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  bool flushBuffer = (ClientFrame_FlushBuffer_(clientFrame->Control) == 0) ? false : true;
  if(flushBuffer) {
    
    fLinkDataBuffer_Clear();
    fLinkCmdBuffer_Clear();
  }

  switch(*fobjectType) {
    
    case FO_TYPE_MCU: {
      
      fFrameHandler_Mcu(clientFrame);
      break;
    }
    
    case FO_TYPE_DATABUS: {
      
      fFrameHandler_DataBus(clientFrame);
      break;
    }
    
    case FO_TYPE_CODE_BLOCK: {
      
      fFrameHandler_CodeBlock(clientFrame);
      break;
    }
    
    case FO_TYPE_STATE_MACHINE: {
      
      fFrameHandler_StateMachine(clientFrame);
      break;
    }
    
    case FO_TYPE_STATE_MACHINE_STATE: {
      
      fFrameHandler_StateMachineState(clientFrame);
      break;
    }
    
    case FO_TYPE_STATE_MACHINE_TRANSITION: {
      
      fFrameHandler_StateMachineTransition(clientFrame);
      break;
    }
    
    case FO_TYPE_FUNCTION: {
      
      fFrameHandler_Function(clientFrame);
      break;
    }
    
    case FO_TYPE_EVENT_GROUP: {
      
      fFrameHandler_EventGroup(clientFrame);
      break;
    }
    
    default: {

      uint8_t controlReqSeq = ClientFrame_RequestSequence_(clientFrame->Control);
      bool isCmdBuffer = (ClientFrame_IsBufferCmdResponse_(clientFrame->Control) == 0) ? false : true;
      fFobjectMcu_SendEventSystemExceptionResponse(MCU_EVENT_ERROR_UNSUPPORTED_FOBJECT_TYPE, controlReqSeq, isCmdBuffer);
      break;
    }
  }
}

/**
 * @brief Handles MCU frames received from the link.
 * 
 * @param clientFrame Pointer to the client frame.
 */
static void fFrameHandler_Mcu(sClientFrame *clientFrame) {
  
  bool errorFobjectProperty = false;
  
  if(clientFrame->FobjectPtr == 0xFFFFFFFFU) {
    clientFrame->FobjectPtr = (uint32_t)&(Faraabin.Mcu);  /* MISRA C:2012 Rule 11.4 deviation */
  }

  uint8_t framePropGroup = ClientFrame_PropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_PropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_RequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_AccessType_(clientFrame->Control);
  bool isCmdBuffer = (ClientFrame_IsBufferCmdResponse_(clientFrame->Control) == 0) ? false : true;

  switch((uint8_t)framePropGroup) {
    
    case FB_PROP_GROUP_DICT: {
      
      uByte2 dictIndex;
      dictIndex.Byte[0] = clientFrame->Payload[0];
      dictIndex.Byte[1] = clientFrame->Payload[1];
      
      if(dictIndex.U16 >= Faraabin.Database.DatabaseArrayIndex) {

        fFobjectMcu_SendEventSystemExceptionResponse(MCU_EVENT_ERROR_DICT_INDEX_OVERFLOW, controlReqSeq, isCmdBuffer);
        break;
      }
      
      fLinkSerializer_SerializeDict(Faraabin.Database.DatabaseArray[dictIndex.U16], dictIndex.U16, controlReqSeq);
      
      fFobjectMcu_SendEventSystemResponse(MCU_EVENT_INFO_DICT_END, controlReqSeq, isCmdBuffer);
      
      break;
    }
    
    case FB_PROP_GROUP_SETTING: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_MCU_PROP_ID_SETTING_ENABLE: {
          
          if(controlAccessType == (uint8_t)CLIENT_FRAME_ACCESS_TYPE_WRITE) {

            Faraabin.Mcu.Enable = (*clientFrame->Payload == 0U) ? false : true;
          }
          
          if(controlReqSeq != 0U) {
            fLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &(Faraabin.Mcu.Seq), controlReqSeq, true, isCmdBuffer);
          }
          
          break;
        }

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER        
        case FB_MCU_PROP_ID_SETTING_SEND_PROFILER_ENABLE: {
          
          if(controlAccessType == (uint8_t)CLIENT_FRAME_ACCESS_TYPE_WRITE) {

            if(*clientFrame->Payload == 0U) {
              
              fFaraabinAddOn_CpuProfiler_SendingDisable();
            } else {
              
              fFaraabinAddOn_CpuProfiler_SendingEnable();
            }
          }
          
          if(controlReqSeq != 0U) {
            
            fLinkSerializer_McuSendLive(clientFrame->FobjectPtr, controlReqSeq, true, isCmdBuffer);
          }
          
          break;
        }
#endif        
       
        default: {

          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_MONITORING: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_MCU_PROP_ID_MONITORING_VARIABLE: {
          
          uByte4 ptr;
          ptr.Byte[0] = clientFrame->Payload[0];
          ptr.Byte[1] = clientFrame->Payload[1];
          ptr.Byte[2] = clientFrame->Payload[2];
          ptr.Byte[3] = clientFrame->Payload[3];
          
          uByte4 size;
          size.Byte[0] = clientFrame->Payload[4];
          size.Byte[1] = clientFrame->Payload[5];
          size.Byte[2] = clientFrame->Payload[6];
          size.Byte[3] = clientFrame->Payload[7];
          
          bool isExternal = (clientFrame->Payload[8] != 0U);
          
          uByte4 externalFuncPtr;
          externalFuncPtr.Byte[0] = clientFrame->Payload[9];
          externalFuncPtr.Byte[1] = clientFrame->Payload[10];
          externalFuncPtr.Byte[2] = clientFrame->Payload[11];
          externalFuncPtr.Byte[3] = clientFrame->Payload[12];
          
          bool isAccessCb = (clientFrame->Payload[13] != 0U);
          
          uByte4 accessCbFuncPtr;
          accessCbFuncPtr.Byte[0] = clientFrame->Payload[14];
          accessCbFuncPtr.Byte[1] = clientFrame->Payload[15];
          accessCbFuncPtr.Byte[2] = clientFrame->Payload[16];
          accessCbFuncPtr.Byte[3] = clientFrame->Payload[17];
          
          uint8_t *valuePtr = &(clientFrame->Payload[18]);
          
          if(controlAccessType == (uint8_t)CLIENT_FRAME_ACCESS_TYPE_WRITE) {
            
            if(isAccessCb) {
              
              FaraabinVarAccessCallback func = (FaraabinVarAccessCallback)externalFuncPtr.U32;  /* MISRA C:2012 Rule 11.1 deviation */
              if(func != NULL) {
                
                uint8_t res = func(true, ptr.U32, valuePtr, size.U32);
                if(res != 0U) {
                  //TODO: Send error
                }
              }
              
            } else {
              
              //Write value
              uint8_t *dst = (uint8_t*)ptr.U32; /* MISRA C:2012 Rule 11.4 deviation */
              uint8_t *src = valuePtr;
              for(uint32_t i = 0U; i < size.U32; i++) {
                dst[i] = src[i];
              }
            }
            
            FaraabinVarAccessCallback func = (FaraabinVarAccessCallback)accessCbFuncPtr.U32;  /* MISRA C:2012 Rule 11.1 deviation */
            if(func != NULL) {
              
              uint8_t res = func(true, ptr.U32, valuePtr, size.U32);
              if(res != 0U) {
                //TODO: Send error
              }
            }
          }
          
          
          if(controlReqSeq != 0U) {
            
            if(isExternal) {
              
              FaraabinVarAccessCallback func = (FaraabinVarAccessCallback)externalFuncPtr.U32;  /* MISRA C:2012 Rule 11.1 deviation */
              if(func != NULL) {

                uint8_t *dataPtr = NULL;                
                uint8_t res = func(false, ptr.U32, dataPtr, size.U32);

                if(res != 0U) {
                  //TODO: Send error
                } else {
                  fLinkSerializer_VarSendValue(ptr.U32, (uint32_t)dataPtr, size.U32, &(Faraabin.Mcu.Seq), controlReqSeq, true, isCmdBuffer);
                }
              }
              
            } else {
              fLinkSerializer_VarSendValue(ptr.U32, ptr.U32, size.U32, &(Faraabin.Mcu.Seq), controlReqSeq, true, isCmdBuffer);
            }
          }
          
          break;
        }
        
        default: {

          errorFobjectProperty = true;

          break;
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_COMMAND: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_MCU_PROP_ID_COMMAND_PING: {
          
          if(controlReqSeq != 0U) {
      
            fLinkSerializer_McuSendPing((uint32_t)&(Faraabin.Mcu), controlReqSeq, clientFrame->Payload, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
          }
          
          break;
        }
        
        case FB_MCU_PROP_ID_COMMAND_SEND_LIVE: {
          
          fChrono_Start(&(Faraabin.Mcu.ChronoLiveTimeout));
          Faraabin.Mcu._isHostConnected = true;
          
          if(controlReqSeq != 0U) {
          
            fLinkSerializer_McuSendLive((uint32_t)0xFFFFFFFFU, controlReqSeq, true, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_MCU_PROP_ID_COMMAND_SEND_WHOAMI: {
          
          if(controlReqSeq != 0U) {
        
            fLinkSerializer_McuSendWhoAmI((uint32_t)&(Faraabin.Mcu), controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
            
            Faraabin.Flags.Status.Bitfield.McuReset = 0U;
          }
          
          break;
        }
        
        case FB_MCU_PROP_ID_COMMAND_SEND_ALL_DICT: {
          
          if(Faraabin.Flags.Features.Bitfield.AllowSendDickBlocking) {
            Faraabin.Link.DictIsBlocking = (clientFrame->Payload[0] == 0U) ? false : true;
          } else {
            Faraabin.Link.DictIsBlocking = false;
          }
          
          uByte2 tmp2;
          tmp2.Byte[0] = clientFrame->Payload[1];
          tmp2.Byte[1] = clientFrame->Payload[2];
          Faraabin.Mcu.SessionId = tmp2.U16;
          
          bool sendAllow = true;
          if(Faraabin.Flags.Features.Bitfield.Password == 1U) {

            if(strncmp(Faraabin.Link.Password, (char*)(&(clientFrame->Payload[3])), strlen(Faraabin.Link.Password)) != 0) {

              sendAllow = false;
            }
          }
          
          if(sendAllow) {
          
            Faraabin.Link.DictSendFlag = true;
            
            //Set flag for  send all dict
            fLinkDataBuffer_Clear();
            fLinkCmdBuffer_Clear();
            Faraabin.Link.DictReqSeq = controlReqSeq;
          } else {
            
            Faraabin_EventSystemException_EndResponse_((uint32_t)&(Faraabin.Mcu), /* MISRA C:2012 Rule 11.4 deviation */
                                        &(Faraabin.Mcu.Seq), 
                                        Faraabin.Mcu.Enable, 
                                        MCU_EVENT_ERROR_PASSWORD, 
                                        controlReqSeq, isCmdBuffer);
                        
          }
          
          break;
        }
        
        case FB_MCU_PROP_ID_COMMAND_RESET_CPU: {

          fFaraabin_ResetMcu();

          Faraabin_EventSystemException_EndResponse_((uint32_t)&(Faraabin.Mcu), /* MISRA C:2012 Rule 11.4 deviation */
                                        &(Faraabin.Mcu.Seq), 
                                        Faraabin.Mcu.Enable, 
                                        MCU_EVENT_ERROR_RESET_FUNC_NOT_IMPLEMENTED, 
                                        controlReqSeq, isCmdBuffer);
          
          break;
        }
        
        case FB_MCU_PROP_ID_COMMAND_CLEAR_FLAG_BUFFER_OVF: {
          
          Faraabin.DataCircularBuffer._isOvf = false;
          Faraabin.CmdCircularBuffer._isOvf = false;

          Faraabin.Flags.Status.Bitfield.DataBufferOverflow = 0U;
          Faraabin.Flags.Status.Bitfield.CmdBufferOverflow = 0U;

          Faraabin_EventSystem_EndResponse_((uint32_t)&(Faraabin.Mcu),  /* MISRA C:2012 Rule 11.4 deviation */
                                        &(Faraabin.Mcu.Seq), 
                                        Faraabin.Mcu.Enable, 
                                        MCU_EVENT_INFO_CLEAR_FLAG_BUFFER_OVF, 
                                        controlReqSeq, isCmdBuffer);
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_EVENT: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_COMMON_PROP_ID_EVENT_USER_TERMINAL: {
          
          clientFrame->Payload[clientFrame->PayloadSize] = 0x00U; // Terminate end of data
          if(Faraabin.Mcu.fpUserTerminalCallback != NULL) {
            Faraabin.Mcu.fpUserTerminalCallback(clientFrame->Payload, clientFrame->PayloadSize);
          }
          
          if(controlReqSeq != 0U) {
            fFobjectMcu_SendEventSystemResponse(MCU_EVENT_INFO_USER_DATA_RECEIVED, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    default: {
      
      errorFobjectProperty = true;
      break;
    }
  }
  
  if(errorFobjectProperty) {
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)&(Faraabin.Mcu),  /* MISRA C:2012 Rule 11.4 deviation */
                                        &(Faraabin.Mcu.Seq),
                                        Faraabin.Mcu.Enable,
                                        MCU_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq,
                                        isCmdBuffer);
  }
}

/**
 * @brief Handles databus frames received from the link.
 * 
 * @param clientFrame Pointer to the client frame.
 */
static void fFrameHandler_DataBus(sClientFrame* clientFrame) {
  
  bool errorFobjectProperty = false;
  
  sFaraabinFobjectDataBus *dbHandle = (sFaraabinFobjectDataBus*)clientFrame->FobjectPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  uint8_t *param = clientFrame->Payload;
  uint8_t framePropGroup = ClientFrame_PropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_PropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_RequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_AccessType_(clientFrame->Control);
  bool isCmdBuffer = (ClientFrame_IsBufferCmdResponse_(clientFrame->Control) == 0) ? false : true;

  switch((uint8_t)framePropGroup) {

    case FB_PROP_GROUP_SETTING: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_DB_PROP_ID_SETTING_ENABLE: {

              if(controlAccessType == (uint8_t)CLIENT_FRAME_ACCESS_TYPE_WRITE) {
          
                dbHandle->Enable = (*clientFrame->Payload == 0U) ? false : true;
              }
              
              if(controlReqSeq != 0U) {
                fLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, true, isCmdBuffer);
              }
              
              break;
            }
        
            case FB_DB_PROP_ID_SETTING_ALL: {

              if(controlReqSeq != 0U) {
            
                fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
              }
              
              break;
            }
            
            case FB_DB_PROP_ID_SETTING_ITEM_ENABLE: {

              uByte2 channelNo;
              channelNo.Byte[0] = param[0];
              channelNo.Byte[1] = param[1];
              
              bool itemEnable = (param[0] != 0U);
              
              if(channelNo.U16 >= dbHandle->ChannelQty) {
            
                Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_ERROR_NOT_FREE_LOCATION_FOR_ATTACH, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
                return; /* MISRA C:2012 Rule 15.5 deviation */
            
              } else {
            
                if(dbHandle->pBufferChannels[channelNo.U16].ItemFobjectPtr != 0U) {
                  dbHandle->pBufferChannels[channelNo.U16].Enable = itemEnable;
              
                  if(itemEnable == true) {
              
                    dbHandle->AvailableItemsQty++;
                
                  } else {
                
                    dbHandle->AvailableItemsQty--;
                
                  }
                }
            
              }
          
              if(controlReqSeq != 0U) {
          
                fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
              }
          
              break;
        }
            
        case FO_DB_PROP_ID_SETTING_API_TRIG_ENABLE: {

          dbHandle->ApiTrigEnable = (param[0] != 0U);
          
          if(controlReqSeq != 0U) {
          
            fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FO_DB_PROP_ID_SETTING_CH_TRIG_ENABLE: {

          dbHandle->ChTrigEnable = (param[0] != 0U);
          
          if(controlReqSeq != 0U) {
          
            fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FO_DB_PROP_ID_SETTING_CH_TRIG_CONFIG: {

          uByte2 chNo;
          chNo.Byte[0] = param[0];
          chNo.Byte[1] = param[1];
  
          uint8_t chTrigType = param[2];
  
          if(dbHandle->ChTrigNo != chNo.U16) {
            fFaraabinFobjectDataBus_ResetTrigger(dbHandle);
          }
          dbHandle->ChTrigNo = chNo.U16;
          dbHandle->ChTrigType = chTrigType;
          for(int i=0; i<8; i++) {
            dbHandle->ChTrigThreshold.Byte[i] = param[3 + i];
          }
          
          if(controlReqSeq != 0U) {
          
            fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FO_DB_PROP_ID_SETTING_STREAM_DIVIDEBY: {

              uByte2 divideBy;
              divideBy.Byte[0] = param[0];
              divideBy.Byte[1] = param[1];
              
              if(divideBy.U16 == 0) {
            
                Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_ERROR_PARAM, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
                return; /* MISRA C:2012 Rule 15.5 deviation */
            
              } else {
            
                dbHandle->StreamDivideBy = divideBy.U16;
              }
          
              if(controlReqSeq != 0U) {
          
                fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
              }
          
              break;
        }
        
        case FO_DB_PROP_ID_SETTING_TIMER_DIVIDEBY: {

              uByte2 divideBy;
              divideBy.Byte[0] = param[0];
              divideBy.Byte[1] = param[1];
              
              if(divideBy.U16 == 0) {
            
                Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_ERROR_PARAM, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
                return; /* MISRA C:2012 Rule 15.5 deviation */
            
              } else {
            
                dbHandle->TimerDivideBy = divideBy.U16;
              }
          
              if(controlReqSeq != 0U) {
          
                fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
              }
          
              break;
        }
        
        case FO_DB_PROP_ID_SETTING_TRIG_DIVIDEBY: {

              uByte2 divideBy;
              divideBy.Byte[0] = param[0];
              divideBy.Byte[1] = param[1];
              
              if(divideBy.U16 == 0) {
            
                Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_ERROR_PARAM, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
                return; /* MISRA C:2012 Rule 15.5 deviation */
            
              } else {
            
                dbHandle->TrigDivideBy = divideBy.U16;
              }
          
              if(controlReqSeq != 0U) {
          
                fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
              }
          
              break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_MONITORING: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_DB_PROP_ID_MONITORING_CAPTURE_VALUE:
        case FB_DB_PROP_ID_MONITORING_STREAM_VALUE: {

          /* Do nothing! */
          break;
        }

        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_COMMAND: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_DB_PROP_ID_COMMAND_ATTACH: {
          
          uint8_t itemFobjectType = param[0];
  
          uByte4 itemFobjectPtr;
          itemFobjectPtr.Byte[0] = param[1];
          itemFobjectPtr.Byte[1] = param[2];
          itemFobjectPtr.Byte[2] = param[3];
          itemFobjectPtr.Byte[3] = param[4];
  
          uByte2 itemFobjectParam;
          itemFobjectParam.Byte[0] = param[5];
          itemFobjectParam.Byte[1] = param[6];
  
          uint8_t varTypeArchitecture = param[7];
          uint8_t varPrimitiveId = param[8];
  
          uByte2 itemChannelNo;
          itemChannelNo.Byte[0] = param[9];
          itemChannelNo.Byte[1] = param[10];
          
          uint8_t ret = FARAABIN_OK;

          if(itemFobjectType == (uint8_t)FO_TYPE_CODE_BLOCK) {
            ret = fFaraabinFobjectDataBus_AttachCodeBlockToChannel(dbHandle, itemChannelNo.U16, itemFobjectPtr.U32);
          } else {
            ret = fFaraabinFobjectDataBus_AttachVariableToChannel(dbHandle, itemChannelNo.U16, itemFobjectPtr.U32, itemFobjectParam.U16, varTypeArchitecture, varPrimitiveId, itemFobjectType);
          }

          if(ret != FARAABIN_OK) {

            Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_ERROR_ATTACH, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
          } else {
            
            if(controlReqSeq != 0U) {
          
              fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
            }
          }

          break;
        }
        
        case FB_DB_PROP_ID_COMMAND_DETACH: {
          
          uByte2 itemChannelNo;
          itemChannelNo.Byte[0] = param[0];
          itemChannelNo.Byte[1] = param[1];
          
          uint8_t ret = FARAABIN_OK;

          ret = fFaraabinFobjectDataBus_DetachFromChannel(dbHandle, itemChannelNo.U16);

          if(ret != FARAABIN_OK) {

            Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_ERROR_DETACH, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
          } else {
            
            if(controlReqSeq != 0U) {
          
              fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
            }
          }

          break;
        }
        
        case FB_DB_PROP_ID_COMMAND_DETACH_ALL: {
          
          if(fFaraabinFobjectDataBus_DetachAllChannels(dbHandle) != FARAABIN_OK) {

            Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_ERROR_CODE_BLOCK_NOT_EMPTY, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
          }
      
          if(controlReqSeq != 0U) {
            
            fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_DB_PROP_ID_COMMAND_START_STREAM: {
          
          uByte2 streamDivideBy;
          streamDivideBy.Byte[0] = param[0];
          streamDivideBy.Byte[1] = param[1];
          dbHandle->StreamDivideBy = streamDivideBy.U16;
          
          fFaraabinFobjectDataBus_StartStream(dbHandle);
            
          uint8_t eventParam[7];
          
          eventParam[0] = (uint8_t)dbHandle->CurrentState;
          
          uByte2 tmp2;
          tmp2.U16 = dbHandle->StreamDivideBy;
          eventParam[1] = tmp2.Byte[0];
          eventParam[2] = tmp2.Byte[1];
          
          uByte4 tmp4;
          tmp4.U32 = dbHandle->CycleUs;
          eventParam[3] = tmp4.Byte[0];
          eventParam[4] = tmp4.Byte[1];
          eventParam[5] = tmp4.Byte[2];
          eventParam[6] = tmp4.Byte[3];
          
          Faraabin_EventSystem_ParamEndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_INFO_STATE_CHANGE, eventParam, 7U, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
          
          break;
        }
        
        case FB_DB_PROP_ID_COMMAND_START_TRIG: {
          
          if(dbHandle->BufferCaptureSize > 0U) {
      
            uByte4 timeAfterTrigMs;
            timeAfterTrigMs.Byte[0] = param[0];
            timeAfterTrigMs.Byte[1] = param[1];
            timeAfterTrigMs.Byte[2] = param[2];
            timeAfterTrigMs.Byte[3] = param[3];
            dbHandle->TimeAfterTrigMs = timeAfterTrigMs.U32;
            
            uByte2 trigDivideBy;
            trigDivideBy.Byte[0] = param[4];
            trigDivideBy.Byte[1] = param[5];
            dbHandle->TrigDivideBy = trigDivideBy.U16;
            
            fFaraabinFobjectDataBus_StartTrigger(dbHandle);
            
            uint8_t eventParam[11];
            
            eventParam[0] = (uint8_t)dbHandle->CurrentState;
            
            uByte4 tmp4;
            tmp4.U32 = dbHandle->TimeAfterTrigMs;
            eventParam[1] = tmp4.Byte[0];
            eventParam[2] = tmp4.Byte[1];
            eventParam[3] = tmp4.Byte[2];
            eventParam[4] = tmp4.Byte[3];
            
            uByte2 tmp2;
            tmp2.U16 = dbHandle->TrigDivideBy;
            eventParam[5] = tmp2.Byte[0];
            eventParam[6] = tmp2.Byte[1];
            
            tmp4.U32 = dbHandle->CycleUs;
            eventParam[7] = tmp4.Byte[0];
            eventParam[8] = tmp4.Byte[1];
            eventParam[9] = tmp4.Byte[2];
            eventParam[10] = tmp4.Byte[3];
            
            Faraabin_EventSystem_ParamEndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_INFO_STATE_CHANGE, eventParam, 11U, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
          }
          
          break;
        }
        
        case FB_DB_PROP_ID_COMMAND_START_TIMER: {
          
          if(dbHandle->BufferCaptureSize > 0U) {
      
            uByte4 timerWindowMs;
            timerWindowMs.Byte[0] = param[0];
            timerWindowMs.Byte[1] = param[1];
            timerWindowMs.Byte[2] = param[2];
            timerWindowMs.Byte[3] = param[3];
            dbHandle->TimerWindowMs = timerWindowMs.U32;
            
            uByte2 timerDivideBy;
            timerDivideBy.Byte[0] = param[4];
            timerDivideBy.Byte[1] = param[5];
            dbHandle->TimerDivideBy = timerDivideBy.U16;
            
            fFaraabinFobjectDataBus_StartTimer(dbHandle);
            
            uint8_t eventParam[11];
            
            eventParam[0] = (uint8_t)dbHandle->CurrentState;
            
            uByte4 tmp4;
            tmp4.U32 = dbHandle->TimerWindowMs;
            eventParam[1] = tmp4.Byte[0];
            eventParam[2] = tmp4.Byte[1];
            eventParam[3] = tmp4.Byte[2];
            eventParam[4] = tmp4.Byte[3];
            
            uByte2 tmp2;
            tmp2.U16 = dbHandle->TimerDivideBy;
            eventParam[5] = tmp2.Byte[0];
            eventParam[6] = tmp2.Byte[1];
            
            tmp4.U32 = dbHandle->CycleUs;
            eventParam[7] = tmp4.Byte[0];
            eventParam[8] = tmp4.Byte[1];
            eventParam[9] = tmp4.Byte[2];
            eventParam[10] = tmp4.Byte[3];
            
            Faraabin_EventSystem_ParamEndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_INFO_STATE_CHANGE, eventParam, 11U, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
          }
          
          break;
        }
        
        case FB_DB_PROP_ID_COMMAND_STOP: {
          
          fFaraabinFobjectDataBus_Stop(dbHandle);
      
          Faraabin_EventSystem_ParamEndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_INFO_STATE_CHANGE, (uint8_t*)&dbHandle->CurrentState, 1, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
          
          break;
        }
        
        case FB_DB_PROP_ID_COMMAND_CAPTURE_SEND: {
          
          dbHandle->CaptureSendingCnt = 0U;
          dbHandle->CaptureSendingReqSeq = controlReqSeq;
          
          dbHandle->CaptureSendingQty = dbHandle->QueueItemCount;
          
          if(dbHandle->CaptureSendingQty == 0U) {
            
            Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_ERROR_NO_CAPTURE_DATA_FOR_SEND, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
          
          } else {
            
            dbHandle->CurrentState = DATABUS_STATE_CAPTURE_SEND;
            
            uint8_t eventParam[5];
            
            eventParam[0] = (uint8_t)dbHandle->CurrentState;
            
            uByte4 tmp4;
            tmp4.U32 = dbHandle->CaptureSendingQty;
            eventParam[1] = tmp4.Byte[0];
            eventParam[2] = tmp4.Byte[1];
            eventParam[3] = tmp4.Byte[2];
            eventParam[4] = tmp4.Byte[3];
            
            Faraabin_EventSystem_ParamResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_INFO_STATE_CHANGE, eventParam, 5U, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
          }
          
          break;
        }
        
        case FB_DB_PROP_ID_COMMAND_MANUAL_TRIG: {
          
          fFaraabinFobjectDataBus_ForceManualTrigger(dbHandle);
            
          Faraabin_EventSystem_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_INFO_MANUAL_TRIG, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
          
          break;
        }
        
        case FB_DB_PROP_ID_COMMAND_ENABLE_ALL_ITEM: {
          
          for(uint16_t i = 0; i < dbHandle->ChannelQty; i++) {
            dbHandle->pBufferChannels[i].Enable = true;
          }
          
          if(controlReqSeq != 0U) {
          
            fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_DB_PROP_ID_COMMAND_DISABLE_ALL_ITEM: {
          
          for(uint16_t i = 0; i < dbHandle->ChannelQty; i++) {
            dbHandle->pBufferChannels[i].Enable = false;
          }
          
          if(controlReqSeq != 0U) {
          
            fLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_EVENT: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_COMMON_PROP_ID_EVENT_USER_TERMINAL: {

          clientFrame->Payload[clientFrame->PayloadSize] = 0x00U; // Terminate end of data
          if(dbHandle->fpUserTerminalCallback != NULL) {
            dbHandle->fpUserTerminalCallback(clientFrame->Payload, clientFrame->PayloadSize);
          }
          
          if(controlReqSeq != 0U) {
            
            Faraabin_EventSystem_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, DATABUS_EVENT_INFO_USER_DATA_RECEIVED, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }

    default: {
      
      errorFobjectProperty = true;
      break;
    }
  }
  
  if(errorFobjectProperty) {
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)dbHandle, /* MISRA C:2012 Rule 11.4 deviation */
                                        &dbHandle->Seq,
                                        dbHandle->Enable,
                                        DATABUS_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq,
                                        isCmdBuffer);
  }
}

/**
 * @brief Handles codeblock frames received from the link.
 * 
 * @param clientFrame Pointer to the client frame.
 */
static void fFrameHandler_CodeBlock(sClientFrame* clientFrame) {
  
  #ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  
  bool errorFobjectProperty = false;
  
  sCpuProcess *cbHandle = (sCpuProcess*)clientFrame->FobjectPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  uint8_t *param = clientFrame->Payload;
  uint8_t framePropGroup = ClientFrame_PropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_PropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_RequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_AccessType_(clientFrame->Control);
  bool isCmdBuffer = (ClientFrame_IsBufferCmdResponse_(clientFrame->Control) == 0) ? false : true;

  switch((uint8_t)framePropGroup) {
    
    case FB_PROP_GROUP_SETTING: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_CB_PROP_ID_SETTING_ENALBE: {
          
          if(controlAccessType == (uint8_t)CLIENT_FRAME_ACCESS_TYPE_WRITE) {
      
            cbHandle->_enable = (*clientFrame->Payload == 0U) ? false : true;
          }
          
          if(controlReqSeq != 0U) {
            fLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq, true, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_CB_PROP_ID_SETTING_ALL: {
          
          if(controlReqSeq != 0U) {
        
            fLinkSerializer_CodeBlockSendSetting(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_CB_PROP_ID_SETTING_LIMIT_PARAM: {
          
          uByte4 intervalThrMaxUs;  
          intervalThrMaxUs.Byte[0] = param[0];
          intervalThrMaxUs.Byte[1] = param[1];
          intervalThrMaxUs.Byte[2] = param[2];
          intervalThrMaxUs.Byte[3] = param[3];
          
          uByte4 intervalThrMinUs;
          intervalThrMinUs.Byte[0] = param[4];
          intervalThrMinUs.Byte[1] = param[5];
          intervalThrMinUs.Byte[2] = param[6];
          intervalThrMinUs.Byte[3] = param[7];
          
          uByte4 executeThrMaxUs;
          executeThrMaxUs.Byte[0] = param[8];
          executeThrMaxUs.Byte[1] = param[9];
          executeThrMaxUs.Byte[2] = param[10];
          executeThrMaxUs.Byte[3] = param[11];
          
          uByte4 executeThrMinUs;
          executeThrMinUs.Byte[0] = param[12];
          executeThrMinUs.Byte[1] = param[13];
          executeThrMinUs.Byte[2] = param[14];
          executeThrMinUs.Byte[3] = param[15];
      
          cbHandle->IntervalThresholdMaxUs = intervalThrMaxUs.U32;
          cbHandle->IntervalThresholdMinUs = intervalThrMinUs.U32;
          cbHandle->ExecuteThresholdMaxUs = executeThrMaxUs.U32;
          cbHandle->ExecuteThresholdMinUs = executeThrMinUs.U32;
          
          if(controlReqSeq != 0U) {
            
            fLinkSerializer_CodeBlockSendSetting(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_CB_PROP_ID_SETTING_EVENT_TO_STREAM_ENABLE: {
          
          cbHandle->IsSendingEventsToDbStreamEnabled = !(param[0] == 0U);
          
          if(controlReqSeq != 0U) {
            
            fLinkSerializer_CodeBlockSendSetting(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_CB_PROP_ID_SETTING_EVENT_TO_CAPTURE_ENABLE: {
          
          cbHandle->ISSendingEventsToDbCaptureEnabled = !(param[0] == 0U);
          
          if(controlReqSeq != 0U) {
            
            fLinkSerializer_CodeBlockSendSetting(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_CB_PROP_ID_SETTING_DATA_TO_STREAM_ENABLE: {
          
          cbHandle->IsSendingDataToDbStreamEnabled = !(param[0] == 0U);
          
          if(controlReqSeq != 0U) {
            
            fLinkSerializer_CodeBlockSendSetting(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;  
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_MONITORING: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_CB_PROP_ID_MONITORING_VALUE: {
          
          if(controlReqSeq != 0U) {
            fLinkSerializer_CodeBlockSendValue(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq, true, isCmdBuffer);
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break; 
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_COMMAND: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_CB_PROP_ID_COMMAND_RESET_VALUE: {
          
          fCpuProfiler_ProcessResetStatistics(cbHandle);
          
          Faraabin_EventSystem_EndResponse_((uint32_t)cbHandle, &cbHandle->_seq, cbHandle->_enable, eCB_EVENT_INFO_RESET, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
      
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break; 
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_EVENT: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_COMMON_PROP_ID_EVENT_USER_TERMINAL: {
          
          clientFrame->Payload[clientFrame->PayloadSize] = 0x00U; // Terminate end of data
//          if(cbHandle->fpUserTerminalCallback != NULL) {
//            cbHandle->fpUserTerminalCallback(clientFrame->Payload, clientFrame->PayloadSize);
//          }
          
//          if(controlReqSeq != 0U) {
//            
//            Faraabin_EventSystem_EndResponse_((uint32_t)cbHandle, &cbHandle->Seq, cbHandle->Enable, eCB_EVENT_INFO_USER_DATA_RECEIVED, controlReqSeq);
//          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break; 
        }
      }
      
      break;
    }
    
    default: {
      
      errorFobjectProperty = true;
      break;
    }
  }
  
  if(errorFobjectProperty) {
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)cbHandle, /* MISRA C:2012 Rule 11.4 deviation */
                                        &cbHandle->_seq,
                                        cbHandle->_enable,
                                        eCB_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq, 
                                        isCmdBuffer);
  }
  
  #endif
}

/**
 * @brief Handles state machine frames received from the link.
 * 
 * @param clientFrame Pointer to the client frame.
 */
static void fFrameHandler_StateMachine(sClientFrame* clientFrame) {
  
  #ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
  
  bool errorFobjectProperty = false;
  
  sStateMachine *smHandle = (sStateMachine*)clientFrame->FobjectPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  uint8_t *payload = clientFrame->Payload;
  uint8_t framePropGroup = ClientFrame_PropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_PropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_RequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_AccessType_(clientFrame->Control);
  bool isCmdBuffer = (ClientFrame_IsBufferCmdResponse_(clientFrame->Control) == (uint8_t)(0U)) ? false : true;

  switch((uint8_t)framePropGroup) {
    
    case FB_PROP_GROUP_SETTING: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_SM_PROP_ID_SETTING_ENALBE: {
          
          if(controlAccessType == (uint8_t)CLIENT_FRAME_ACCESS_TYPE_WRITE) {
      
            smHandle->_enable = (*clientFrame->Payload == (uint8_t)(0U)) ? false : true;
          }
          
          if(controlReqSeq != (uint8_t)(0U)) {
            fLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &smHandle->_seq, controlReqSeq, true, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_SM_PROP_ID_SETTING_ALL: {
          
          if(controlReqSeq != (uint8_t)(0U)) {
        
            fLinkSerializer_StateMachineSendSetting(clientFrame->FobjectPtr, &smHandle->_seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_SM_PROP_ID_SETTING_ENABLE_ALL_TRANSITIONS: {
          
          smHandle->AllTransitionEnable = (payload[0] != (uint8_t)(0U));
          
          if(controlReqSeq != (uint8_t)(0U)) {
            
            fLinkSerializer_StateMachineSendSetting(clientFrame->FobjectPtr, &smHandle->_seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        case FB_SM_PROP_ID_SETTING_ENABLE_ALL_STATE_TRANSITIONS: {
          
          uByte4 statePtr;
          statePtr.Byte[0] = payload[0];
          statePtr.Byte[1] = payload[1];
          statePtr.Byte[2] = payload[2];
          statePtr.Byte[3] = payload[3];
          
          bool enable = (payload[4] != (uint8_t)(0U));
          
      
          sState *state = (sState*)statePtr.U32;  /* MISRA C:2012 Rule 11.4 deviation */
          state->AllTransitionEnable = enable;
          
          if(controlReqSeq != (uint8_t)(0U)) {
            
            fLinkSerializer_StateMachineSendSetting(clientFrame->FobjectPtr, &smHandle->_seq, controlReqSeq, isCmdBuffer);
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_MONITORING: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_SM_PROP_ID_MONITORING_VALUE: {

          if(controlReqSeq != (uint8_t)(0U)) {
            fLinkSerializer_StateMachineSendValue(clientFrame->FobjectPtr, &smHandle->_seq, controlReqSeq, true, isCmdBuffer);
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_COMMAND: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_SM_PROP_ID_COMMAND_MANUAL_TRANSITION: {

          uByte4 transitionPtr;
          transitionPtr.Byte[0] = payload[0];
          transitionPtr.Byte[1] = payload[1];
          transitionPtr.Byte[2] = payload[2];
          transitionPtr.Byte[3] = payload[3];
      
          sTransition *transition = (sTransition*)transitionPtr.U32;  /* MISRA C:2012 Rule 11.4 deviation */
          if(fStateMachine_AdvFeat_ForceManualTransition(transition) == SM_OK) {
            
            Faraabin_EventSystem_EndResponse_((uint32_t)smHandle, &smHandle->_seq, smHandle->_enable, eSM_EVENT_INFO_CMD_MANUAL_TRANSITION, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
            
          } else {
            
            Faraabin_EventSystemException_EndResponse_((uint32_t)smHandle, &smHandle->_seq, smHandle->_enable, eSM_EVENT_ERROR_MANUAL_TRANSITION_SRC_MISMATCH, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
            
          }
          
          break;
        }
        
        case FB_SM_PROP_ID_COMMAND_VIRTUAL_TRANSITION: {

          uByte4 dstStatePtr;
          dstStatePtr.Byte[0] = payload[0];
          dstStatePtr.Byte[1] = payload[1];
          dstStatePtr.Byte[2] = payload[2];
          dstStatePtr.Byte[3] = payload[3];
      
          fStateMachine_AdvFeat_ForceVirtualTransition(smHandle, (sState*)dstStatePtr.U32); /* MISRA C:2012 Rule 11.4 deviation */
          
          Faraabin_EventSystem_EndResponse_((uint32_t)smHandle, &smHandle->_seq, smHandle->_enable, eSM_EVENT_INFO_CMD_VIRTUAL_TRANSITION, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
          
          break;
        }
        
        case FB_SM_PROP_ID_COMMAND_RESET: {

          fStateMachine_Reset(smHandle);
          
          Faraabin_EventSystem_EndResponse_((uint32_t)smHandle, &smHandle->_seq, smHandle->_enable, eSM_EVENT_INFO_CMD_RESET, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_EVENT: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_COMMON_PROP_ID_EVENT_USER_TERMINAL: {

          clientFrame->Payload[clientFrame->PayloadSize] = 0x00U; // Terminate end of data
//          if(smHandle->fpUserDataReceivedEvent != NULL) {
//            sStateMachine_UserDataReceivedEventArgs ev;
//            
//            ev.pUserData = clientFrame->Payload;
//            ev.UserDataSize = clientFrame->PayloadSize;
//            smHandle->fpUserDataReceivedEvent((object)smHandle, &ev);
//          }
          
          if(controlReqSeq != 0U) {
            Faraabin_EventSystem_EndResponse_((uint32_t)smHandle, &smHandle->_seq, smHandle->_enable, eSM_EVENT_INFO_USER_DATA_RECEIVED, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    default: {
      
      errorFobjectProperty = true;
      break;
    }
  }
  
  if(errorFobjectProperty) {
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)smHandle, /* MISRA C:2012 Rule 11.4 deviation */
                                        &smHandle->_seq,
                                        smHandle->_enable,
                                        eSM_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq,
                                        isCmdBuffer);
  }
  
  #endif
}

/**
 * @brief Handles state machine's state frames received from the link.
 * 
 * @param clientFrame Pointer to the client frame.
 */
static void fFrameHandler_StateMachineState(sClientFrame* clientFrame) {
  
  #ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
  
  bool errorFobjectProperty = false;
  
  sState *smStateHandle = (sState*)clientFrame->FobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  uint8_t framePropGroup = ClientFrame_PropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_PropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_RequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_AccessType_(clientFrame->Control);
  bool isCmdBuffer = (ClientFrame_IsBufferCmdResponse_(clientFrame->Control) == (uint8_t)0) ? false : true;

  switch((uint8_t)framePropGroup) {
    
    case FB_PROP_GROUP_SETTING: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_SM_PROP_ID_SETTING_ENALBE: {
          
          if(controlAccessType == (uint8_t)CLIENT_FRAME_ACCESS_TYPE_WRITE) {
      
            smStateHandle->_enable = (*clientFrame->Payload == (uint8_t)0U) ? false : true;
          }
          
          if(controlReqSeq != (uint8_t)0U) {
            fLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &smStateHandle->_seq, controlReqSeq, true, isCmdBuffer);
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    default: {
      
      errorFobjectProperty = true;
      break;
    }
  }
  
  if(errorFobjectProperty) {
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)smStateHandle,  /* MISRA C:2012 Rule 11.4 deviation */
                                        &smStateHandle->_seq,
                                        smStateHandle->_enable,
                                        eSM_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq,
                                        isCmdBuffer);
  }

  #endif
}

/**
 * @brief Handles state machine's transition frames received from the link.
 * 
 * @param clientFrame Pointer to the client frame.
 */
static void fFrameHandler_StateMachineTransition(sClientFrame* clientFrame) {
  
  #ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
  
  bool errorFobjectProperty = false;
  
  sTransition *smTransitionHandle = (sTransition*)clientFrame->FobjectPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  uint8_t framePropGroup = ClientFrame_PropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_PropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_RequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_AccessType_(clientFrame->Control);
  bool isCmdBuffer = (ClientFrame_IsBufferCmdResponse_(clientFrame->Control) == 0) ? false : true;

  switch((uint8_t)framePropGroup) {
    
    case FB_PROP_GROUP_SETTING: {
      
      uint8_t propId = (uint8_t)framePropId;
      
      switch(propId) {
        
        case FB_SM_PROP_ID_SETTING_ENALBE: {
          
          if(controlAccessType == (uint8_t)CLIENT_FRAME_ACCESS_TYPE_WRITE) {
      
            smTransitionHandle->_enable = (*clientFrame->Payload == (uint8_t)0U) ? false : true;
          }
          
          if(controlReqSeq != (uint8_t)0U) {
            fLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &smTransitionHandle->_seq, controlReqSeq, true, isCmdBuffer);
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    default: {
      
      errorFobjectProperty = true;
      break;
    }
  }
  
  if(errorFobjectProperty) {
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)smTransitionHandle, /* MISRA C:2012 Rule 11.4 deviation */
                                        &smTransitionHandle->_seq,
                                        smTransitionHandle->_enable,
                                        eSM_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq,
                                        isCmdBuffer);
  }
  
  #endif
}

/**
 * @brief Handles function frames received from the link.
 * 
 * @param clientFrame Pointer to the client frame.
 */
static void fFrameHandler_Function(sClientFrame* clientFrame) {
  
  #ifdef FB_FEATURE_FLAG_MCU_CLI
  
  bool errorFobjectProperty = false;
    
  sFaraabinFobjectFunction *fnHandle = (sFaraabinFobjectFunction*)clientFrame->FobjectPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  uint8_t *param = (uint8_t*)clientFrame->Payload;
  uint8_t framePropGroup = ClientFrame_PropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_PropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_RequestSequence_(clientFrame->Control);
  bool isCmdBuffer = (ClientFrame_IsBufferCmdResponse_(clientFrame->Control) == (uint8_t)0U) ? false : true;
  
  switch((uint8_t)framePropGroup) {
    
    case FB_PROP_GROUP_COMMAND: {
      
      uint8_t propId = (uint8_t)framePropId;

      switch(propId) {
        
        case FB_FN_PROP_ID_COMMAND_RUN: {
          
          uByte4 objectPtr;
          objectPtr.Byte[0] = param[0];
          objectPtr.Byte[1] = param[1];
          objectPtr.Byte[2] = param[2];
          objectPtr.Byte[3] = param[3];
          
          uByte2 paramSize;
          paramSize.Byte[0] = param[4];
          paramSize.Byte[1] = param[5];

          uint8_t ret = fFunctionEngine_Start((uint32_t)fnHandle, objectPtr.U32, &(param[6]), paramSize.U16); /* MISRA C:2012 Rule 11.4 deviation */

          if(ret == FARAABIN_FENGINE_FUNCTION_ALREADY_RUNNING_ERROR) {

            Faraabin_EventSystemExceptionExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, FN_EVENT_ERROR_CMD_IS_RUNNING_NOW, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
            
            break;
          }

          if(ret == FARAABIN_FENGINE_MAX_CONCURRENT_LIMIT_REACHED_ERROR) {

            Faraabin_EventSystemExceptionExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, FN_EVENT_ERROR_MAX_CONCURRENT_CMD_REACHED, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
            
            break;
          }

          Faraabin_EventSystemExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, FN_EVENT_INFO_RUN, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
          
          break;
        }
        
        case FB_FN_PROP_ID_COMMAND_STOP: {
          
          uByte4 objectPtr;
          objectPtr.Byte[0] = param[0];
          objectPtr.Byte[1] = param[1];
          objectPtr.Byte[2] = param[2];
          objectPtr.Byte[3] = param[3];
          
          uint8_t ret = fFunctionEngine_Stop(clientFrame->FobjectPtr, objectPtr.U32);

          if(ret == FARAABIN_FENGINE_FUNCTION_NOT_IN_LIST_ERROR) {

            Faraabin_EventSystemExceptionExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, FN_EVENT_ERROR_CMD_NOT_FOUND, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
            
            break;
          }

          Faraabin_EventSystemExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, FN_EVENT_INFO_STOP, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */

          break;
        }
        
        case FB_FN_PROP_ID_COMMAND_PAUSE: {

          uByte4 objectPtr;
          objectPtr.Byte[0] = param[0];
          objectPtr.Byte[1] = param[1];
          objectPtr.Byte[2] = param[2];
          objectPtr.Byte[3] = param[3];
          
          uint8_t ret = fFunctionEngine_Pause(clientFrame->FobjectPtr, objectPtr.U32);
          
          if(ret == FARAABIN_FENGINE_FUNCTION_NOT_IN_LIST_ERROR) {
            
            Faraabin_EventSystemException_EndResponse_((uint32_t)fnHandle, &fnHandle->Seq, true, FN_EVENT_ERROR_CMD_NOT_FOUND, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
            
            break;
          }

          Faraabin_EventSystemExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, FN_EVENT_INFO_PAUSE, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */

          break;
        }
        
        case FB_FN_PROP_ID_COMMAND_RESUME: {

          uByte4 objectPtr;
          objectPtr.Byte[0] = param[0];
          objectPtr.Byte[1] = param[1];
          objectPtr.Byte[2] = param[2];
          objectPtr.Byte[3] = param[3];
          
          uint8_t ret = fFunctionEngine_Resume(clientFrame->FobjectPtr, objectPtr.U32);
          
          if(ret == FARAABIN_FENGINE_FUNCTION_NOT_IN_LIST_ERROR) {
            
            Faraabin_EventSystemException_EndResponse_((uint32_t)fnHandle, &fnHandle->Seq, true, FN_EVENT_ERROR_CMD_NOT_FOUND, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */
            
            break;
            
          }

          Faraabin_EventSystemExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, FN_EVENT_INFO_RESUME, controlReqSeq, isCmdBuffer);  /* MISRA C:2012 Rule 11.4 deviation */

          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    default: {
      // Not supported property group.
      break;
    }
  }
  
  if(errorFobjectProperty) {
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)fnHandle, /* MISRA C:2012 Rule 11.4 deviation */
                                        &fnHandle->Seq,
                                        true,
                                        FN_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq,
                                        isCmdBuffer);
  }
  
  #endif
}

/**
 * @brief Handles event group frames received from the link.
 * 
 * @param clientFrame Pointer to the client frame.
 */
static void fFrameHandler_EventGroup(sClientFrame* clientFrame) {
  
  bool errorFobjectProperty = false;
  
  sFaraabinFobjectEventGroup *egHandle = (sFaraabinFobjectEventGroup*)clientFrame->FobjectPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  uint8_t framePropGroup = ClientFrame_PropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_PropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_RequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_AccessType_(clientFrame->Control);
  bool isCmdBuffer = (ClientFrame_IsBufferCmdResponse_(clientFrame->Control) == 0) ? false : true;

  switch((uint8_t)framePropGroup) {
    
    case FB_PROP_GROUP_SETTING: {
      
      uint8_t propId = (uint8_t)framePropId;

      switch(propId) {
        
        case FB_FN_PROP_ID_SETTING_ENALBE: {
          
          if(controlAccessType == (uint8_t)CLIENT_FRAME_ACCESS_TYPE_WRITE) {
            egHandle->Enable = (*clientFrame->Payload == (uint8_t)0U) ? false : true;
          }
          
          if(controlReqSeq != 0U) {
            fLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &egHandle->Seq, controlReqSeq, true, isCmdBuffer);
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case FB_PROP_GROUP_EVENT: {
      
      uint8_t propId = (uint8_t)framePropId;

      switch(propId) {
        
        case FB_COMMON_PROP_ID_EVENT_USER_TERMINAL: {
          
          clientFrame->Payload[clientFrame->PayloadSize] = (uint8_t)0x00U; // Terminate end of data
          if(egHandle->fpUserTerminalCallback != NULL) {
            egHandle->fpUserTerminalCallback(clientFrame->Payload, clientFrame->PayloadSize);
          }
          
          if(controlReqSeq != 0U) {
            Faraabin_EventSystem_EndResponse_((uint32_t)egHandle, &egHandle->Seq, egHandle->Enable, EG_EVENT_INFO_USER_DATA_RECEIVED, controlReqSeq, isCmdBuffer); /* MISRA C:2012 Rule 11.4 deviation */
          }
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    default: {
      
      errorFobjectProperty = true;
      break;
    }
  }
  
  if(errorFobjectProperty) {
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)egHandle, /* MISRA C:2012 Rule 11.4 deviation */
                                        &egHandle->Seq,
                                        egHandle->Enable,
                                        EG_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq,
                                        isCmdBuffer);
  }
}

/**
 * @brief Transmits available data in TX buffer to the link.
 * 
 * @param flush Forces the function to flsuh the buffer.
 */
static void fSendCircularBuffer(bool flush) {
  
  uint8_t *buffPtr = NULL;
  uint16_t transmitSize = 0U;
  
  if(Faraabin.Link.IsFlushingBuffer) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(flush) {
    Faraabin.Link.IsFlushingBuffer = true;
  }
  
  do{
    
    if(flush) {
  
      while(fFaraabin_IsSending()) {
        
        if(fChrono_IsTimeout(&(Faraabin.Link.ChronoPortSending))) {
          
          fFobjectMcu_SendEventSystemException(MCU_EVENT_ERROR_TX_FRAME_TIMEOUT);

          if(flush) {
            Faraabin.Link.IsFlushingBuffer = false;
          }
          
          return; /* MISRA C:2012 Rule 15.5 deviation */
        }
      }
    } else {
      
      if(fFaraabin_IsSending()) {
        return; /* MISRA C:2012 Rule 15.5 deviation */
      }
    }

    uint8_t lowControl = (uint8_t)1U;

    //Check cmd buffer overflow
    if(Faraabin.CmdCircularBuffer._isFull) {\
      Faraabin.CmdCircularBuffer._isOvf = true;\
    }
  
    transmitSize = fLinkSerializer_FlushCmdBuffer(&buffPtr);
    if(transmitSize == 0U) {
      lowControl = 0U;

      //Check data buffer overflow
      if(Faraabin.DataCircularBuffer._isFull) {\
        Faraabin.DataCircularBuffer._isOvf = true;\
      }
      transmitSize = fLinkSerializer_FlushDataBuffer(&buffPtr);
    }
    if(transmitSize > 0U) {
      if(buffPtr == NULL) {
        //Return mem error
        break;
      }
      fChrono_StartTimeoutMs(&(Faraabin.Link.ChronoPortSending), (transmitSize * FB_BYTE_SENDING_TIME_MS) * 2U);
      
      for(uint16_t i = 0U; i < transmitSize; i++) {
        Faraabin.Link.SendBuffer[i] = buffPtr[i];
      }
      Faraabin.Link.SendBuffer[transmitSize] = lowControl;
      for(uint8_t i = 0U; i < sizeof(HUB_EOF_PATTERN); i++) {
        Faraabin.Link.SendBuffer[transmitSize + 1U + i] = HUB_EOF_PATTERN[i];
      }
      
      if(fFaraabin_Send(Faraabin.Link.SendBuffer, transmitSize + 1U + sizeof(HUB_EOF_PATTERN)) != 0U) {
        
        fFobjectMcu_SendEventSystemException(MCU_EVENT_ERROR_TX_FRAME_SEND);
        
      }
    } else {
      break;
    }
  } while((flush) && (transmitSize > 0U));
  
  if(flush) {
    Faraabin.Link.IsFlushingBuffer = false;
  }
}

/*
link_serializer
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%*/

/**
 * @brief Initializes link serializer module.
 * 
 * @note This function is called internally in faraabin.c file.
 * 
 * @param txBuffer Pointer to the TX buffer that serializer needs to keep transmitting data.
 * @param txBufferSize Size of the TX buffer.
 * @return result Can be one of FARAABIN_RET values.
 */
static uint8_t fLinkSerializer_Init(uint8_t *txBuffer, size_t txBufferSize) {
  
  if(fLinkDataBuffer_Init(txBuffer, txBufferSize) != FARAABIN_OK) {
    return FARAABIN_SERIALIZER_DATA_BUFFER_NOT_INITIALIZED; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(fLinkCmdBuffer_Init() != FARAABIN_OK) {
    return FARAABIN_SERIALIZER_CMD_BUFFER_NOT_INITIALIZED;  /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  for(uint16_t i = 0; i < TEXT_EVENT_MAX_REENTRANCE; i++) {

    for(uint16_t j = 0; j < TEXT_EVENT_BUFFER_SIZE; j++) {

      Faraabin.Serializer.TextEventBuffer[i][j] = 0x00U;
      
    }
  }
  Faraabin.Serializer.DepthCounter = 0U;
  
  Faraabin.Serializer.DictIterator.CurrentSubDictIndex = 0U;
  Faraabin.Serializer.DictIterator.TotalSubDicts = 0U;
  Faraabin.Serializer.DictIterator.CurrentDictIndex = 0U;
  Faraabin.Serializer.DictIterator._isInCountingMode = false;
  
  return FARAABIN_OK;
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
  bool isCmdBuffer) {
    
  if((!isResponse) && !fFaraabin_IsAllowEvent()) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(!fobjectEnableState) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  sEventParam eventParam;

  eventParam.Severity = eventSeverity;
  eventParam.EventId = eventId;
  eventParam.pParam = param;  /* MISRA C:2012 Rule 11.5 deviation */
  eventParam.ParamSize = paramSize;
  eventParam.fpGeneratePayload = generatePayloadFunc;
  eventParam.pPayloadParam = payloadParam;

  fSerializeFrame(
    (isResponse) ? FRAME_TYPE_RESPONSE : FRAME_TYPE_EVENT,
    fobjectSeq,
    reqSeq,
    isEnd,
    fobjectPtr,
    extendedFobjectPtr,
    (uint8_t)FB_PROP_GROUP_EVENT,
    (uint8_t)eventPropId,
    fSerializePayload_Event, &eventParam,
    isCmdBuffer);
}

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
  const char *format, ...) {
    
  if(!fFaraabin_IsAllowEvent()) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(!fobjectEnableState) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  if(format == NULL) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(Faraabin.Serializer.DepthCounter >= (TEXT_EVENT_MAX_REENTRANCE + 1U)) {

    fFobjectMcu_SendEventSystem(MCU_EVENT_ERROR_MAX_PRINTF_REENTRANT);
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  Faraabin.Serializer.DepthCounter++;

  uint16_t reentranceCounter = Faraabin.Serializer.DepthCounter;

  va_list args;           /* MISRA C:2012 Rule 17.1 deviation */
  va_start(args, format); /* MISRA C:2012 Rule 17.1 deviation */

  int stringLen = vsnprintf((char*)Faraabin.Serializer.TextEventBuffer[reentranceCounter - 1U], TEXT_EVENT_BUFFER_SIZE, format, args);  /* MISRA C:2012 Rule 21.6 deviation */
  
  va_end(args);           /* MISRA C:2012 Rule 17.1 deviation */
  
  fFaraabinLink_Serialize_Event(fobjectPtr,
    extendedFobjectPtr,
    fobjectSeq,
    fobjectEnableState,
    eventPropId,
    severity,
    0,
    Faraabin.Serializer.TextEventBuffer[reentranceCounter - 1U],
    stringLen,
    0,
    false,
    NULL,
    NULL,
    true,
    false);
  
  Faraabin.Serializer.DepthCounter--;
}

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
  char *stringPtr) {
    
  if(!fFaraabin_IsAllowEvent()) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(!fobjectEnableState) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  if(stringPtr == NULL) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  fFaraabinLink_Serialize_Event(fobjectPtr,
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
    true,
    false);
}

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
  uint16_t enumId) {
  
  fFaraabinLink_Serialize_Event(fobjectPtr,
    extendedFobjectPtr,
    fobjectSeq,
    fobjectEnableState,
    FB_COMMON_PROP_ID_EVENT_USER_CODE,
    severity,
    enumId,
    (uint8_t*)&enumPtr,
    4,
    0,
    false,
    NULL,
    NULL,
    true,
    false);
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
static void fLinkSerializer_SerializeDict(
  uint32_t fobjectPtr,
  uint16_t dictIndex,
  uint8_t reqSeq) {
  
  fDictIterator_ResetCounter(dictIndex);
  Faraabin.Serializer.DictIterator._isInCountingMode = true;
  fGenerateDict(fobjectPtr, reqSeq);
  Faraabin.Serializer.DictIterator._isInCountingMode = false;
  fGenerateDict(fobjectPtr, reqSeq);
}

/**
 * @brief Flushes main queue of faraabin.
 * 
 * @param ptrToBuffer Pointer to the pointer of faraabin TX buffer.
 * @return num Number of bytes that have been flushed.
 */
static uint16_t fLinkSerializer_FlushDataBuffer(uint8_t **ptrToBuffer) {
  
  uint16_t allowableSize = 0;
  
  fLinkBuffer_FlushByPointer_(&(Faraabin.DataCircularBuffer), ptrToBuffer, &allowableSize);
  
  return allowableSize;
}

/**
 * @brief Flushes main queue of faraabin.
 * 
 * @param ptrToBuffer Pointer to the pointer of faraabin TX buffer.
 * @return num Number of bytes that have been flushed.
 */
static uint16_t fLinkSerializer_FlushCmdBuffer(uint8_t **ptrToBuffer) {
  
  uint16_t allowableSize = 0;
  
  fLinkBuffer_FlushByPointer_(&(Faraabin.CmdCircularBuffer), ptrToBuffer, &allowableSize);
  
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
  uint8_t frameType,
  uint8_t *fobjectSeq,
  uint8_t reqSeq,
  bool isEnd,
  uint32_t fobjectPtr,
  uint32_t extendedFobjectPtr,
  uint8_t clientFrameGroup,
  uint8_t clientFrameId,
  void(*generatePayloadFunc)(uint32_t fobjectPtr, void *param), void *payloadParam,
  bool isCmdBuffer) {
    
  if((frameType == FRAME_TYPE_EVENT) && (!fFaraabin_IsAllowEvent())) {
    return; /* MISRA C:2012 Rule 15.5 deviation */
  }
  
  FARAABIN_CRITICAL_ENTER_;

  Faraabin.ActiveBuffer = isCmdBuffer ? &(Faraabin.CmdCircularBuffer) : &(Faraabin.DataCircularBuffer);
    
  //Start frame
  Faraabin.Serializer.CheckSum = 0U;

  uint8_t seq = 0U;
  if(fobjectPtr != 0U) {
    (*fobjectSeq)++;
    if((*fobjectSeq) > 15U) {
      *fobjectSeq = 0U;
    }
    seq = *fobjectSeq;
  }
  
  Faraabin.Serializer.NodeSeq++;
  if(Faraabin.Serializer.NodeSeq > 15U) {
    Faraabin.Serializer.NodeSeq = 0U;
  }

  uint16_t control = 0;
  control |= (((uint16_t)frameType & 0x01U) << 0U);
  control |= (((uint16_t)isEnd & 0x01U) << 1U);
  control |= (((uint16_t)reqSeq & 0x0FU) << 2U);
  control |= (((uint16_t)seq & 0x0FU) << 6U);
  control |= (((uint16_t)Faraabin.Serializer.NodeSeq & 0x0FU) << 10U);
  uint8_t extPtr = (extendedFobjectPtr != 0U) ? 1U : 0U;
  control |= (((uint16_t)extPtr & 0x01U) << 14U);
  
  fAddToBufferU16(control);
  
  fAddToBufferU32(fChrono_GetTick());
  fAddToBufferU32(fobjectPtr);
  if(extPtr == 1U) {
    fAddToBufferU32(extendedFobjectPtr);
  }

  uint8_t fobjectProp = (uint8_t)(clientFrameGroup << 5U) + (uint8_t)(clientFrameId);
  ADD_U8_(fobjectProp);
  
  if(generatePayloadFunc != NULL) {
    generatePayloadFunc(fobjectPtr, payloadParam);
  }

  //Terminate frame
  Faraabin.Serializer.CheckSum ^= (uint8_t)0xFFU;   /* invert the bits in the checksum */
  fLinkBuffer_Put_(Faraabin.Serializer.CheckSum);
  
  for(int i = 0; i < sizeof(EOF_PATTERN); i++) {
    fLinkBuffer_Put_(EOF_PATTERN[i]);
  }
  
  FARAABIN_CRITICAL_EXIT_;
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
static void fLinkSerializer_CodeBlockSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, bool isCmdBuffer) {

  fSerializeFrame(
    (isResponse) ? FRAME_TYPE_RESPONSE : FRAME_TYPE_EVENT,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_MONITORING,
    (uint8_t)FB_CB_PROP_ID_MONITORING_VALUE,
    fGeneratePayload_CodeBlockValue, NULL,
    isCmdBuffer);
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
static void fLinkSerializer_CodeBlockSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isCmdBuffer) {
  
  fSerializeFrame(
    FRAME_TYPE_RESPONSE,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_SETTING,
    (uint8_t)FB_CB_PROP_ID_SETTING_ALL,
    fGeneratePayload_CodeBlockSetting, NULL,
    isCmdBuffer);
}
#endif

/**
 * @brief This is a helper function from fSerializeFrame() to send common user enable status via faraabin link.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for checking if this frame is response or not.
 */
static void fLinkSerializer_CommonSendEnable(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, bool isCmdBuffer) {

  fSerializeFrame(
    (isResponse) ? FRAME_TYPE_RESPONSE : FRAME_TYPE_EVENT,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_SETTING,
    (uint8_t)FB_COMMON_PROP_ID_SETTING_ENABLE,
    fGeneratePayload_CommonEnableStatus, NULL,
    isCmdBuffer);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send databus settings via faraabin link.
 * 
 * @param fobjectPtr Pointer to the databus fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 */
static void fLinkSerializer_DataBusSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isCmdBuffer) {

  fSerializeFrame(
    FRAME_TYPE_RESPONSE,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_SETTING,
    (uint8_t)FB_DB_PROP_ID_SETTING_ALL,
    fGeneratePayload_DataBusSetting, NULL,
    isCmdBuffer);
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
static void fLinkSerializer_DataBusSendCaptureValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isEnd, void* captureValue) {

  fSerializeFrame(
    FRAME_TYPE_RESPONSE,
    (fobjectSeq),
    (reqSeq),
    (isEnd),
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_MONITORING,
    (uint8_t)FB_DB_PROP_ID_MONITORING_CAPTURE_VALUE,
    fGeneratePayload_DataBusCaptureValue, captureValue,
    false);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send databus stream values via faraabin link.
 * 
 * @param fobjectPtr Pointer to the databus fobject.
 * @param fobjectSeq Sequence counter of the fobject.
 * @param reqSeq Request sequence counter of the frame.
 * @param isResponse Flag for indicating that this frame is a response.
 */
static void fLinkSerializer_DataBusSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse) {

  fSerializeFrame(
    (isResponse) ? FRAME_TYPE_RESPONSE : FRAME_TYPE_EVENT,
    (fobjectSeq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_MONITORING,
    (uint8_t)FB_DB_PROP_ID_MONITORING_STREAM_VALUE,
    fGeneratePayload_DataBusValue, NULL,
    false);
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
static void fLinkSerializer_VarSendValue(uint32_t fobjectPtr, uint32_t dataPtr, uint32_t size, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, bool isCmdBuffer) {
  
  sVarSendParam param;
  param.VarPtr = fobjectPtr;
  param.VarSize = size;
  param.DataPtr = dataPtr;

  fSerializeFrame(
    (isResponse) ? FRAME_TYPE_RESPONSE : FRAME_TYPE_EVENT,
    (fobjectSeq),
    (reqSeq),
    (true),
    (uint32_t)&(Faraabin.Mcu),  /* MISRA C:2012 Rule 11.4 deviation */
    0,
    (uint8_t)FB_PROP_GROUP_MONITORING,
    (uint8_t)FB_MCU_PROP_ID_MONITORING_VARIABLE,
    fGeneratePayload_VarValue, &param,
    isCmdBuffer);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send MCU ping results via faraabin link.
 * 
 * @param fobjectPtr Pointer to the MCU fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 * @param param Pointer to the parameters of the ping to be sent to application.
 */
static void fLinkSerializer_McuSendPing(uint32_t fobjectPtr, uint8_t reqSeq, void* param, bool isCmdBuffer) {

  fSerializeFrame(
    FRAME_TYPE_RESPONSE,
    &(Faraabin.Mcu.Seq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_MONITORING,
    (uint8_t)FB_MCU_PROP_ID_MONITORING_PING,
    fGeneratePayload_McuPing, param,
    isCmdBuffer);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send MCU live frame via faraabin link.
 * 
 * @param fobjectPtr Pointer to the MCU fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 */
static void fLinkSerializer_McuSendLive(uint32_t fobjectPtr, uint8_t reqSeq, bool isEnd, bool isCmdBuffer) {

  fSerializeFrame(
    FRAME_TYPE_RESPONSE,
    &(Faraabin.Mcu.Seq),
    (reqSeq),
    isEnd,
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_MONITORING,
    (uint8_t)FB_MCU_PROP_ID_MONITORING_LIVE,
    fGeneratePayload_McuLive, NULL,
    isCmdBuffer);
}

/**
 * @brief This is a helper function from fSerializeFrame() to send MCU WhoAmI frame via faraabin link.
 * 
 * @param fobjectPtr Pointer to the MCU fobject.
 * @param fobjectSeq Fobject sequence counter.
 * @param reqSeq Request sequence counter.
 */
static void fLinkSerializer_McuSendWhoAmI(uint32_t fobjectPtr, uint8_t reqSeq, bool isCmdBuffer) {

  fSerializeFrame(
    FRAME_TYPE_RESPONSE,
    &(Faraabin.Mcu.Seq),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_MONITORING,
    (uint8_t)FB_MCU_PROP_ID_MONITORING_WHOAMI,
    fGeneratePayload_McuWhoAmI, NULL,
    isCmdBuffer);
}

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
void fFaraabinLink_Serialize_McuSendProfiler(uint32_t fobjectPtr) {
  
  fSerializeFrame(
    FRAME_TYPE_EVENT,
    &(Faraabin.Mcu.Seq),
    (0),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_MONITORING,
    (uint8_t)FB_MCU_PROP_ID_MONITORING_PROFILER,
    fGeneratePayload_McuProfiler, NULL,
    false);
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
static void fLinkSerializer_StateMachineSendValue(uint32_t fobjectPtr, uint8_t *fobjectSeq, uint8_t reqSeq, bool isResponse, bool isCmdBuffer) {

  fSerializeFrame(
      (isResponse) ? FRAME_TYPE_RESPONSE : FRAME_TYPE_EVENT,
      (fobjectSeq), 
      (reqSeq),
      (true),
      (fobjectPtr),
      0,
      (uint8_t)FB_PROP_GROUP_MONITORING,
      (uint8_t)FB_SM_PROP_ID_MONITORING_VALUE,
      fGeneratePayload_StateMachineValue, NULL,
      isCmdBuffer);
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
static void fLinkSerializer_StateMachineSendSetting(uint32_t fobjectPtr, uint8_t *fobjectSeqPtr, uint8_t reqSeq, bool isCmdBuffer) {

  fSerializeFrame(
    FRAME_TYPE_RESPONSE,
    (fobjectSeqPtr),
    (reqSeq),
    (true),
    (fobjectPtr),
    0,
    (uint8_t)FB_PROP_GROUP_SETTING,
    (uint8_t)FB_SM_PROP_ID_SETTING_ALL,
    fGeneratePayload_StateMachineSetting, NULL,
    isCmdBuffer);
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
void fFaraabinLink_Serialize_VarTypeStructMemberDict(
  uint32_t structPtr,
  uint32_t varTypePtr,
  uint32_t varPtr,
  uint32_t varArrayQty,
  bool isPtr,
  char *name,
  uint8_t reqSeq) {
    
  sFaraabinFobjectVarType *me = (sFaraabinFobjectVarType*)structPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  sDictStructMemberPayload payLoad;
  
  payLoad.VarPtr = varPtr;
  payLoad.VarTypePtr = varTypePtr;
  payLoad.VarArrayQty = varArrayQty;
  payLoad._isPtr = isPtr;
  payLoad.Name = name;
  
  fSerializeDict(structPtr, &me->Seq, reqSeq, fGeneratePayload_VarTypeStructMemberDict, &payLoad);
  
}

/**
 * @brief This is a helper function from fSerializeFrame() to send user defined enumeration type members via faraabin link.
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
  uint8_t reqSeq) {
    
  sFaraabinFobjectVarType *me = (sFaraabinFobjectVarType*)enumPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  sDictEnumMemberPayload payLoad;
  
  payLoad.Name = enumMemberName;
  payLoad.Id = enumMemberId;
  
  fSerializeDict(enumPtr, &me->Seq, reqSeq, fGeneratePayload_VarTypeEnumMemberDict, &payLoad);
  
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
  uint8_t reqSeq) {
    
  sDictVarPayloadParam payLoad;
  
  payLoad.VarPtr = varPtr;
  payLoad.VarTypePtr = varTypePtr;
  payLoad.VarArrayQty = varArrayQty;
  payLoad.ExternalInterface = externalInterface;
  payLoad.AccessCallBack = accessCallback;  
  payLoad._isPtr = isPtr;
  payLoad.Name = name;
  payLoad.Path = path;
  payLoad.FileName = fileName;

  CORRECT_PATH_(payLoad.Path);
  
  fSerializeDict(varPtr, &(Faraabin.Mcu.Seq), reqSeq, fGeneratePayload_VarDict, &payLoad);
  
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
void fFaraabinLink_Serialize_FnGrpDict(
  char *path,
  char *fileName,
  char *objectName,
  uint32_t objectPtr,
  uint32_t groupTypePtr,
  uint8_t reqSeq) {
    
  sDictFunctionGroupPayloadParam payLoad;
  
  payLoad.Name = objectName;
  payLoad.Path = path;
  payLoad.FileName = fileName;
  payLoad.ObjectPtr = objectPtr;
  payLoad.GroupTypePtr = groupTypePtr;

  CORRECT_PATH_(payLoad.Path);
  
  fSerializeDict((uint32_t)&(Faraabin.Mcu), &(Faraabin.Mcu.Seq), reqSeq, fGeneratePayload_FunctionGroupDict, &payLoad); /* MISRA C:2012 Rule 11.4 deviation */
  
}

/**
 * @brief This is a helper function from fSerializeFrame() to send function group member dictionary via faraabin link.
 * 
 * @param groupTypePtr Pointer to the function group type.
 * @param fn Pointer to the function fobject.
 * @param reqSeq Request sequence counter.
 */
void fFaraabinLink_Serialize_FnGrpTypeMemberDict(uint32_t groupTypePtr, void *fn, uint8_t reqSeq) {
  
  sFaraabinFobjectFunctionGroupType *me = (sFaraabinFobjectFunctionGroupType*)groupTypePtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  fSerializeDict(groupTypePtr, &me->Seq, reqSeq, fGeneratePayload_FunctionGroupTypeMemberDict, fn);
  
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
//static void fAddToBufferU8(uint8_t d) {
//  
//  Faraabin.Serializer.CheckSum += d;
//  fLinkBuffer_Put_(d);
//}

/**
 * @brief Adds an unsigned 16-bit data to faraabin TX buffer.
 * 
 * @param d Value of data.
 */
static void fAddToBufferU16(uint16_t d) {
  
  uByte2 tmp;
  tmp.U16 = d;

  ADD_U8_(tmp.Byte[0]);
  ADD_U8_(tmp.Byte[1]);
}

/**
 * @brief Adds an unsigned 32-bit data to faraabin TX buffer.
 * 
 * @param d Value of data.
 */
static void fAddToBufferU32(uint32_t d) {
  
  uByte4 tmp;
  tmp.U32 = d;
  
  ADD_U8_(tmp.Byte[0]);
  ADD_U8_(tmp.Byte[1]);
  ADD_U8_(tmp.Byte[2]);
  ADD_U8_(tmp.Byte[3]);
}

/**
 * @brief Adds an unsigned 64-bit data to faraabin TX buffer.
 * 
 * @param d Value of data.
 */
static void fAddToBufferU64(uint64_t d) {
  
  uByte8 tmp;
  tmp.U64 = d;
  
  ADD_U8_(tmp.Byte[0]);
  ADD_U8_(tmp.Byte[1]);
  ADD_U8_(tmp.Byte[2]);
  ADD_U8_(tmp.Byte[3]);
  ADD_U8_(tmp.Byte[4]);
  ADD_U8_(tmp.Byte[5]);
  ADD_U8_(tmp.Byte[6]);
  ADD_U8_(tmp.Byte[7]);
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
 
 ADD_U8_(tmp.Byte[0]);
 ADD_U8_(tmp.Byte[1]);
 ADD_U8_(tmp.Byte[2]);
 ADD_U8_(tmp.Byte[3]);
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
 
 ADD_U8_(tmp.Byte[0]);
 ADD_U8_(tmp.Byte[1]);
 ADD_U8_(tmp.Byte[2]);
 ADD_U8_(tmp.Byte[3]);
 ADD_U8_(tmp.Byte[4]);
 ADD_U8_(tmp.Byte[5]);
 ADD_U8_(tmp.Byte[6]);
 ADD_U8_(tmp.Byte[7]);
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
    ADD_U8_(data[i]);
  }
}

/**
 * @brief Generates payload for reporting enable status of common fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_CommonEnableStatus(uint32_t fobjectPtr, void *param) {

  UNUSED_(param);
  uint8_t *fobjectType = (uint8_t*)fobjectPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  
  switch(*fobjectType) {
    case FO_TYPE_STATE_MACHINE: {
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
      ADD_U8_(((sStateMachine*)fobjectPtr)->_enable);  /* MISRA C:2012 Rule 11.4 deviation */
#else
      ADD_U8_(0);
#endif
      break;
    }

    case FO_TYPE_STATE_MACHINE_STATE: {
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
      ADD_U8_(((sState*)fobjectPtr)->_enable); /* MISRA C:2012 Rule 11.4 deviation */
#else
      ADD_U8_(0);
#endif
      break;
    }

    case FO_TYPE_STATE_MACHINE_TRANSITION: {
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
      ADD_U8_(((sTransition*)fobjectPtr)->_enable);  /* MISRA C:2012 Rule 11.4 deviation */
#else
      ADD_U8_(0);
#endif
      break;
    }

    case FO_TYPE_MCU: {
      ADD_U8_(Faraabin.Mcu.Enable);
      break;
    }

    case FO_TYPE_EVENT_GROUP: {
      ADD_U8_(((sFaraabinFobjectEventGroup*)fobjectPtr)->Enable);  /* MISRA C:2012 Rule 11.4 deviation */
      break;
    }

    case FO_TYPE_DATABUS: {
      ADD_U8_(((sFaraabinFobjectDataBus*)fobjectPtr)->Enable); /* MISRA C:2012 Rule 11.4 deviation */
      break;
    }

    case FO_TYPE_CODE_BLOCK: {
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
      ADD_U8_(((sCpuProcess*)fobjectPtr)->_enable);  /* MISRA C:2012 Rule 11.4 deviation */
#else
      ADD_U8_(0);
#endif
      break;
    }

    case FO_TYPE_VAR_TYPE_ENUM_MEMBER:
    case FO_TYPE_VAR_TYPE_STRUCT_MEMBER:
    case FO_TYPE_ENTITY_NUMERICAL:
    case FO_TYPE_ENTITY_EVENT:
    case FO_TYPE_VAR:
    case FO_TYPE_VAR_TYPE:
    case FO_TYPE_CONTAINER:
    default: {
      // Do nothing.
      break;
    }
  }
}

/**
 * @brief Generates payload for codeblock fobjects .
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
static void fGeneratePayload_CodeBlockDict(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sCpuProcess *me = (sCpuProcess*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    ADD_NAME_FIELDS_();
    return;
  }
  
  // Setting
  fGeneratePayload_CodeBlockSetting(fobjectPtr, NULL);
  
  // Status
  
  // Dict
  fAddToBufferU32(fCpuProfiler_ProcessGetRamUsage(me));
  ADD_U8_(me->MaxSectionQty);
  ADD_U8_(me->SectionQty);
  ADD_U8_(me->_isCodeblock);
  ADD_U8_(me->Priority);
  
  fAddToBufferU32((uint32_t)&(me->CpuUsagePercent));  /* MISRA C:2012 Rule 10.8 deviation */  /* MISRA C:2012 Rule 11.4 deviation */
  
  fAddToBufferU32((uint32_t)&(me->RunCount));         /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)&(me->IntervalUs));       /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)&(me->IntervalMaximaUs)); /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)&(me->IntervalMinimaUs)); /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)&(me->IntervalErrorCnt)); /* MISRA C:2012 Rule 11.4 deviation */
  
  fAddToBufferU32((uint32_t)&(me->ExecuteTimeUs));    /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)&(me->ExecuteTimeNetUs)); /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)&(me->ExecuteMaximaUs));  /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)&(me->ExecuteMinimaUs));  /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)&(me->ExecuteErrorCnt));  /* MISRA C:2012 Rule 11.4 deviation */
  
  for(uint16_t i = 0; i < me->SectionQty; i++) {
    fAddToBufferU32((uint32_t)&(me->_pBufferSections[i]->ExecuteTimeUs));     /* MISRA C:2012 Rule 11.4 deviation */
    fAddToBufferU32((uint32_t)&(me->_pBufferSections[i]->ExecuteTimeNetUs));  /* MISRA C:2012 Rule 11.4 deviation */
    ADD_U8_(me->_pBufferSections[i]->InternalId);
		
		uint16_t size = strlen(me->_pBufferSections[i]->Name);
		fAddToBufferU16(size);
    if(size > 0) {
      fAddToBuffer((uint8_t*)me->_pBufferSections[i]->Name, size);
    }
  }
  
  ADD_NAME_FIELDS_();  
}
#endif

/**
 * @brief Generates payload for sending values of codeblock fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
static void fGeneratePayload_CodeBlockValue(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  
  sCpuProcess *me = (sCpuProcess*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
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
  
  ADD_U8_(me->SectionQty);
  for(uint16_t i = 0; i < me->SectionQty; i++) {
    
    if(me->_pBufferSectionsCall[i] != NULL) {
      
      ADD_U8_(me->_pBufferSectionsCall[i]->InternalId);
      fAddToBufferU32(me->_pBufferSectionsCall[i]->ExecuteTimeUs);
      fAddToBufferU32(me->_pBufferSectionsCall[i]->ExecuteTimeNetUs);
      
    } else {
      
      ADD_U8_(255U);
      fAddToBufferU32(0U);
      
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
static void fGeneratePayload_CodeBlockSetting(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sCpuProcess *me = (sCpuProcess*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_U8_(me->_enable);
    
  fAddToBufferU32(me->IntervalThresholdMaxUs);
  fAddToBufferU32(me->IntervalThresholdMinUs);
  fAddToBufferU32(me->ExecuteThresholdMaxUs);
  fAddToBufferU32(me->ExecuteThresholdMinUs);
  ADD_U8_(me->IsSendingEventsToDbStreamEnabled);
  ADD_U8_(me->ISSendingEventsToDbCaptureEnabled);
  ADD_U8_(me->IsSendingDataToDbStreamEnabled);
}
#endif

/**
 * @brief Generates payload for dictionary of databus fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_DataBusDict(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectDataBus *me = (sFaraabinFobjectDataBus*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    ADD_NAME_FIELDS_();
    return;
  }
  
  // Status
  
  // Setting
  fGeneratePayload_DataBusSetting(fobjectPtr, NULL);
  
  // Dict
  fAddToBufferU32(
    sizeof(sFaraabinFobjectDataBus) + 
    (sizeof(sFaraabinFobjectDataBus_Channel) * me->ChannelQty) +
    (sizeof(sFaraabinFobjectDataBus_CaptureValue) * me->BufferCaptureSize)
  );
  
  fAddToBufferU32((uint32_t)&(me->CycleUs));  /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32(me->BufferCaptureSize);
  fAddToBufferU16(me->ChannelQty);
  ADD_NAME_FIELDS_();
}

/**
 * @brief Generates payload for settings of databus fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_DataBusSetting(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectDataBus *me = (sFaraabinFobjectDataBus*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_U8_(me->Enable);
  
  fAddToBufferU16(me->AttachedItemsQty);
  fAddToBufferU16(me->StreamDivideBy);
  fAddToBufferU32(me->BufferCaptureSize);
  fAddToBufferU16(me->TimerDivideBy);
  fAddToBufferU32(me->TimerWindowMs);
  fAddToBufferU16(me->TrigDivideBy);
  fAddToBufferU32(me->TimeAfterTrigMs);
  ADD_U8_(me->ApiTrigEnable);
  ADD_U8_(me->CurrentState);
  fAddToBufferU32(me->CycleUs);
  
  ADD_U8_(me->ChTrigEnable);
  fAddToBufferU16(me->ChTrigNo);
  ADD_U8_(me->ChTrigType);
  for(uint8_t i = 0; i < 8; i++) {
    ADD_U8_(me->ChTrigThreshold.Byte[i]);
  }
  
  fAddToBufferU16(fFaraabinFobjectDataBus_GetAttachCount(me));
    
  if(me->_init) {
    
    for(uint16_t i = 0; i<me->ChannelQty; i++) {
      
      if(me->pBufferChannels[i].ItemFobjectPtr != 0U) {
        
        fAddToBufferU16(i);
        ADD_U8_(me->pBufferChannels[i].ItemFobjectType);
        fAddToBufferU32(me->pBufferChannels[i].ItemFobjectPtr);
        fAddToBufferU16(me->pBufferChannels[i].ItemFobjectParam);
        ADD_U8_((uint8_t)me->pBufferChannels[i].Enable);
        
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
static void fGeneratePayload_DataBusCaptureValue(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);
  sFaraabinFobjectDataBus_CaptureValue *value = param;  /* MISRA C:2012 Rule 11.5 deviation */
  
  ADD_U8_(value->FobjectType);
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
static void fGeneratePayload_DataBusValue(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectDataBus *me = (sFaraabinFobjectDataBus*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  for(uint16_t i = 0; i < me->ChannelQty; i++) {

    if(me->pBufferChannels[i].ItemFobjectPtr == 0U) {
      continue;
    }

    if(!me->pBufferChannels[i].Enable) {
      continue;
    }
      
    switch(me->pBufferChannels[i].ItemFobjectType) {
      
      case FO_TYPE_VAR:
      case FO_TYPE_ENTITY_NUMERICAL: {
        
        ADD_U8_(me->pBufferChannels[i].ItemFobjectType);
        fAddToBufferU32(me->pBufferChannels[i].ItemFobjectPtr);
        
        fAddToBufferU16(me->pBufferChannels[i].ItemFobjectParam);
        fAddToBuffer((uint8_t*)me->pBufferChannels[i].ItemFobjectPtr, me->pBufferChannels[i].ItemFobjectParam); /* MISRA C:2012 Rule 11.4 deviation */
    
        break;
      }
      
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
      case FO_TYPE_CODE_BLOCK: {

        sCpuProcess *cb = (sCpuProcess*)me->pBufferChannels[i].ItemFobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */

        if(cb->IsSendingDataToDbStreamEnabled && cb->_init) {

          ADD_U8_(me->pBufferChannels[i].ItemFobjectType);
          fAddToBufferU32(me->pBufferChannels[i].ItemFobjectPtr);
          
          ADD_U8_(cb->SectionQty);
          
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
              
              ADD_U8_(cb->_pBufferSectionsCall[j]->InternalId);
              fAddToBufferU32(cb->_pBufferSectionsCall[j]->ExecuteTimeUs);
              fAddToBufferU32(cb->_pBufferSectionsCall[j]->ExecuteTimeNetUs);
              
            } else {
              
              ADD_U8_(255);
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
static void fGeneratePayload_EventGroupDict(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectEventGroup *me = (sFaraabinFobjectEventGroup*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    ADD_NAME_FIELDS_();
    return;
  }
  
  // Setting
  ADD_U8_(me->Enable);
  
  // Status
  
  // Dict
  fAddToBufferU32(sizeof(sFaraabinFobjectEventGroup));
  ADD_NAME_FIELDS_();
}

/**
 * @brief Generates payload for sending dictionary of fobject container.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_ContainerDict(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFaraabinFobjectContainer *me = (sFaraabinFobjectContainer*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    ADD_NAME_FIELDS_();
    return;
  }
  
  // Setting
  
  // Status
  
  // Dict
  fAddToBufferU32(sizeof(sFaraabinFobjectContainer));
  ADD_NAME_FIELDS_();
}

/**
 * @brief Generates payload for sending dictionary of MCU fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_McuDict(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);
  sFobjectMcu *me = (sFobjectMcu*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    ADD_NAME_FIELDS_();
    return;
  }
  
  // Setting
  ADD_U8_(me->Enable);
  
  // Status
  uint16_t runningFunctions = fFunctionEngine_GetQtyOfRunningFunctions();
  fAddToBufferU16(runningFunctions);
  for(uint16_t i = 0; i < runningFunctions; i++) {
    
    uint32_t ptr = 0U;
    uint32_t objectPtr = 0U;
    if(fFunctionEngine_GetPtrsOfRunningFunction(i, &ptr, &objectPtr) != 0U) {
      // |Not going to happen.
    }
    
    fAddToBufferU32(ptr);
    fAddToBufferU32(objectPtr);
  }
  
  // Dict
  fAddToBufferU32(sizeof(sFaraabin));
  fAddToBufferU32(Faraabin.DataCircularBuffer.Size);
}

/**
 * @brief Generates payload for sending ping results of MCU fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_McuPing(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);

  uint8_t *param_ptr = (uint8_t*)param; /* MISRA C:2012 Rule 11.5 deviation */
  ADD_U8_(param_ptr[0]);

}

/**
 * @brief Generates payload for sending MCU Live frame.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_McuLive(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);
  UNUSED_(param);

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  sCpuProfiler_StatusFlag status = fCpuProfiler_GetStatusFlag();
  Faraabin.Flags.Status.Bitfield.ProfilerListOvf = status.ProcessListOvf;
  Faraabin.Flags.Status.Bitfield.ProfilerDepthOvf = status.DepthOvf;
  Faraabin.Flags.Status.Bitfield.ProfilerDuplicate = status.DuplicateProcess;

  Faraabin.Flags.Status.Bitfield.DataBufferOverflow = Faraabin.DataCircularBuffer._isOvf;
  Faraabin.Flags.Status.Bitfield.CmdBufferOverflow = Faraabin.CmdCircularBuffer._isOvf;
#endif
  
  fAddToBufferU32(Faraabin.Flags.Status.U32);
  fAddToBufferU16(Faraabin.Mcu.SessionId);
  
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  fAddToBufferU16((uint16_t)(fCpuProfiler_GetCpuUsage() * 100.0f)); /* MISRA C:2012 Rule 10.8 deviation */
#else
  fAddToBufferU16(0); //Cpu usage
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
static void fGeneratePayload_McuProfiler(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);
  UNUSED_(param);

  fAddToBufferU16((uint16_t)(fCpuProfiler_GetCpuUsage() * 100.0f)); /* MISRA C:2012 Rule 10.8 deviation */
  
  sCpuProcess** processes = fCpuProfiler_GetProcessList();
  
  fAddToBufferU16(CPU_PROFILER_MAX_PROCESS);
  for(uint16_t i=0; i < CPU_PROFILER_MAX_PROCESS; i++) {
    
    if(processes[i] != NULL) {
      
      fAddToBufferU32((uint32_t)(processes[i]));  /* MISRA C:2012 Rule 11.4 deviation */
      fAddToBufferU16((uint16_t)(processes[i]->CpuUsagePercent * 100.0f));  /* MISRA C:2012 Rule 10.8 deviation */
      
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
static void fGeneratePayload_McuWhoAmI(uint32_t fobjectPtr, void *param) {

  UNUSED_(param);
  UNUSED_(fobjectPtr);
  
  bool isBigEndian = false;
  
  uByte2 tmp;
  
  tmp.U16 = 0x0001U;
  if(tmp.Byte[0] == 0x00U) {
    isBigEndian =  true;
  } else {
    isBigEndian =  false;
  }
  
  ADD_U8_(isBigEndian ? (uint8_t)1 : (uint8_t)0);
  ADD_U8_(VERSION_MAJOR);
  ADD_U8_(VERSION_MINOR);
  fAddToBufferU32(Faraabin.Flags.Features.U32);
  fAddToBufferU32(Faraabin.Database.DatabaseArrayIndex);
  fAddToBufferU32(fCountAllDictsFrames());
  fAddToBufferU32(fFaraabin_GetRxBufferSize());
  fAddToBufferU32(fChrono_GetTickToNsCoef());
  fAddToBufferU32(fChrono_GetTickTopValue());
  fAddToBufferU32(Faraabin.Mcu.BootTimeMs);
  fAddToBufferU16(strlen(fFaraabin_GetFirmwareName()));
  fAddToBufferString(fFaraabin_GetFirmwareName());
  fAddToBufferU16(strlen(fFaraabin_GetFirmwareInfo()));
  fAddToBufferString(fFaraabin_GetFirmwareInfo());
  ADD_U8_('\0');
}

/**
 * @brief Generates payload for sending dictionary of state machine fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fGeneratePayload_StateMachineDict(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sStateMachine *me = (sStateMachine*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    ADD_NAME_FIELDS_();
    return;
  }
  
  // Setting
  fGeneratePayload_StateMachineSetting(fobjectPtr, NULL);
  
  // Status
  fAddToBufferU32((uint32_t)me->_currentState);           /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)me->_lastTransitionOccurred); /* MISRA C:2012 Rule 11.4 deviation */
  
  // Dict
  fAddToBufferU32(fStateMachine_GetRamUsage(me));
  
  ADD_U8_((uint8_t)me->_isHierarchical);
  
  fAddToBufferU16(me->_maxStateQty);
  fAddToBufferU16(me->_stateQty);
  fAddToBufferU32((uint32_t)me->_firstState); /* MISRA C:2012 Rule 11.4 deviation */
  
  fAddToBufferU16(me->_maxTransitionQty);
  fAddToBufferU16(me->_transitionQty);
  
  fAddToBufferU16(me->_maxDepth);
  ADD_NAME_FIELDS_();
}
#endif

/**
 * @brief Generates payload for sending dictionary of state machine's states.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fGeneratePayload_StateMachineDictState(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sState *me = (sState*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_U8_(FO_TYPE_STATE_MACHINE_STATE | 0x80U);
  
  // Setting
  ADD_U8_(me->_enable);
  ADD_U8_(me->AllTransitionEnable);
  
  // Status
  
  // Dict
  fAddToBufferU32(sizeof(sState)); 
  
  fAddToBufferU16(me->_stateId);
  ADD_U8_(me->_depth);
  ADD_U8_((uint8_t)me->_isFirstChild);
  fAddToBufferU32((uint32_t)me->_stateMachine); /* MISRA C:2012 Rule 11.6 deviation */
  fAddToBufferU32((uint32_t)me->_parent);       /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)me->_firstChild);   /* MISRA C:2012 Rule 11.4 deviation */
  
  uint16_t size = strlen(me->Name);
  fAddToBufferU16(size);
  if(size > 0) {
    fAddToBuffer((uint8_t*)me->Name, size);
  }
}
#endif

/**
 * @brief Generates payload for sending dictionary of state machine's transitions.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fGeneratePayload_StateMachineDictTransition(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sTransition *me = (sTransition*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  sState *srcState = (sState*)me->_srcState;  /* MISRA C:2012 Rule 11.5 deviation */
  
  ADD_U8_(FO_TYPE_STATE_MACHINE_TRANSITION | 0x80U);
  
  // Setting
  ADD_U8_(me->_enable);
  
  // Status
  
  // Dict
  fAddToBufferU32(sizeof(sTransition));
  
  fAddToBufferU16(me->_transitionId);
  fAddToBufferU32((uint32_t)srcState->_stateMachine); /* MISRA C:2012 Rule 11.6 deviation */
  fAddToBufferU32((uint32_t)me->_srcState);           /* MISRA C:2012 Rule 11.6 deviation */
  fAddToBufferU32((uint32_t)me->_dstState);           /* MISRA C:2012 Rule 11.6 deviation */
  
  uint16_t size = strlen(me->Name);
  fAddToBufferU16(size);
  if(size > 0) {
    fAddToBuffer((uint8_t*)me->Name, size);
  }
}
#endif

/**
 * @brief Generates payload for sending values of state machine fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fGeneratePayload_StateMachineValue(uint32_t fobjectPtr, void *param) {

  UNUSED_(param);
  
  sStateMachine *me = (sStateMachine*)fobjectPtr;         /* MISRA C:2012 Rule 11.4 deviation */
  
  fAddToBufferU32((uint32_t)me->_currentState);           /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBufferU32((uint32_t)me->_lastTransitionOccurred); /* MISRA C:2012 Rule 11.4 deviation */
}
#endif

/**
 * @brief Generates payload for sending settings of state machine fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
static void fGeneratePayload_StateMachineSetting(uint32_t fobjectPtr, void *param) {

  UNUSED_(param);
  
  sStateMachine *me = (sStateMachine*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_U8_(me->_enable);
  
  ADD_U8_(me->AllTransitionEnable);
  
  fAddToBufferU16(me->_stateQty);
  for(uint16_t i = 0; i < me->_stateQty; i++) {
    
    sState *state = (sState*)me->_pBufferListState[i];
    
    ADD_U8_(state->_enable);
    ADD_U8_(state->AllTransitionEnable);
  }
  
  fAddToBufferU16(me->_transitionQty);
  for(uint16_t i = 0; i < me->_transitionQty; i++) {
    
    sTransition *tran = (sTransition*)me->_pBufferListTransition[i];
    
    ADD_U8_(tran->_enable);
  }
}
#endif

/**
 * @brief Generates payload for sending values of variable fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_VarValue(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);

  sVarSendParam *par = (sVarSendParam*)param;
  
  fAddToBufferU32(par->VarPtr);                       /* MISRA C:2012 Rule 11.4 deviation */
  fAddToBuffer((uint8_t*)par->DataPtr, par->VarSize); /* MISRA C:2012 Rule 11.5 deviation */
}

/**
 * @brief Generates payload for sending dictionary of variable fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_VarDict(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);

  sDictVarPayloadParam *me = (sDictVarPayloadParam*)param; /* MISRA C:2012 Rule 11.5 deviation */
  
  ADD_U8_(FO_TYPE_VAR | 0x80U);
  
  fAddToBufferU32(me->VarPtr);
  fAddToBufferU32(me->VarTypePtr);
  fAddToBufferU32(me->VarArrayQty);
  
  uint8_t control = 0x00U;
  if(me->ExternalInterface != 0U) {
    control |= 0x01U;
  }
  if(me->AccessCallBack != 0U) {
    control |= 0x02U;
  }
  if(me->_isPtr) {
    control |= 0x04U;
  }
  ADD_U8_(control);
  
  if(me->ExternalInterface != 0U) {
    fAddToBufferU32(me->ExternalInterface);
  }
  if(me->AccessCallBack != 0U) {
    fAddToBufferU32(me->AccessCallBack);
  }
  
  ADD_NAME_FIELDS_WO_FILENAME_();
}

/**
 * @brief Generates payload for sending dictionary of varType fobjects.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_VarTypeDict(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sFaraabinFobjectVarType *me = (sFaraabinFobjectVarType*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    ADD_NAME_FIELDS_();
    return;
  }

  // Setting

  // Status

  // Dict
  fAddToBufferU32(sizeof(sFaraabinFobjectVarType));
  
  ADD_U8_((uint8_t)me->DataType);
  fAddToBufferU32(me->Size);
  ADD_NAME_FIELDS_();
}

/**
 * @brief Generates payload for sending dictionary of structure members.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_VarTypeStructMemberDict(uint32_t fobjectPtr, void *param) {

  UNUSED_(fobjectPtr);
  
  sDictStructMemberPayload *par = (sDictStructMemberPayload*)param; /* MISRA C:2012 Rule 11.5 deviation */
  
  ADD_U8_(FO_TYPE_VAR_TYPE_STRUCT_MEMBER | 0x80U);
  
  fAddToBufferU32(par->VarPtr);
  fAddToBufferU32(par->VarTypePtr);
  fAddToBufferU32(par->VarArrayQty);
  ADD_U8_((uint8_t)par->_isPtr);

  uint16_t nameSize = strlen(par->Name);
  fAddToBufferU16(nameSize);

  if(nameSize > 0) {
    fAddToBuffer((uint8_t*)par->Name, nameSize);
  }
}

/**
 * @brief Generates payload for sending dictionary of enum members.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_VarTypeEnumMemberDict(uint32_t fobjectPtr, void *param) {

  UNUSED_(fobjectPtr);
  
  sDictEnumMemberPayload *par = (sDictEnumMemberPayload*)param; /* MISRA C:2012 Rule 11.5 deviation */
  
  ADD_U8_(FO_TYPE_VAR_TYPE_ENUM_MEMBER | 0x80U);
  
  fAddToBufferU16(par->Id);
  
  uint16_t nameSize = strlen(par->Name);
  fAddToBufferU16(nameSize);

  if(nameSize > 0) {
    fAddToBuffer((uint8_t*)par->Name, nameSize);
  }
}

/**
 * @brief Generates payload for sending index and total number of faraabin dictionary.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_DictIterator(uint32_t fobjectPtr, void *param) {

  fAddToBufferU16(Faraabin.Serializer.DictIterator.CurrentDictIndex);
  fAddToBufferU16(Faraabin.Serializer.DictIterator.TotalSubDicts);
  fAddToBufferU16(Faraabin.Serializer.DictIterator.CurrentSubDictIndex);
  
  sGeneratePayloadDict_Param *par = (sGeneratePayloadDict_Param*)param; /* MISRA C:2012 Rule 11.5 deviation */
  if(par->fpGenerateDictPayload != NULL) {
    par->fpGenerateDictPayload(fobjectPtr, par->pDictPayloadParam);
  }
}

/**
 * @brief 
 * 
 * @param fobjectPtr 
 * @param param 
 */
static void fSerializePayload_Event(uint32_t fobjectPtr, void *param) {
  
  sEventParam *par = (sEventParam*)param; /* MISRA C:2012 Rule 11.5 deviation */
  
  ADD_U8_((uint8_t)par->Severity);
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
static void fDictIterator_Iterate(void) {

  if(Faraabin.Serializer.DictIterator._isInCountingMode) {
    Faraabin.Serializer.DictIterator.TotalSubDicts++;
  } else {
    Faraabin.Serializer.DictIterator.CurrentSubDictIndex++;
  }
}

/**
 * @brief Resets the internal counter of DictIterator to start over.
 * 
 * @param dictIndex Current dictionary index.
 */
static void fDictIterator_ResetCounter(uint16_t dictIndex) {

  Faraabin.Serializer.DictIterator.CurrentDictIndex = dictIndex;
  Faraabin.Serializer.DictIterator.CurrentSubDictIndex = 0U;
  Faraabin.Serializer.DictIterator.TotalSubDicts = 0U;
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

  if(!Faraabin.Serializer.DictIterator._isInCountingMode) {
    sGeneratePayloadDict_Param param;

    param.fpGenerateDictPayload = fpGenerateDictPayload;
    param.pDictPayloadParam = dictPayloadParam;

    fSerializeFrame(  
      FRAME_TYPE_RESPONSE,
      fobjectSeqPtr,
      reqSeq,
      false,
      fobjectPtr,
      0,
      (uint8_t)FB_PROP_GROUP_DICT,
      (uint8_t)FB_COMMON_PROP_ID_DICT,
      fGeneratePayload_DictIterator, &param,
      false);
      
    fFaraabin_FlushBuffer();
  }

  fDictIterator_Iterate();

}

/**
 * @brief Generates dictionary for the fobjects based on their type.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param reqSeq Request sequence counter.
 */
static void fGenerateDict(uint32_t fobjectPtr, uint8_t reqSeq) {

  uint8_t *fobjectType = (uint8_t*)fobjectPtr;  /* MISRA C:2012 Rule 11.4 deviation */
  
  switch(*fobjectType) {
    
    case FO_TYPE_MCU: {
      
      fSerializeDict(fobjectPtr, &(Faraabin.Mcu.Seq), reqSeq, fGeneratePayload_McuDict, NULL);
      
      break;
    }
    
    case FO_TYPE_VAR_TYPE: {
      sFaraabinFobjectVarType *me = (sFaraabinFobjectVarType*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */

      CORRECT_PATH_(me->Path);
  
      switch(me->DataType) {
        
        case VAR_DATA_TYPE_PRIMITIVE: {

          fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fGeneratePayload_VarTypeDict, NULL);
          
          break;
        }
        
        case VAR_DATA_TYPE_USER_DEFINED_ENUM:
        case VAR_DATA_TYPE_USER_DEFINED_STRUCT:
        case VAR_DATA_TYPE_USER_DEFINED_UNION: {

          fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fGeneratePayload_VarTypeDict, NULL);
          
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
    
    case FO_TYPE_FUNCTION_GROUP_TYPE: {
      sFaraabinFobjectFunctionGroupType *me = (sFaraabinFobjectFunctionGroupType*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */

      fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fGeneratePayload_FunctionGroupTypeDict, NULL);
          
      if(me->fpSendMember != 0U) {
        me->fpSendMember(fobjectPtr, reqSeq);
      }
      
      break;
    }
    
    case FO_TYPE_EVENT_GROUP: {
      sFaraabinFobjectEventGroup *me = (sFaraabinFobjectEventGroup*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */

      CORRECT_PATH_(me->Path);

      fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fGeneratePayload_EventGroupDict, NULL);
      
      break;
    }
    
    case FO_TYPE_CONTAINER: {
      sFaraabinFobjectContainer *me = (sFaraabinFobjectContainer*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
      
      fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fGeneratePayload_ContainerDict, NULL);
      
      if(me->fpContainerFunc != NULL) {
        me->fpContainerFunc(reqSeq);
      }

      break;
    }
    
    case FO_TYPE_DATABUS: {
      sFaraabinFobjectDataBus *me = (sFaraabinFobjectDataBus*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */

      CORRECT_PATH_(me->Path);

      fSerializeDict(fobjectPtr, &me->Seq, reqSeq, fGeneratePayload_DataBusDict, NULL);
      
      break;
    }
    
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
    case FO_TYPE_CODE_BLOCK: {

      sCpuProcess *me = (sCpuProcess*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */

      CORRECT_PATH_(me->Path);

      fSerializeDict(fobjectPtr, &me->_seq, reqSeq, fGeneratePayload_CodeBlockDict, NULL);
      
      break;
    }
#endif
    
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
    case FO_TYPE_STATE_MACHINE: {

      sStateMachine *me = (sStateMachine*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */

      CORRECT_PATH_(me->Path);
  
      fSerializeDict(fobjectPtr, &me->_seq, reqSeq, fGeneratePayload_StateMachineDict, NULL);
      
      for(uint16_t i = 0; i < me->_stateQty; i++) {
        
        if(me->_pBufferListState[i] != NULL) {
          sState *state = (sState*)me->_pBufferListState[i];
          fSerializeDict((uint32_t)state, &me->_seq, reqSeq, fGeneratePayload_StateMachineDictState, NULL); /* MISRA C:2012 Rule 11.4 deviation */
        }
      }
      
      for(uint16_t i = 0; i < me->_transitionQty; i++) {
        
        sTransition *tran = (sTransition*)me->_pBufferListTransition[i];
        
        if(tran != NULL) {
          
          fSerializeDict((uint32_t)tran, &me->_seq, reqSeq, fGeneratePayload_StateMachineDictTransition, NULL); /* MISRA C:2012 Rule 11.4 deviation */

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
 * @return num Number of frames for all of the dictionaries in database.
 */
static uint32_t fCountAllDictsFrames(void) {
  
  fDictIterator_ResetCounter(0U);

  for(uint16_t i = 0; i < Faraabin.Database.DatabaseArrayIndex; i++) {
    Faraabin.Serializer.DictIterator._isInCountingMode = true;
    fGenerateDict(Faraabin.Database.DatabaseArray[i], 0U);
  }

  Faraabin.Serializer.DictIterator._isInCountingMode = false;

  return Faraabin.Serializer.DictIterator.TotalSubDicts; // It is counted for all dicts, hence all frame counts
}

/**
 * @brief Generates dictionary payload for function groups.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the parameters of the payload.
 */
static void fGeneratePayload_FunctionGroupTypeDict(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(param);

  sFaraabinFobjectFunctionGroupType *me = (sFaraabinFobjectFunctionGroupType*)fobjectPtr; /* MISRA C:2012 Rule 11.4 deviation */
  
  ADD_CONTROL_BYTE_();
  
  if(!me->_init) {
    
    uint16_t size;

    size = strlen(me->Name);
    fAddToBufferU16(size);
    if(size > 0) {
      fAddToBuffer((uint8_t*)me->Name, size);
    }

    size = strlen(me->FileName);
    fAddToBufferU16(size);
    if(size > 0) {
      fAddToBuffer((uint8_t*)me->FileName, size);
    }

    size = strlen(me->Help);
    fAddToBufferU16(size);
    if(size > 0) {
      fAddToBuffer((uint8_t*)me->Help, size);
    }

    return; /* MISRA C:2012 Rule 15.5 deviation */
  }

  // Setting

  // Status

  // Dict
  fAddToBufferU32(sizeof(sFaraabinFobjectFunctionGroupType));
  
  uint16_t size = strlen(me->Name);
  fAddToBufferU16(size);
  if(size > 0) {
    fAddToBuffer((uint8_t*)me->Name, size);
  }
  size = strlen(me->FileName);
  fAddToBufferU16(size);
  if(size > 0) {
    fAddToBuffer((uint8_t*)me->FileName, size);
  }
  size = strlen(me->Help);
  fAddToBufferU16(size);
  if(size > 0) {
    fAddToBuffer((uint8_t*)me->Help, size);
  }
}

/**
 * @brief Generates dictionary payload for function group members.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the payload parameters.
 */
static void fGeneratePayload_FunctionGroupTypeMemberDict(uint32_t fobjectPtr, void *param) {
  
  UNUSED_(fobjectPtr);
  
  sFaraabinFobjectFunction *par = (sFaraabinFobjectFunction*)param; /* MISRA C:2012 Rule 11.5 deviation */
  
  ADD_U8_(FO_TYPE_FUNCTION_GROUP_TYPE_MEMBER | 0x80U);
  fAddToBufferU32(sizeof(sFaraabinFobjectFunction));
  
  fAddToBufferU32((uint32_t)par);

  uint16_t size = strlen(par->Name);
  fAddToBufferU16(size);
  if(size > 0) {
    fAddToBuffer((uint8_t*)par->Name, size);
  }
  
  size = strlen(par->Help);
  fAddToBufferU16(size);
  if(size > 0) {
    fAddToBuffer((uint8_t*)par->Help, size);
  }
}

/**
 * @brief Generates dictionary payload for function group.
 * 
 * @param fobjectPtr Pointer to the fobject.
 * @param param Pointer to the payload parameters.
 */
static void fGeneratePayload_FunctionGroupDict(uint32_t fobjectPtr, void *param) {

  UNUSED_(fobjectPtr);
  
  sDictFunctionGroupPayloadParam *me = (sDictFunctionGroupPayloadParam*)param; /* MISRA C:2012 Rule 11.5 deviation */
  
  ADD_U8_(FO_TYPE_FUNCTION_GROUP | 0x80U);
  
  fAddToBufferU32(me->ObjectPtr);
  fAddToBufferU32(me->GroupTypePtr);
  ADD_NAME_FIELDS_();
}

#endif //FARAABIN_ENABLE

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
