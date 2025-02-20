/**
 ******************************************************************************
 * @file           : fb_link_handler.c
 * @brief          : Faraabin link handler module.
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
#include "faraabin_link_handler.h"

#include "faraabin.h"
#include "faraabin_port.h"

#include "faraabin_link_buffer.h"

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
#include "add_on/cpu_profiler/faraabin_addon_cpu_profiler.h"
#endif

#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
#include "add_on/state_machine/faraabin_addon_state_machine.h"
#endif

#include <stdlib.h>

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
 * @brief Faraabin link minimum size of frame.
 * 
 */
#define MINIMUM_FRAME_SIZE  7

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/**
 * @brief Priority level of the client frame.
 * 
 */
typedef enum {
  
  eFB_CLIENT_FRAME_PRIORITY_LOW = 0,
  eFB_CLIENT_FRAME_PRIORITY_HIGH,
  
}eClientFrame_Priority;

/**
 * @brief Access type of the client frame.
 * 
 */
typedef enum {
  
  eFB_CLIENT_FRAME_ACCESS_TYPE_READ = 0,
  eFB_CLIENT_FRAME_ACCESS_TYPE_WRITE = 1,
  
}eClientFrame_AccessType;

/* Private variables ---------------------------------------------------------*/
sLinkHandlerInternal LinkHandler;

/* Private function prototypes -----------------------------------------------*/
static void fFrameHandler(sClientFrame* clientFrame);

static void fMcuFrameHandler(sClientFrame* clientFrame);
static void fDataBusFrameHandler(sClientFrame* clientFrame);
static void fCodeBlockFrameHandler(sClientFrame* clientFrame);
static void fStateMachineFrameHandler(sClientFrame* clientFrame);
static void fStateMachineStateFrameHandler(sClientFrame* clientFrame);
static void fStateMachineTransitionFrameHandler(sClientFrame* clientFrame);
static void fFunctionFrameHandler(sClientFrame* clientFrame);
static void fEventGroupEventHandler(sClientFrame* clientFrame);

static void fSendCircularBuffer(bool flush);

/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
              ##### fb_link_handler.c Exported Functions #####
===============================================================================*/
/**
 * @brief LinkHandler.Initializes link handler of faraabin.
 * 
 * @note This function is called internally in faraabin.c file.
 * 
 * @param rxBuffer Pointer to the RX buffer that serializer needs to keep transmitting data.
 * @param rxBufferSize Size of the RX buffer.
 * @return result Could be any value from LINK_HANDLER_RESULT group.
 */
uint8_t fFaraabinLinkHandler_Init(uint8_t *rxBuffer, uint16_t rxBufferSize) {
  
  LinkHandler.Init = false;
  
  LinkHandler.Password = NULL;
  
  if(rxBufferSize < 20U) {
    return FB_LINK_HANDLER_RESULT_ERROR_ALLOCATED_RX_BUFFER_SIZE_IS_SMALL;
  }

  LinkHandler.RxCharBufferSize = rxBufferSize;

  if(rxBuffer != NULL) {
    LinkHandler.RxCharBuffer = rxBuffer;
  } else {

    LinkHandler.RxCharBuffer = malloc(rxBufferSize);
    if(LinkHandler.RxCharBuffer == NULL) {
      return FB_LINK_HANDLER_RESULT_ERROR_RX_BUFFER_SIZE_IS_NULL;
    }
  }

  for(uint16_t i = 0; i < sizeof(rxBufferSize); i++) {
    LinkHandler.RxCharBuffer[i] = 0x00U;
  }

  LinkHandler.IsNewFrameDetected = false;
  LinkHandler.IsFlushingBuffer = false;
  LinkHandler.DictSendingMode.ReqSeq = 0U;
  LinkHandler.DictSendingMode.SendFlag = false;

  LinkHandler.Init = true;
  return FB_LINK_HANDLER_RESULT_OK;
}

/**
 * @brief Runs the link handler priodically and handles frame if new one is detected.
 * 
 * @note This function is called in faraabin.c internally.
 * 
 */
void fFaraabinLinkHandler_Run(void) {

  if(!LinkHandler.Init) {
    return;
  }

  // Frame handling
  if(LinkHandler.IsNewFrameDetected) {

    fFrameHandler(&LinkHandler.ClientFrame);
    LinkHandler.IsNewFrameDetected = false;

  }

  fSendCircularBuffer(false);
  
  // Dict handling
  if(LinkHandler.DictSendingMode.SendFlag == true) {
    
    uint16_t dictQty = fFaraabinDatabase_GetNumberOfAddedDicts();
    
    if(LinkHandler.DictSendingMode.IsBlocking && FaraabinFlags.Features.Bitfield.AllowSendDickBlocking) {

      for(uint16_t i = 0; i < dictQty; i++) {

        fFaraabinLinkSerializer_SerializeDict(fFaraabinDatabase_GetFobjectPointerFromDict(i), i, LinkHandler.DictSendingMode.ReqSeq);
        fSendCircularBuffer(true);

      }
      LinkHandler.DictSendingMode.SendFlag = false;
      fFaraabinFobjectMcu_SendEventSystemResponse(eMCU_EVENT_INFO_DICT_END, LinkHandler.DictSendingMode.ReqSeq);
      FaraabinFlags.Status.Bitfield.NewDict = 0U;
      
    } else {
      
      static uint16_t dictIndex = 0;

       fFaraabinLinkSerializer_SerializeDict(fFaraabinDatabase_GetFobjectPointerFromDict(dictIndex), dictIndex, LinkHandler.DictSendingMode.ReqSeq);
       fSendCircularBuffer(true);
       dictIndex++;

       if(dictIndex >= dictQty) {

         dictIndex = 0U;
         LinkHandler.DictSendingMode.SendFlag = false;
         fFaraabinFobjectMcu_SendEventSystemResponse(eMCU_EVENT_INFO_DICT_END, LinkHandler.DictSendingMode.ReqSeq);
         FaraabinFlags.Status.Bitfield.NewDict = 0U;

       }
     }
  }
}

/**
 * @brief Get newly received byte from the link and interprets it, if it is a valid frame.
 * 
 * @param c Received character.
 */
void fFaraabinLinkHandler_CharReceived(uint8_t c) {
  
  static uint16_t RxCharBufferIndex = 0;
  
  sFaraabinFobjectMcu* mcuHandle = fFaraabinFobjectMcu_GetFobject();
  mcuHandle->StatisticsRxBytesCnt++;

  if(LinkHandler.IsNewFrameDetected) {
    
    mcuHandle->StatisticsRxFramesOverrideErrorCnt++;
    
    RxCharBufferIndex = 0U;
    fFaraabinFobjectMcu_SendEventSystemException(eMCU_EVENT_ERROR_RX_FRAME_BEFORE_END_OF_PREVIOUS_FRAME);
    
  }

  if(c != FB_EOF) {
    
    LinkHandler.RxCharBuffer[RxCharBufferIndex++] = c;
    
    if(RxCharBufferIndex >= LinkHandler.RxCharBufferSize) {
      
      RxCharBufferIndex = 0U;
      fFaraabinFobjectMcu_SendEventSystemException(eMCU_EVENT_ERROR_RX_FRAME_BIG_SIZE);
      
    }
  } else {
    
    mcuHandle->StatisticsRxFramesCnt++;
    
    uint8_t ret = fFaraaninLinkDeserializer_Deserialize(LinkHandler.RxCharBuffer, RxCharBufferIndex, &LinkHandler.ClientFrame);
    
    switch(ret) {

      case DESERIALIZE_OK: {
        
        uint8_t ClientFramePriority = ClientFrame_GetPriority_(LinkHandler.ClientFrame.Control);
        if(ClientFramePriority == (uint8_t)eFB_CLIENT_FRAME_PRIORITY_HIGH) {
          
          fFrameHandler(&LinkHandler.ClientFrame);
          
        } else {
          
          LinkHandler.IsNewFrameDetected = true;
        }
        break;
      }

      case DESERIALIZE_ERROR_CHECKSUM: {
        
        mcuHandle->StatisticsRxFramesChecksumErrorCnt++;
        
        LinkHandler.IsNewFrameDetected = false;
        fFaraabinFobjectMcu_SendEventSystemException(eMCU_EVENT_ERROR_RX_FRAME_CHECKSUM);
        
        break;
      }

      case DESERIALIZE_ERROR_DEESCAPE: {
        
        mcuHandle->StatisticsRxFramesEscapingErrorCnt++;
        
        LinkHandler.IsNewFrameDetected = false;
        fFaraabinFobjectMcu_SendEventSystemException(eMCU_EVENT_ERROR_RX_FRAME_DESCAPE);
        
        break;
      }

      case DESERIALIZE_ERROR_MINIMUM_FRAME_SIZE: {
        
        mcuHandle->StatisticsRxFramesMinimumSizeErrorCnt++;
        
        LinkHandler.IsNewFrameDetected = false;
        fFaraabinFobjectMcu_SendEventSystemException(eMCU_EVENT_ERROR_RX_FRAME_SMALL_SIZE);
        
        break;
      }

      default: {

        fFaraabinFobjectMcu_SendEventSystemException(eMCU_EVENT_ERROR_UNDEF);
        break;
      }
    }
    
    RxCharBufferIndex = 0U;
  }
}

/**
 * @brief Flushes the link TX buffer
 * 
 */
void fFaraabinLinkHandler_FlushBuffer(void) {
  fSendCircularBuffer(true);
}

/**
 * @brief Sets the LinkHandler.Password for authenticating faraabin connection.
 * 
 * @param LinkHandler.Password 
 */
void fFaraabinLinkHandler_SetPassword(char *password) {
  
  // This line is for suppressing unused variable compiler warning.
  (void)LinkHandler.Password;
  
  LinkHandler.Password = password;
  
}

/**
 * @brief Gets the amount of RAM usage in bytes for link handler.
 * 
 * @return usage Amount of RAM usage in bytes. 
 */
uint32_t fFaraabinLinkHandler_GetRamUsage(void) {
	
	return sizeof(sLinkHandlerInternal);
}

/*
===============================================================================
                ##### fb_link_handler.c Private Functions #####
===============================================================================*/
/**
 * @brief Handles new valid frame.
 * 
 * @param LinkHandler.ClientFrame Pointer to the client frame.
 */
static void fFrameHandler(sClientFrame *clientFrame) {
  
  uint8_t *fobjectType = (clientFrame->FobjectPtr == 0xFFFFFFFFU) ? (uint8_t*)fFaraabinFobjectMcu_GetFobject() : (uint8_t*)clientFrame->FobjectPtr;

  switch((eFaraabin_FobjectType)*fobjectType) {
    
    case eFO_TYPE_MCU: {
      
      fMcuFrameHandler(clientFrame);
      
      break;
    }
    
    case eFO_TYPE_DATABUS: {
      
      fDataBusFrameHandler(clientFrame);
      
      break;
    }
    
    case eFO_TYPE_CODE_BLOCK: {
      
      fCodeBlockFrameHandler(clientFrame);
      
      break;
    }
    
    case eFO_TYPE_STATE_MACHINE: {
      
      fStateMachineFrameHandler(clientFrame);
      
      break;
    }
    
    case eFO_TYPE_STATE_MACHINE_STATE: {
      
      fStateMachineStateFrameHandler(clientFrame);
      
      break;
    }
    
    case eFO_TYPE_STATE_MACHINE_TRANSITION: {
      
      fStateMachineTransitionFrameHandler(clientFrame);
      
      break;
    }
    
    case eFO_TYPE_FUNCTION: {
      
      fFunctionFrameHandler(clientFrame);
      
      break;
    }
    
    case eFO_TYPE_EVENT_GROUP: {
      
      fEventGroupEventHandler(clientFrame);
      
      break;
    }
    
    default: {

      uint8_t controlReqSeq = ClientFrame_GetRequestSequence_(clientFrame->Control);
      fFaraabinFobjectMcu_SendEventSystemExceptionResponse(eMCU_EVENT_ERROR_UNSUPPORTED_FOBJECT_TYPE, controlReqSeq);

      break;
    }
  }
}

/**
 * @brief Handles MCU frames received from the link.
 * 
 * @param LinkHandler.ClientFrame Pointer to the client frame.
 */
static void fMcuFrameHandler(sClientFrame *clientFrame) {
  
  #ifdef FARAABIN_ENABLE
  
  bool errorFobjectProperty = false;
  
  if(clientFrame->FobjectPtr == 0xFFFFFFFFU) {
    clientFrame->FobjectPtr = (uint32_t)fFaraabinFobjectMcu_GetFobject();
  }

  sFaraabinFobjectMcu *mcuHandle = (sFaraabinFobjectMcu*)clientFrame->FobjectPtr;
  uint8_t framePropGroup = ClientFrame_GetPropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_GetPropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_GetRequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_GetAccessType_(clientFrame->Control);

  switch((eFaraabinLinkSerializer_PropertyGroup)framePropGroup) {
    
    case eFB_PROP_GROUP_DICT: {
      
      uByte2 dictIndex;
      dictIndex.Byte[0] = clientFrame->Payload[0];
      dictIndex.Byte[1] = clientFrame->Payload[1];
      
      if(dictIndex.U16 >= fFaraabinDatabase_GetNumberOfAddedDicts()) {

        fFaraabinFobjectMcu_SendEventSystemExceptionResponse(eMCU_EVENT_ERROR_DICT_INDEX_OVERFLOW, controlReqSeq);

        break;
      }
      
      fFaraabinLinkSerializer_SerializeDict(fFaraabinDatabase_GetFobjectPointerFromDict(dictIndex.U16), dictIndex.U16, controlReqSeq);
      
      fFaraabinFobjectMcu_SendEventSystemResponse(eMCU_EVENT_INFO_DICT_END, controlReqSeq);
      
      break;
    }
    
    case eFB_PROP_GROUP_SETTING: {
      
      eFaraabinLinkSerializer_McuPropertyIdSetting propId = (eFaraabinLinkSerializer_McuPropertyIdSetting)framePropId;
      
      switch(propId) {
        
        case eFB_MCU_PROP_ID_SETTING_ENABLE: {
          
          if(controlAccessType == (uint8_t)eFB_CLIENT_FRAME_ACCESS_TYPE_WRITE) {

            sFaraabinFobjectMcu *obj = (sFaraabinFobjectMcu*)clientFrame->FobjectPtr;

            obj->Enable = (*clientFrame->Payload == 0U) ? false : true;
          }
          
          if(controlReqSeq != 0U) {
            fFaraabinLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &mcuHandle->Seq, controlReqSeq, true);
          }
          
          break;
        }
				
				case eFB_MCU_PROP_ID_SETTING_SEND_PROFILER_ENABLE: {
          
          if(controlAccessType == (uint8_t)eFB_CLIENT_FRAME_ACCESS_TYPE_WRITE) {

            sFaraabinFobjectMcu *obj = (sFaraabinFobjectMcu*)clientFrame->FobjectPtr;

            obj->IsSendingCpuProfiler = (*clientFrame->Payload == 0U) ? false : true;
          }
          
          if(controlReqSeq != 0U) {
						
						fFaraabinLinkSerializer_McuSendLive(clientFrame->FobjectPtr, &mcuHandle->Seq, controlReqSeq);
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
    
    case eFB_PROP_GROUP_MONITORING: {
      
      eFaraabinLinkSerializer_McuPropertyIdMonitoring propId = (eFaraabinLinkSerializer_McuPropertyIdMonitoring)framePropId;
      
      switch(propId) {
        
        case eFB_MCU_PROP_ID_MONITORING_VARIABLE: {
          
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
          
          if(controlAccessType == (uint8_t)eFB_CLIENT_FRAME_ACCESS_TYPE_WRITE) {
            
            if(isAccessCb) {
              
              FaraabinVarAccessCallback func = (FaraabinVarAccessCallback)externalFuncPtr.U32;
              if(func != NULL) {
                
                uint8_t res = func(eVAR_ACCESS_TYPE_WRITE, ptr.U32, valuePtr, size.U32);
                if(res != 0U) {
                  //TODO: Send error
                }
              }
              
            } else {
              
              //Write value
              uint8_t *dst = (uint8_t*)ptr.U32;
              uint8_t *src = valuePtr;
              for(uint32_t i = 0U; i < size.U32; i++) {
                dst[i] = src[i];
              }
            }
            
            FaraabinVarAccessCallback func = (FaraabinVarAccessCallback)accessCbFuncPtr.U32;
            if(func != NULL) {
              
              uint8_t res = func(eVAR_ACCESS_TYPE_WRITE, ptr.U32, valuePtr, size.U32);
              if(res != 0U) {
                //TODO: Send error
              }
            }
          }
          
          
          if(controlReqSeq != 0U) {
            
            if(isExternal) {
              
              FaraabinVarAccessCallback func = (FaraabinVarAccessCallback)externalFuncPtr.U32;
              if(func != NULL) {

                uint8_t *dataPtr = NULL;                
                uint8_t res = func(eVAR_ACCESS_TYPE_READ, ptr.U32, dataPtr, size.U32);

                if(res != 0U) {
                  //TODO: Send error
                } else {
                  fFaraabinLinkSerializer_VarSendValue(ptr.U32, (uint32_t)dataPtr, size.U32, &mcuHandle->Seq, controlReqSeq, true);
                }
              }
              
            } else {
              fFaraabinLinkSerializer_VarSendValue(ptr.U32, ptr.U32, size.U32, &mcuHandle->Seq, controlReqSeq, true);
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
    
    case eFB_PROP_GROUP_COMMAND: {
      
      eFaraabinLinkSerializer_McuProperyIdCommand propId = (eFaraabinLinkSerializer_McuProperyIdCommand)framePropId;
      
      switch(propId) {
        
        case eFB_MCU_PROP_ID_COMMAND_PING: {
          
          if(controlReqSeq != 0U) {
      
            fFaraabinLinkSerializer_McuSendPing((uint32_t)mcuHandle, &mcuHandle->Seq, controlReqSeq, clientFrame->Payload);
            
          }
          
          break;
        }
        
        case eFB_MCU_PROP_ID_COMMAND_SEND_LIVE: {
          
          fChrono_Start(&(mcuHandle->ChronoLiveTimeout));
          mcuHandle->_isHostConnected = true;
          
          if(controlReqSeq != 0U) {
          
            fFaraabinLinkSerializer_McuSendLive((uint32_t)0xFFFFFFFFU, &mcuHandle->Seq, controlReqSeq);
            
          }
          
          break;
        }
        
        case eFB_MCU_PROP_ID_COMMAND_SEND_WHOAMI: {
          
          if(controlReqSeq != 0U) {
        
            fFaraabinLinkSerializer_McuSendWhoAmI((uint32_t)mcuHandle, &mcuHandle->Seq, controlReqSeq);
            
            FaraabinFlags.Status.Bitfield.McuReset = 0U;
          }
          
          break;
        }
        
        case eFB_MCU_PROP_ID_COMMAND_SEND_ALL_DICT: {
          
					if(FaraabinFlags.Features.Bitfield.AllowSendDickBlocking) {
						LinkHandler.DictSendingMode.IsBlocking = (clientFrame->Payload[0] == 0U) ? false : true;
					} else {
						LinkHandler.DictSendingMode.IsBlocking = false;
					}
					
					uByte2 tmp2;
					tmp2.Byte[0] = clientFrame->Payload[1];
					tmp2.Byte[1] = clientFrame->Payload[2];
					mcuHandle->SessionId = tmp2.U16;
          
          bool sendAllow = true;
          if(FaraabinFlags.Features.Bitfield.Password == 1U) {

            if(strncmp(LinkHandler.Password, (char*)(&(clientFrame->Payload[3])), strlen(LinkHandler.Password)) != 0) {

              sendAllow = false;

            }
          }
          
          if(sendAllow) {
          
            //Set flag for  send all dict
						fFaraabinLinkBuffer_Clear();						
						
            LinkHandler.DictSendingMode.SendFlag = true;
            LinkHandler.DictSendingMode.ReqSeq = controlReqSeq;
          } else {
            
            Faraabin_EventSystemException_EndResponse_((uint32_t)mcuHandle, 
                                        &mcuHandle->Seq, 
                                        mcuHandle->Enable, 
                                        eMCU_EVENT_ERROR_PASSWORD, 
                                        controlReqSeq);
                        
          }
          
          break;
        }
        
        case eFB_MCU_PROP_ID_COMMAND_RESET_CPU: {

          fFaraabin_ResetMcu();

          Faraabin_EventSystemException_EndResponse_((uint32_t)mcuHandle, 
                                        &mcuHandle->Seq, 
                                        mcuHandle->Enable, 
                                        eMCU_EVENT_ERROR_RESET_FUNC_NOT_IMPLEMENTED, 
                                        controlReqSeq);
          
          break;
        }
        
        case eFB_MCU_PROP_ID_COMMAND_CLEAR_FLAG_BUFFER_OVF: {
          
          FaraabinFlags.Status.Bitfield.BufferOverflow = 0U;

          Faraabin_EventSystem_EndResponse_((uint32_t)mcuHandle, 
                                        &mcuHandle->Seq, 
                                        mcuHandle->Enable, 
                                        eMCU_EVENT_INFO_CLEAR_FLAG_BUFFER_OVF, 
                                        controlReqSeq);
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case eFB_PROP_GROUP_EVENT: {
      
      eFaraabinLinkSerializer_CommonPropertyIdEvent propId = (eFaraabinLinkSerializer_CommonPropertyIdEvent)framePropId;
      
      switch(propId) {
        
        case eFB_COMMON_PROP_ID_EVENT_USER_TERMINAL: {
          
          clientFrame->Payload[clientFrame->PayloadSize] = 0x00U; // Terminate end of data
          if(mcuHandle->fpUserTerminalCallback != NULL) {
            mcuHandle->fpUserTerminalCallback(clientFrame->Payload, clientFrame->PayloadSize);
          }
          
          if(controlReqSeq != 0U) {
            fFaraabinFobjectMcu_SendEventSystemResponse(eMCU_EVENT_INFO_USER_DATA_RECEIVED, controlReqSeq);
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
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)mcuHandle,
                                        &mcuHandle->Seq,
                                        mcuHandle->Enable,
                                        eMCU_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq);
  }
  
  #endif
}

/**
 * @brief Handles databus frames received from the link.
 * 
 * @param LinkHandler.ClientFrame Pointer to the client frame.
 */
static void fDataBusFrameHandler(sClientFrame* clientFrame) {
  
  #ifdef FARAABIN_ENABLE
  
  bool errorFobjectProperty = false;
  
  sFaraabinFobjectDataBus *dbHandle = (sFaraabinFobjectDataBus*)clientFrame->FobjectPtr;
  uint8_t *param = clientFrame->Payload;
  uint8_t framePropGroup = ClientFrame_GetPropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_GetPropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_GetRequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_GetAccessType_(clientFrame->Control);

  switch((eFaraabinLinkSerializer_PropertyGroup)framePropGroup) {

    case eFB_PROP_GROUP_SETTING: {
      
      eFaraabinLinkSerializer_DataBusPropertyIdSetting propId = (eFaraabinLinkSerializer_DataBusPropertyIdSetting)framePropId;
      
      switch(propId) {
        
        case eFB_DB_PROP_ID_SETTING_ENABLE: {

              if(controlAccessType == (uint8_t)eFB_CLIENT_FRAME_ACCESS_TYPE_WRITE) {
          
                dbHandle->Enable = (*clientFrame->Payload == 0U) ? false : true;
              }
              
              if(controlReqSeq != 0U) {
                fFaraabinLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq, true);
              }
              
              break;
            }
        
            case eFB_DB_PROP_ID_SETTING_ALL: {

              if(controlReqSeq != 0U) {
            
                fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
              }
              
              break;
            }
            
            case eFB_DB_PROP_ID_SETTING_ITEM_ENABLE: {

              uByte2 channelNo;
              channelNo.Byte[0] = param[0];
              channelNo.Byte[1] = param[1];
              
              bool itemEnable = (param[0] != 0U);
              
              if(channelNo.U16 >= dbHandle->ChannelQty) {
            
                Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_ERROR_NOT_FREE_LOCATION_FOR_ATTACH, controlReqSeq);
                return;
            
              } else {
            
                if(dbHandle->_pBufferChannels[channelNo.U16].ItemFobjectPtr != 0U) {
                  dbHandle->_pBufferChannels[channelNo.U16].Enable = itemEnable;
              
                  if(itemEnable == true) {
              
                    dbHandle->AvailableItemsQty++;
                
                  } else {
                
                    dbHandle->AvailableItemsQty--;
                
                  }
                }
            
              }
          
              if(controlReqSeq != 0U) {
          
                fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
              }
          
              break;
        }
            
        case eFO_DB_PROP_ID_SETTING_API_TRIG_ENABLE: {

          dbHandle->ApiTrigEnable = (param[0] != 0U);
          
          if(controlReqSeq != 0U) {
          
            fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
          }
          
          break;
        }
        
        case eFO_DB_PROP_ID_SETTING_CH_TRIG_ENABLE: {

          dbHandle->ChTrigEnable = (param[0] != 0U);
          
          if(controlReqSeq != 0U) {
          
            fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
          }
          
          break;
        }
        
        case eFO_DB_PROP_ID_SETTING_CH_TRIG_CONFIG: {

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
          
            fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
          }
          
          break;
        }
				
				case eFO_DB_PROP_ID_SETTING_STREAM_DIVIDEBY: {

              uByte2 divideBy;
              divideBy.Byte[0] = param[0];
              divideBy.Byte[1] = param[1];
              
              if(divideBy.U16 == 0) {
            
                Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_ERROR_PARAM, controlReqSeq);
                return;
            
              } else {
            
								dbHandle->StreamDivideBy = divideBy.U16;
              }
          
              if(controlReqSeq != 0U) {
          
                fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
              }
          
              break;
        }
				
				case eFO_DB_PROP_ID_SETTING_TIMER_DIVIDEBY: {

              uByte2 divideBy;
              divideBy.Byte[0] = param[0];
              divideBy.Byte[1] = param[1];
              
              if(divideBy.U16 == 0) {
            
                Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_ERROR_PARAM, controlReqSeq);
                return;
            
              } else {
            
								dbHandle->TimerDivideBy = divideBy.U16;
              }
          
              if(controlReqSeq != 0U) {
          
                fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
              }
          
              break;
        }
				
				case eFO_DB_PROP_ID_SETTING_TRIG_DIVIDEBY: {

              uByte2 divideBy;
              divideBy.Byte[0] = param[0];
              divideBy.Byte[1] = param[1];
              
              if(divideBy.U16 == 0) {
            
                Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_ERROR_PARAM, controlReqSeq);
                return;
            
              } else {
            
								dbHandle->TrigDivideBy = divideBy.U16;
              }
          
              if(controlReqSeq != 0U) {
          
                fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
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
    
    case eFB_PROP_GROUP_MONITORING: {
      
      eFaraabinLinkSerializer_DataBusPropertyIdMonitoring propId = (eFaraabinLinkSerializer_DataBusPropertyIdMonitoring)framePropId;
      
      switch(propId) {
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case eFB_PROP_GROUP_COMMAND: {
      
      eFaraabinLinkSerializer_DataBusPropertyIdCommand propId = (eFaraabinLinkSerializer_DataBusPropertyIdCommand)framePropId;
      
      switch(propId) {
        
        case eFB_DB_PROP_ID_COMMAND_ATTACH: {
          
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
          
          uint8_t ret = FARAABIN_DB_OK;

          if(itemFobjectType == (uint8_t)eFO_TYPE_CODE_BLOCK) {
            ret = fFaraabinFobjectDataBus_AttachCodeBlockToChannel(dbHandle, itemChannelNo.U16, itemFobjectPtr.U32);
          } else {
            ret = fFaraabinFobjectDataBus_AttachVariableToChannel(dbHandle, itemChannelNo.U16, itemFobjectPtr.U32, itemFobjectParam.U16, varTypeArchitecture, varPrimitiveId, itemFobjectType);
          }

          if(ret != FARAABIN_DB_OK) {

            Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_ERROR_ATTACH, controlReqSeq);
          } else {
            
            if(controlReqSeq != 0U) {
          
              fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
            }
          }

          break;
        }
        
        case eFB_DB_PROP_ID_COMMAND_DETACH: {
          
          uByte2 itemChannelNo;
          itemChannelNo.Byte[0] = param[0];
          itemChannelNo.Byte[1] = param[1];
          
          uint8_t ret = FARAABIN_DB_OK;

          ret = fFaraabinFobjectDataBus_DetachFromChannel(dbHandle, itemChannelNo.U16);

          if(ret != FARAABIN_DB_OK) {

            Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_ERROR_DETACH, controlReqSeq);
          } else {
            
            if(controlReqSeq != 0U) {
          
              fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
            }
          }

          break;
        }
        
        case eFB_DB_PROP_ID_COMMAND_DETACH_ALL: {
          
          if(fFaraabinFobjectDataBus_DetachAllChannels(dbHandle) != FARAABIN_DB_OK) {

            Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_ERROR_CODE_BLOCK_NOT_EMPTY, controlReqSeq);
          }
      
          if(controlReqSeq != 0U) {
            
            fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
          }
          
          break;
        }
        
        case eFB_DB_PROP_ID_COMMAND_START_STREAM: {
          
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
          
          Faraabin_EventSystem_ParamEndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_INFO_STATE_CHANGE, eventParam, 7U, controlReqSeq);
          
          break;
        }
        
        case eFB_DB_PROP_ID_COMMAND_START_TRIG: {
          
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
            
            Faraabin_EventSystem_ParamEndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_INFO_STATE_CHANGE, eventParam, 11U, controlReqSeq);        
          }
          
          break;
        }
        
        case eFB_DB_PROP_ID_COMMAND_START_TIMER: {
          
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
            
            Faraabin_EventSystem_ParamEndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_INFO_STATE_CHANGE, eventParam, 11U, controlReqSeq);        
          }
          
          break;
        }
        
        case eFB_DB_PROP_ID_COMMAND_STOP: {
          
          fFaraabinFobjectDataBus_Stop(dbHandle);
      
          Faraabin_EventSystem_ParamEndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_INFO_STATE_CHANGE, (uint8_t*)&dbHandle->CurrentState, 1, controlReqSeq);
          
          break;
        }
        
        case eFB_DB_PROP_ID_COMMAND_CAPTURE_SEND: {
          
          dbHandle->CaptureSendingCnt = 0U;
          dbHandle->CaptureSendingReqSeq = controlReqSeq;
          
          dbHandle->CaptureSendingQty = dbHandle->QueueItemCount;
          
          if(dbHandle->CaptureSendingQty == 0U) {
            
            Faraabin_EventSystemException_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_ERROR_NO_CAPTURE_DATA_FOR_SEND, controlReqSeq);
          
          } else {
            
            dbHandle->CurrentState = eDATABUS_STATE_CAPTURE_SEND;
            
            uint8_t eventParam[5];
            
            eventParam[0] = (uint8_t)dbHandle->CurrentState;
            
            uByte4 tmp4;
            tmp4.U32 = dbHandle->CaptureSendingQty;
            eventParam[1] = tmp4.Byte[0];
            eventParam[2] = tmp4.Byte[1];
            eventParam[3] = tmp4.Byte[2];
            eventParam[4] = tmp4.Byte[3];
            
            Faraabin_EventSystem_ParamResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_INFO_STATE_CHANGE, eventParam, 5U, controlReqSeq);
          }
          
          break;
        }
        
        case eFB_DB_PROP_ID_COMMAND_MANUAL_TRIG: {
          
          fFaraabinFobjectDataBus_ForceManualTrigger(dbHandle);
            
          Faraabin_EventSystem_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_INFO_MANUAL_TRIG, controlReqSeq);
          
          break;
        }
        
        case eFB_DB_PROP_ID_COMMAND_ENABLE_ALL_ITEM: {
          
          for(uint16_t i = 0; i < dbHandle->ChannelQty; i++) {
            dbHandle->_pBufferChannels[i].Enable = true;
          }
          
          if(controlReqSeq != 0U) {
          
            fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
          }
          
          break;
        }
        
        case eFB_DB_PROP_ID_COMMAND_DISABLE_ALL_ITEM: {
          
          for(uint16_t i = 0; i < dbHandle->ChannelQty; i++) {
            dbHandle->_pBufferChannels[i].Enable = false;
          }
          
          if(controlReqSeq != 0U) {
          
            fFaraabinLinkSerializer_DataBusSendSetting(clientFrame->FobjectPtr, &dbHandle->Seq, controlReqSeq);
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
    
    case eFB_PROP_GROUP_EVENT: {
      
      eFaraabinLinkSerializer_CommonPropertyIdEvent propId = (eFaraabinLinkSerializer_CommonPropertyIdEvent)framePropId;
      
      switch(propId) {
        
        case eFB_COMMON_PROP_ID_EVENT_USER_TERMINAL: {

          clientFrame->Payload[clientFrame->PayloadSize] = 0x00U; // Terminate end of data
          if(dbHandle->fpUserTerminalCallback != NULL) {
            dbHandle->fpUserTerminalCallback(clientFrame->Payload, clientFrame->PayloadSize);
          }
          
          if(controlReqSeq != 0U) {
            
            Faraabin_EventSystem_EndResponse_((uint32_t)dbHandle, &dbHandle->Seq, dbHandle->Enable, eDATABUS_EVENT_INFO_USER_DATA_RECEIVED, controlReqSeq);
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
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)dbHandle,
                                        &dbHandle->Seq,
                                        dbHandle->Enable,
                                        eDATABUS_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq);
  }
  
  #endif
}

/**
 * @brief Handles codeblock frames received from the link.
 * 
 * @param LinkHandler.ClientFrame Pointer to the client frame.
 */
static void fCodeBlockFrameHandler(sClientFrame* clientFrame) {
  
  #ifdef FARAABIN_ENABLE
  #ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  
  bool errorFobjectProperty = false;
  
  sCpuProcess *cbHandle = (sCpuProcess*)clientFrame->FobjectPtr;
  uint8_t *param = clientFrame->Payload;
  uint8_t framePropGroup = ClientFrame_GetPropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_GetPropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_GetRequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_GetAccessType_(clientFrame->Control);

  switch((eFaraabinLinkSerializer_PropertyGroup)framePropGroup) {
    
    case eFB_PROP_GROUP_SETTING: {
      
      eFaraabinLinkSerializer_CodeBlockPropertyIdSetting propId = (eFaraabinLinkSerializer_CodeBlockPropertyIdSetting)framePropId;
      
      switch(propId) {
        
        case eFB_CB_PROP_ID_SETTING_ENALBE: {
          
          if(controlAccessType == (uint8_t)eFB_CLIENT_FRAME_ACCESS_TYPE_WRITE) {
      
            cbHandle->_enable = (*clientFrame->Payload == 0U) ? false : true;
          }
          
          if(controlReqSeq != 0U) {
            fFaraabinLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq, true);
          }
          
          break;
        }
        
        case eFB_CB_PROP_ID_SETTING_ALL: {
          
          if(controlReqSeq != 0U) {
        
            fFaraabinLinkSerializer_CodeBlockSendSetting(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq);
          }
          
          break;
        }
        
        case eFB_CB_PROP_ID_SETTING_LIMIT_PARAM: {
          
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
            
            fFaraabinLinkSerializer_CodeBlockSendSetting(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq);
          }
          
          break;
        }
        
        case eFB_CB_PROP_ID_SETTING_EVENT_TO_STREAM_ENABLE: {
          
          cbHandle->IsSendingEventsToDbStreamEnabled = !(param[0] == 0U);
          
          if(controlReqSeq != 0U) {
            
            fFaraabinLinkSerializer_CodeBlockSendSetting(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq);
          }
          
          break;
        }
        
        case eFB_CB_PROP_ID_SETTING_EVENT_TO_CAPTURE_ENABLE: {
          
          cbHandle->ISSendingEventsToDbCaptureEnabled = !(param[0] == 0U);
          
          if(controlReqSeq != 0U) {
            
            fFaraabinLinkSerializer_CodeBlockSendSetting(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq);
          }
          
          break;
        }
        
        case eFB_CB_PROP_ID_SETTING_DATA_TO_STREAM_ENABLE: {
          
          cbHandle->IsSendingDataToDbStreamEnabled = !(param[0] == 0U);
          
          if(controlReqSeq != 0U) {
            
            fFaraabinLinkSerializer_CodeBlockSendSetting(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq);
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
    
    case eFB_PROP_GROUP_MONITORING: {
      
      eFaraabinLinkSerializer_CodeBlockPropertyIdMonitoring propId = (eFaraabinLinkSerializer_CodeBlockPropertyIdMonitoring)framePropId;
      
      switch(propId) {
        
        case eFB_CB_PROP_ID_MONITORING_VALUE: {
          
          if(controlReqSeq != 0U) {
            fFaraabinLinkSerializer_CodeBlockSendValue(clientFrame->FobjectPtr, &cbHandle->_seq, controlReqSeq, true);
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
    
    case eFB_PROP_GROUP_COMMAND: {
      
      eFaraabinLinkSerializer_CodeBlockPropertyIdCommand propId = (eFaraabinLinkSerializer_CodeBlockPropertyIdCommand)framePropId;
      
      switch(propId) {
        
        case eFB_CB_PROP_ID_COMMAND_RESET_VALUE: {
          
					fCpuProfiler_ProcessResetStatistics(cbHandle);
          
          Faraabin_EventSystem_EndResponse_((uint32_t)cbHandle, &cbHandle->_seq, cbHandle->_enable, eCB_EVENT_INFO_RESET, controlReqSeq);
      
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break; 
        }
      }
      
      break;
    }
    
    case eFB_PROP_GROUP_EVENT: {
      
      eFaraabinLinkSerializer_CommonPropertyIdEvent propId = (eFaraabinLinkSerializer_CommonPropertyIdEvent)framePropId;
      
      switch(propId) {
        
        case eFB_COMMON_PROP_ID_EVENT_USER_TERMINAL: {
          
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
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)cbHandle,
                                        &cbHandle->_seq,
                                        cbHandle->_enable,
                                        eCB_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq);
  }
  
  #endif
  #endif
}

/**
 * @brief Handles state machine frames received from the link.
 * 
 * @param LinkHandler.ClientFrame Pointer to the client frame.
 */
static void fStateMachineFrameHandler(sClientFrame* clientFrame) {
  
  #ifdef FARAABIN_ENABLE
  #ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
  
  bool errorFobjectProperty = false;
  
  sStateMachine *smHandle = (sStateMachine*)clientFrame->FobjectPtr;
  uint8_t *payload = clientFrame->Payload;
  uint8_t framePropGroup = ClientFrame_GetPropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_GetPropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_GetRequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_GetAccessType_(clientFrame->Control);

  switch((eFaraabinLinkSerializer_PropertyGroup)framePropGroup) {
    
    case eFB_PROP_GROUP_SETTING: {
      
      eFaraabinLinkSerializer_StateMachinePropertyIdSetting propId = (eFaraabinLinkSerializer_StateMachinePropertyIdSetting)framePropId;
      
      switch(propId) {
        
        case eFB_SM_PROP_ID_SETTING_ENALBE: {
          
          if(controlAccessType == (uint8_t)eFB_CLIENT_FRAME_ACCESS_TYPE_WRITE) {
      
            smHandle->_enable = (*clientFrame->Payload == 0U) ? false : true;
          }
          
          if(controlReqSeq != 0U) {
            fFaraabinLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &smHandle->_seq, controlReqSeq, true);
          }
          
          break;
        }
        
        case eFB_SM_PROP_ID_SETTING_ALL: {
          
          if(controlReqSeq != 0U) {
        
            fFaraabinLinkSerializer_StateMachineSendSetting(clientFrame->FobjectPtr, &smHandle->_seq, controlReqSeq);
          }
          
          break;
        }
        
        case eFB_SM_PROP_ID_SETTING_ENABLE_ALL_TRANSITIONS: {
          
          smHandle->AllTransitionEnable = (payload[0] != 0U);
          
          if(controlReqSeq != 0U) {
            
            fFaraabinLinkSerializer_StateMachineSendSetting(clientFrame->FobjectPtr, &smHandle->_seq, controlReqSeq);
          }
          
          break;
        }
        
        case eFB_SM_PROP_ID_SETTING_ENABLE_ALL_STATE_TRANSITIONS: {
          
          uByte4 statePtr;
          statePtr.Byte[0] = payload[0];
          statePtr.Byte[1] = payload[1];
          statePtr.Byte[2] = payload[2];
          statePtr.Byte[3] = payload[3];
          
          bool enable = (payload[4] != 0U);
          
      
          sState *state = (sState*)statePtr.U32;
          state->AllTransitionEnable = enable;
          
          if(controlReqSeq != 0U) {
            
            fFaraabinLinkSerializer_StateMachineSendSetting(clientFrame->FobjectPtr, &smHandle->_seq, controlReqSeq);
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
    
    case eFB_PROP_GROUP_MONITORING: {
      
      eFaraabinLinkSerializer_StateMachinePropertyIdMonitoring propId = (eFaraabinLinkSerializer_StateMachinePropertyIdMonitoring)framePropId;
      
      switch(propId) {
        
        case eFB_SM_PROP_ID_MONITORING_VALUE: {

          if(controlReqSeq != 0U) {
            fFaraabinLinkSerializer_StateMachineSendValue(clientFrame->FobjectPtr, &smHandle->_seq, controlReqSeq, true);
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
    
    case eFB_PROP_GROUP_COMMAND: {
      
      eFaraabinLinkSerializer_StateMachinePropertyIdCommand propId = (eFaraabinLinkSerializer_StateMachinePropertyIdCommand)framePropId;
      
      switch(propId) {
        
        case eFB_SM_PROP_ID_COMMAND_MANUAL_TRANSITION: {

          uByte4 transitionPtr;
          transitionPtr.Byte[0] = payload[0];
          transitionPtr.Byte[1] = payload[1];
          transitionPtr.Byte[2] = payload[2];
          transitionPtr.Byte[3] = payload[3];
      
          sTransition *transition = (sTransition*)transitionPtr.U32;
          if(transition->_srcState == smHandle->_currentState) {
            
            transition->_manualTrans = true;
            
            Faraabin_EventSystem_EndResponse_((uint32_t)smHandle, &smHandle->_seq, smHandle->_enable, eSM_EVENT_INFO_CMD_MANUAL_TRANSITION, controlReqSeq);
            
          } else {
            
            Faraabin_EventSystemException_EndResponse_((uint32_t)smHandle, &smHandle->_seq, smHandle->_enable, eSM_EVENT_ERROR_MANUAL_TRANSITION_SRC_MISMATCH, controlReqSeq);
            
          }
          
          break;
        }
        
        case eFB_SM_PROP_ID_COMMAND_VIRTUAL_TRANSITION: {

          uByte4 dstStatePtr;
          dstStatePtr.Byte[0] = payload[0];
          dstStatePtr.Byte[1] = payload[1];
          dstStatePtr.Byte[2] = payload[2];
          dstStatePtr.Byte[3] = payload[3];
      
          fStateMachine_AdvFeat_ForceVirtualTransition(smHandle, (sState*)dstStatePtr.U32);
          
          Faraabin_EventSystem_EndResponse_((uint32_t)smHandle, &smHandle->_seq, smHandle->_enable, eSM_EVENT_INFO_CMD_VIRTUAL_TRANSITION, controlReqSeq);
          
          break;
        }
        
        case eFB_SM_PROP_ID_COMMAND_RESET: {

          fStateMachine_Reset(smHandle);
          
          Faraabin_EventSystem_EndResponse_((uint32_t)smHandle, &smHandle->_seq, smHandle->_enable, eSM_EVENT_INFO_CMD_RESET, controlReqSeq);
          
          break;
        }
        
        default: {
          
          errorFobjectProperty = true;
          break;
        }
      }
      
      break;
    }
    
    case eFB_PROP_GROUP_EVENT: {
      
      eFaraabinLinkSerializer_CommonPropertyIdEvent propId = (eFaraabinLinkSerializer_CommonPropertyIdEvent)framePropId;
      
      switch(propId) {
        
        case eFB_COMMON_PROP_ID_EVENT_USER_TERMINAL: {

          clientFrame->Payload[clientFrame->PayloadSize] = 0x00U; // Terminate end of data
//          if(smHandle->fpUserDataReceivedEvent != NULL) {
//            sStateMachine_UserDataReceivedEventArgs ev;
//            
//            ev.pUserData = clientFrame->Payload;
//            ev.UserDataSize = clientFrame->PayloadSize;
//            smHandle->fpUserDataReceivedEvent((object)smHandle, &ev);
//          }
          
          if(controlReqSeq != 0U) {
            Faraabin_EventSystem_EndResponse_((uint32_t)smHandle, &smHandle->_seq, smHandle->_enable, eSM_EVENT_INFO_USER_DATA_RECEIVED, controlReqSeq);
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
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)smHandle,
                                        &smHandle->_seq,
                                        smHandle->_enable,
                                        eSM_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq);
  }
  
  #endif
  #endif
}

/**
 * @brief Handles state machine's state frames received from the link.
 * 
 * @param LinkHandler.ClientFrame Pointer to the client frame.
 */
static void fStateMachineStateFrameHandler(sClientFrame* clientFrame) {
  
  #ifdef FARAABIN_ENABLE
  #ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
  
  bool errorFobjectProperty = false;
  
  sState *smStateHandle = (sState*)clientFrame->FobjectPtr;
  uint8_t framePropGroup = ClientFrame_GetPropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_GetPropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_GetRequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_GetAccessType_(clientFrame->Control);

  switch((eFaraabinLinkSerializer_PropertyGroup)framePropGroup) {
    
    case eFB_PROP_GROUP_SETTING: {
      
      eFaraabinLinkSerializer_StateMachinePropertyIdSetting propId = (eFaraabinLinkSerializer_StateMachinePropertyIdSetting)framePropId;
      
      switch(propId) {
        
        case eFB_SM_PROP_ID_SETTING_ENALBE: {
          
          if(controlAccessType == (uint8_t)eFB_CLIENT_FRAME_ACCESS_TYPE_WRITE) {
      
            smStateHandle->_enable = (*clientFrame->Payload == 0U) ? false : true;
          }
          
          if(controlReqSeq != 0U) {
            fFaraabinLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &smStateHandle->_seq, controlReqSeq, true);
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
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)smStateHandle,
                                        &smStateHandle->_seq,
                                        smStateHandle->_enable,
                                        eSM_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq);
  }
  
  #endif
  #endif
}

/**
 * @brief Handles state machine's transition frames received from the link.
 * 
 * @param LinkHandler.ClientFrame Pointer to the client frame.
 */
static void fStateMachineTransitionFrameHandler(sClientFrame* clientFrame) {
  
  #ifdef FARAABIN_ENABLE
  #ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
  
  bool errorFobjectProperty = false;
  
  sTransition *smTransitionHandle = (sTransition*)clientFrame->FobjectPtr;
  uint8_t framePropGroup = ClientFrame_GetPropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_GetPropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_GetRequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_GetAccessType_(clientFrame->Control);

  switch((eFaraabinLinkSerializer_PropertyGroup)framePropGroup) {
    
    case eFB_PROP_GROUP_SETTING: {
      
      eFaraabinLinkSerializer_StateMachinePropertyIdSetting propId = (eFaraabinLinkSerializer_StateMachinePropertyIdSetting)framePropId;
      
      switch(propId) {
        
        case eFB_SM_PROP_ID_SETTING_ENALBE: {
          
          if(controlAccessType == (uint8_t)eFB_CLIENT_FRAME_ACCESS_TYPE_WRITE) {
      
            smTransitionHandle->_enable = (*clientFrame->Payload == 0U) ? false : true;
          }
          
          if(controlReqSeq != 0U) {
            fFaraabinLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &smTransitionHandle->_seq, controlReqSeq, true);
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
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)smTransitionHandle,
                                        &smTransitionHandle->_seq,
                                        smTransitionHandle->_enable,
                                        eSM_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq);
  }
  
  #endif
  #endif
}

/**
 * @brief Handles function frames received from the link.
 * 
 * @param LinkHandler.ClientFrame Pointer to the client frame.
 */
static void fFunctionFrameHandler(sClientFrame* clientFrame) {
  
  #ifdef FARAABIN_ENABLE
  #ifdef FB_FEATURE_FLAG_MCU_CLI
  
  bool errorFobjectProperty = false;
    
  sFaraabinFobjectFunction *fnHandle = (sFaraabinFobjectFunction*)clientFrame->FobjectPtr;
  uint8_t *param = (uint8_t*)clientFrame->Payload;
  uint8_t framePropGroup = ClientFrame_GetPropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_GetPropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_GetRequestSequence_(clientFrame->Control);
  
  switch((eFaraabinLinkSerializer_PropertyGroup)framePropGroup) {
    
    case eFB_PROP_GROUP_COMMAND: {
      
      eFaraabinLinkSerializer_FunctionPropertyIdCommand propId = (eFaraabinLinkSerializer_FunctionPropertyIdCommand)framePropId;

      switch(propId) {
        
        case eFB_FN_PROP_ID_COMMAND_RUN: {
          
          uByte4 objectPtr;
          objectPtr.Byte[0] = param[0];
          objectPtr.Byte[1] = param[1];
          objectPtr.Byte[2] = param[2];
          objectPtr.Byte[3] = param[3];
          
          uByte2 paramSize;
          paramSize.Byte[0] = param[4];
          paramSize.Byte[1] = param[5];

          uint8_t ret = fFaraabinFunctionEngine_Start((uint32_t)fnHandle, objectPtr.U32, &(param[6]), paramSize.U16);

          if(ret == FB_FENGINE_FUNCTION_ALREADY_RUNNING) {

            Faraabin_EventSystemExceptionExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, eFN_EVENT_ERROR_CMD_IS_RUNNING_NOW, controlReqSeq);
            
            break;
          }

          if(ret == FB_FENGINE_MAX_CONCURRENT_LIMIT_REACHED) {

            Faraabin_EventSystemExceptionExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, eFN_EVENT_ERROR_MAX_CONCURRENT_CMD_REACHED, controlReqSeq);
            
            break;
          }

          Faraabin_EventSystemExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, eFN_EVENT_INFO_RUN, controlReqSeq);
          
          break;
        }
        
        case eFB_FN_PROP_ID_COMMAND_STOP: {
          
          uByte4 objectPtr;
          objectPtr.Byte[0] = param[0];
          objectPtr.Byte[1] = param[1];
          objectPtr.Byte[2] = param[2];
          objectPtr.Byte[3] = param[3];
          
          uint8_t ret = fFaraabinFunctionEngine_Stop(clientFrame->FobjectPtr, objectPtr.U32);

          if(ret == FB_FENGINE_FUNCTION_NOT_IN_LIST) {

            Faraabin_EventSystemExceptionExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, eFN_EVENT_ERROR_CMD_NOT_FOUND, controlReqSeq);
            
            break;
          }

          Faraabin_EventSystemExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, eFN_EVENT_INFO_STOP, controlReqSeq);

          break;
        }
        
        case eFB_FN_PROP_ID_COMMAND_PAUSE: {

          uByte4 objectPtr;
          objectPtr.Byte[0] = param[0];
          objectPtr.Byte[1] = param[1];
          objectPtr.Byte[2] = param[2];
          objectPtr.Byte[3] = param[3];
          
          uint8_t ret = fFaraabinFunctionEngine_Pause(clientFrame->FobjectPtr, objectPtr.U32);
          
          if(ret == FB_FENGINE_FUNCTION_NOT_IN_LIST) {
            
            Faraabin_EventSystemException_EndResponse_((uint32_t)fnHandle, &fnHandle->Seq, true, eFN_EVENT_ERROR_CMD_NOT_FOUND, controlReqSeq);
            
            break;
          }

          Faraabin_EventSystemExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, eFN_EVENT_INFO_PAUSE, controlReqSeq);

          break;
        }
        
        case eFB_FN_PROP_ID_COMMAND_RESUME: {

          uByte4 objectPtr;
          objectPtr.Byte[0] = param[0];
          objectPtr.Byte[1] = param[1];
          objectPtr.Byte[2] = param[2];
          objectPtr.Byte[3] = param[3];
          
          uint8_t ret = fFaraabinFunctionEngine_Resume(clientFrame->FobjectPtr, objectPtr.U32);
          
          if(ret == FB_FENGINE_FUNCTION_NOT_IN_LIST) {
            
            Faraabin_EventSystemException_EndResponse_((uint32_t)fnHandle, &fnHandle->Seq, true, eFN_EVENT_ERROR_CMD_NOT_FOUND, controlReqSeq);
            
            break;
            
          }

          Faraabin_EventSystemExtended_EndResponse_((uint32_t)fnHandle, objectPtr.U32, &fnHandle->Seq, true, eFN_EVENT_INFO_RESUME, controlReqSeq);

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
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)fnHandle,
                                        &fnHandle->Seq,
                                        true,
                                        eFN_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq);
  }
  
  #endif
  #endif
}

/**
 * @brief Handles event group frames received from the link.
 * 
 * @param LinkHandler.ClientFrame Pointer to the client frame.
 */
static void fEventGroupEventHandler(sClientFrame* clientFrame) {
  
  #ifdef FARAABIN_ENABLE
  
  bool errorFobjectProperty = false;
  
  sFaraabinFobjectEventGroup *egHandle = (sFaraabinFobjectEventGroup*)clientFrame->FobjectPtr;
  uint8_t framePropGroup = ClientFrame_GetPropGroup_(clientFrame->FobjectProperty);
  uint8_t framePropId = ClientFrame_GetPropId_(clientFrame->FobjectProperty);
  uint8_t controlReqSeq = ClientFrame_GetRequestSequence_(clientFrame->Control);
  uint8_t controlAccessType = ClientFrame_GetAccessType_(clientFrame->Control);

  switch((eFaraabinLinkSerializer_PropertyGroup)framePropGroup) {
    
    case eFB_PROP_GROUP_SETTING: {
      
      eFaraabinLinkSerializer_FunctionPropertyIdSetting propId = (eFaraabinLinkSerializer_FunctionPropertyIdSetting)framePropId;

      switch(propId) {
        
        case eFB_FN_PROP_ID_SETTING_ENALBE: {
          
          if(controlAccessType == (uint8_t)eFB_CLIENT_FRAME_ACCESS_TYPE_WRITE) {
            egHandle->Enable = (*clientFrame->Payload == 0U) ? false : true;
          }
          
          if(controlReqSeq != 0U) {
            fFaraabinLinkSerializer_CommonSendEnable(clientFrame->FobjectPtr, &egHandle->Seq, controlReqSeq, true);
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
    
    case eFB_PROP_GROUP_EVENT: {
      
      eFaraabinLinkSerializer_CommonPropertyIdEvent propId = (eFaraabinLinkSerializer_CommonPropertyIdEvent)framePropId;

      switch(propId) {
        
        case eFB_COMMON_PROP_ID_EVENT_USER_TERMINAL: {
          
          clientFrame->Payload[clientFrame->PayloadSize] = 0x00U; // Terminate end of data
          if(egHandle->fpUserTerminalCallback != NULL) {
            egHandle->fpUserTerminalCallback(clientFrame->Payload, clientFrame->PayloadSize);
          }
          
          if(controlReqSeq != 0U) {
            Faraabin_EventSystem_EndResponse_((uint32_t)egHandle, &egHandle->Seq, egHandle->Enable, eEG_EVENT_INFO_USER_DATA_RECEIVED, controlReqSeq);
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
    
    Faraabin_EventSystemException_ParamEndResponse_((uint32_t)egHandle,
                                        &egHandle->Seq,
                                        egHandle->Enable,
                                        eEG_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
                                        (uint8_t*)&(clientFrame->FobjectProperty),
                                        1,
                                        controlReqSeq);
  }
  
  #endif
}

/**
 * @brief Transmits available data in TX buffer to the link.
 * 
 * @param flush Forces the function to flsuh the buffer.
 */
static void fSendCircularBuffer(bool flush) {
  uint8_t *buffPtr = NULL;
  uint16_t transmitSize = 0;
  
  if(LinkHandler.IsFlushingBuffer == true) {
    return;
  }
  
  if(flush == true) {
    LinkHandler.IsFlushingBuffer = true;
  }
  
  do{
    
    if(flush == true) {
  
      while(fFaraabin_IsSending() == true) {
        
        if(fChrono_IsTimeout(&(LinkHandler.ChronoPortSending)) == true) {
          
          fFaraabinFobjectMcu_SendEventSystemException(eMCU_EVENT_ERROR_TX_FRAME_TIMEOUT);

          if(flush) {
            LinkHandler.IsFlushingBuffer = false;
          }
          
          return;
        }
      }
    } else {
      
      if(fFaraabin_IsSending() == true) {
        return;
      }
      
    }

    transmitSize = fFaraabinLinkSerializer_FlushBuffer(&buffPtr);
    if(transmitSize > 0U) {
      if(buffPtr == NULL) {
        //Return mem error
        break;
      }
      fChrono_StartTimeoutMs(&LinkHandler.ChronoPortSending, (transmitSize * FB_BYTE_SENDING_TIME_MS) * 2U);
      
      (fFaraabinFobjectMcu_GetFobject())->StatisticsTxBytesCnt += transmitSize;
      
      if(fFaraabin_Send(buffPtr, transmitSize) != 0U) {
        
        fFaraabinFobjectMcu_SendEventSystemException(eMCU_EVENT_ERROR_TX_FRAME_SEND);
        
      }
    } else {
      break;
    }
  }while((flush == true) && (transmitSize > 0U));
  
  if(flush == true) {
    LinkHandler.IsFlushingBuffer = false;
  }
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
