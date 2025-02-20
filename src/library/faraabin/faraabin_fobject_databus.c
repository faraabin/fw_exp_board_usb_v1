/**
 ******************************************************************************
 * @file           : faraabin_fobject_databus.c
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
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_databus.h"

#include "faraabin.h"
#include "faraabin_fobject.h"
#include "faraabin_database.h"
#include "faraabin_config.h"

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
#include "add_on/cpu_profiler/faraabin_addon_cpu_profiler.h"
#endif

#include "add_on/runtime_scaler/runtime_scaler.h"

#include "faraabin_fobject_system_event_wrapper.h"
#include "faraabin_link_handler.h"
#include "faraabin_internal.h"

#include <stdlib.h>

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/**
 * @brief Increments the number of items in databus queue.
 * 
 */
#define INCREMENT_COUNT_() me->QueueItemCount++

/**
 * @brief Decrements the number of items in databus queue.
 * 
 */
#define DECREMENT_COUNT_() me->QueueItemCount--

/**
 * @brief Increments rear (tail) index of the databus queue.
 * 
 */
#define INCREMENT_REAR_INDEX_()    me->_queueRearIndex = (me->_queueRearIndex + 1) % me->BufferCaptureSize

/**
 * @brief Increments front (head) index of the databus queue.
 * 
 */
#define INCREMENT_FRONT_INDEX_()   me->_queueFrontIndex   = (me->_queueFrontIndex + 1) % me->BufferCaptureSize

/**
 * @brief Write an item in the databus capture buffer.
 * 
 * @param index_ Index of the item in capture buffer.
 * @param item_ Item to be added to the capture buffer.
 */
#define WRITE_ELEMENT_(index_, item_) \
  do { \
    offset = (index_) * sizeof(sFaraabinFobjectDataBus_CaptureValue); \
    for(uint32_t i=0; i<sizeof(sFaraabinFobjectDataBus_CaptureValue); i++) { \
      uint8_t *tmp = (uint8_t*)me->_pBufferCapture;\
      tmp[offset + i] = (item_)[i]; \
    } \
  }while(0)

/**
 * @brief Reads an item from capture buffer.
 * 
 * @param index_ Index of the item in capture buffer.
 * @param item_ Placeholder of the item to copy the read value.
 */
#define READ_ELEMENT_(index_, item_) \
  do { \
    offset = (index_) * sizeof(sFaraabinFobjectDataBus_CaptureValue); \
    for(uint32_t i=0; i<sizeof(sFaraabinFobjectDataBus_CaptureValue); i++) { \
      uint8_t *tmp = (uint8_t*)me->_pBufferCapture;\
      item_[i] = tmp[offset + i]; \
    } \
  }while(0)

/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void fQueueClear(sFaraabinFobjectDataBus * const me);
static void fQueueInsert(sFaraabinFobjectDataBus * const me, uint8_t *item);
static uint8_t fQueueRead(sFaraabinFobjectDataBus * const me, uint32_t num, uint8_t *item);

static void fRunCapture(sFaraabinFobjectDataBus *me);
static void fDetectChannelTrig(sFaraabinFobjectDataBus *me);

static void fFreeAllocatedMemory(sFaraabinFobjectDataBus * const me);

/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
              ##### faraabin_fobject_databus.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes databus fobject.
 * 
 * @param me Pointer to the databus fobject.
 * @return result Could be one of values in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_Init(sFaraabinFobjectDataBus *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}
  
  if(fFaraabinDatabase_AddDict((uint32_t)me) != 0U) {
    return 1;
  }
	
	me->_init = false;
	me->_type = (uint8_t)eFO_TYPE_DATABUS;
  
  if(me->ChannelQty == 0U) {
    return 1;
  }
  
	if(me->_isBufferChannelsStatic) {
		
		if(me->_pBufferChannels == NULL) {
			return 1;
		}
	} else {
		
		me->_pBufferChannels = (sFaraabinFobjectDataBus_Channel*)malloc(me->ChannelQty * sizeof(sFaraabinFobjectDataBus_Channel));
    if(me->_pBufferChannels == NULL) {
      return 1;
    }
	}
  
  for(uint16_t i = 0; i < me->ChannelQty; i++) {
    
    me->_pBufferChannels[i].ItemFobjectType = 0U;
    me->_pBufferChannels[i].ItemFobjectPtr = 0U;
    me->_pBufferChannels[i].ItemFobjectParam = 0U;
    me->_pBufferChannels[i].VariableDataType = 0U;
    me->_pBufferChannels[i].PrimitiveVariableId = 0U;
    me->_pBufferChannels[i].Enable = false;
    
  }
  
	if(me->_isBufferCaptureStatic) {
		
		if(me->_pBufferCapture == NULL) {
			return 1;
		}
	} else {
		
		if(me->BufferCaptureSize > 0U) {
    
			me->_pBufferCapture = (sFaraabinFobjectDataBus_CaptureValue*)malloc(sizeof(sFaraabinFobjectDataBus_CaptureValue) * me->BufferCaptureSize);
      if(me->_pBufferCapture == NULL) {

        fFreeAllocatedMemory(me);

        return 1;
      }
		}
	}
	
	// Initialize offline queue
  me->QueueItemCount = 0U;
  me->_queueFrontIndex = 0U;
  me->_queueRearIndex = 0U;
  
  me->CurrentState = eDATABUS_STATE_OFF;
  
  me->ApiTrigEnable = true;
  me->LastTrigSource = eDATABUS_TRIG_SOURCE_API;
  
  me->AttachedItemsQty = 0U;
  me->AvailableItemsQty = 0U;
  me->CaptureSendingQty = 0U;

  fChrono_Start(&me->_chronoCycle);
  
  me->Enable = TRUE;
  
  me->_init = true;
  return 0;
}

/**
 * @brief Deinitializes databus fobject and frees all allocated memory.
 * 
 * @param me Pointer to the databus fobject.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AdvFeat_DeInit(sFaraabinFobjectDataBus *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}

  fFreeAllocatedMemory(me);

  me->_init = false;

  return FARAABIN_DB_OK;
}

/**
 * @brief Runs databus state machine.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_Run(sFaraabinFobjectDataBus *me)  {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return;
	}

  if(!me->_init) {
    return;
  }
	
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
  
  me->CycleUs = fChrono_IntervalUs(&(me->_chronoCycle));

  if(!me->Enable) {
    return;
  }
  
  if(me->AvailableItemsQty == 0U) {
    return;
  }
  
	switch(me->CurrentState) {
    
    case eDATABUS_STATE_OFF: {
      break;
    }
    
    case eDATABUS_STATE_STREAM: {
			
      RUN_EVERY_QTY_OBJ_(me->StreamDivideBy, me->_streamDivbyCnt) {
        
        fFaraabinLinkSerializer_DataBusSendValue((uint32_t)me, &me->Seq, 0, false);
        
        RUN_END_;
      }
      
      break;
    }
    
    case eDATABUS_STATE_TIMER: {
      
      RUN_EVERY_QTY_OBJ_(me->TimerDivideBy, me->_timerDivbyCnt) {
        
        fRunCapture(me);
        
        RUN_END_;
      }
      
      if(fChrono_IsTimeout(&(me->_chronoTrigWindow)) == true) {
        
        me->CurrentState = eDATABUS_STATE_OFF;
        Faraabin_EventSystem_End_((uint32_t)me, &me->Seq, me->Enable, eDATABUS_EVENT_INFO_CAPTURE_END);
        
      }
      
      break;
    }
        
    case eDATABUS_STATE_TRIG_WAIT: {
      
      RUN_EVERY_QTY_OBJ_(me->TrigDivideBy, me->_trigDivbyCnt) {
        
        fRunCapture(me);
        fDetectChannelTrig(me);
        
        RUN_END_;
      }
        
      if(me->_isTriggered == true) {
        
        fChrono_StartTimeoutMs(&(me->_chronoTrigWindow), me->TimeAfterTrigMs);
        
        me->CurrentState = eDATABUS_STATE_TRIG_WINDOW;
        
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
				
        Faraabin_EventSystem_ParamEnd_((uint32_t)me, &me->Seq, me->Enable, eDATABUS_EVENT_INFO_STATE_CHANGE, eventParam, 10U);
        
      }
    
      break;
    }
    
    case eDATABUS_STATE_TRIG_WINDOW: {
      
      RUN_EVERY_QTY_OBJ_(me->TrigDivideBy, me->_trigDivbyCnt) {
            
        fRunCapture(me);
        
        RUN_END_;
      }
      
      if(fChrono_IsTimeout(&(me->_chronoTrigWindow)) == true) {
        
        me->IsCaptureEnd = true;
        me->CurrentState = eDATABUS_STATE_OFF;
        Faraabin_EventSystem_End_((uint32_t)me, &me->Seq, me->Enable, eDATABUS_EVENT_INFO_CAPTURE_END);
        
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
 * @brief Sends captured data if databus is in eDATABUS_STATE_CAPTURE_SEND state.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_SendCaptureDataRun(sFaraabinFobjectDataBus *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return;
	}

  if(!me->_init) {
    return;
  }

  switch(me->CurrentState) {

    case eDATABUS_STATE_CAPTURE_SEND: {

      for(uint16_t i = 0; i < FB_DATABUS_CAPTURE_SEND_ITEMS; i++) {

        sFaraabinFobjectDataBus_CaptureValue tmp_capture;
        
        uint8_t ret = fQueueRead(me, me->CaptureSendingCnt, (uint8_t*)&tmp_capture);
        if(ret == 0U) {
          
          fFaraabinLinkSerializer_DataBusSendCaptureValue(
            (uint32_t)me,
            &me->Seq,
            me->CaptureSendingReqSeq, 
            false,
            &tmp_capture);
            
          fFaraabinLinkHandler_FlushBuffer();
          
        } else {
          
          Faraabin_EventSystem_EndResponse_((uint32_t)me, &me->Seq, me->Enable, eDATABUS_EVENT_ERROR_CAPTURE_QUEUE, me->CaptureSendingReqSeq);
          break;
        }
        
        me->CaptureSendingCnt++;
        if(me->CaptureSendingCnt >= me->CaptureSendingQty) {
          
          me->CurrentState = eDATABUS_STATE_OFF;
          Faraabin_EventSystem_ParamEndResponse_((uint32_t)me, &me->Seq, me->Enable, eDATABUS_EVENT_INFO_STATE_CHANGE, (uint8_t*)&me->CurrentState, 1, me->CaptureSendingReqSeq);

          break;
        }
      }

      break;
    }

    default: {

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
    return 0;
  }

  uint16_t cnt = 0;
  
  for(uint16_t i = 0; i<me->ChannelQty; i++) {
    
    if(me->_pBufferChannels[i].ItemFobjectPtr != 0U) {
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
    return;
  }
  
  if(!me->ChTrigEnable) {
    return;
  }
  
  if(me->ChTrigNo >= me->ChannelQty) {
    return;
  }
  
  if(!me->_pBufferChannels[me->ChTrigNo].Enable) {
    return;
  }
  
  uint8_t *ptr = (uint8_t*)me->_pBufferChannels[me->ChTrigNo].ItemFobjectPtr;

  if(ptr == NULL) {
    return;
  }
  
  uByte8 tmp = {0x00};

  switch((eFaraabinFobjectVarType_PrimitiveId)me->_pBufferChannels[me->ChTrigNo].PrimitiveVariableId) {
    
    case eVAR_DATA_TYPE_PRIMITIVE_BOOL: {
      
      me->_trigChannelPayload.Byte[0] = ptr[0];
      
      break;
    }
    
    case eVAR_DATA_TYPE_PRIMITIVE_UINT8: {
      
      me->_trigChannelPayload.Byte[0] = ptr[0];
      
      break;
    }
    
    case eVAR_DATA_TYPE_PRIMITIVE_INT8: {
      
      me->_trigChannelPayload.Byte[0] = ptr[0];
      
      break;
    }
    
    case eVAR_DATA_TYPE_PRIMITIVE_UINT16: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      me->_trigChannelPayload.U16[0] = tmp.U16[0];
      
      break;
    }
    
    case eVAR_DATA_TYPE_PRIMITIVE_INT16: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      me->_trigChannelPayload.I16[0] = tmp.I16[0];
      
      break;
    }
    
    case eVAR_DATA_TYPE_PRIMITIVE_UINT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      me->_trigChannelPayload.U32[0] = tmp.U32[0];
      
      break;
    }
    
    case eVAR_DATA_TYPE_PRIMITIVE_INT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      me->_trigChannelPayload.I32[0] = tmp.I32[0];
      
      break;
    }
    
    case eVAR_DATA_TYPE_PRIMITIVE_UINT64: {
      
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_INT64: {
      
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_FLOAT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      me->_trigChannelPayload.F32[0] = tmp.F32[0];
      
      break;
    }
    
    case eVAR_DATA_TYPE_PRIMITIVE_FLOAT64: {
      
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
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return;
	}

  if(!me->_init) {
    return;
  }
  
  me->CurrentState = eDATABUS_STATE_STREAM;
  
}

/**
 * @brief Starts the databus in timer mode.
 * 
 * @param me Pointer to the databus fobject.
 * @param timerPrescaler Timer mode prescaler. Starts from 1.
 * @param timerWindow_ms Time window for capturing data in timer mode.
 */
void fFaraabinFobjectDataBus_StartTimer(sFaraabinFobjectDataBus *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return;
	}

  if(!me->_init) {
    return;
  }

  fQueueClear(me);

	fChrono_StartTimeoutMs(&me->_chronoTrigWindow, me->TimerWindowMs);
	
  me->CurrentState = eDATABUS_STATE_TIMER;
}

/**
 * @brief Starts the databus in trigger mode.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_StartTrigger(sFaraabinFobjectDataBus *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return;
	}

  if(!me->_init) {
    return;
  }

  me->_isTriggered = false;
  me->IsCaptureEnd = false;
  fFaraabinFobjectDataBus_ResetTrigger(me);

  fQueueClear(me);
  me->CurrentState = eDATABUS_STATE_TRIG_WAIT;

}

/**
 * @brief Forces a trig in the user application.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_ForceTrigger(sFaraabinFobjectDataBus *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return;
	}

  if(!me->_init) {
    return;
  }
  
  if((me->CurrentState == eDATABUS_STATE_TRIG_WAIT) && (me->ApiTrigEnable == true)) {
    
    me->_trigTimeStamp = fChrono_GetTick();
    me->LastTrigSource = eDATABUS_TRIG_SOURCE_API;
    me->_isTriggered = true;
    
  }
}

/**
 * @brief Forces a trig manually in the user interface.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_ForceManualTrigger(sFaraabinFobjectDataBus *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return;
	}

  if(!me->_init) {
    return;
  }
  
  if(me->CurrentState == eDATABUS_STATE_TRIG_WAIT) {
    
    me->_trigTimeStamp = fChrono_GetTick();
    me->LastTrigSource = eDATABUS_TRIG_SOURCE_MANUAL;
    me->_isTriggered = true;
    
  }
}

/**
 * @brief Puts databus in stop state.
 * 
 * @param me Pointer to the databus fobject.
 */
void fFaraabinFobjectDataBus_Stop(sFaraabinFobjectDataBus *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return;
	}

  if(!me->_init) {
    return;
  }

  me->CurrentState = eDATABUS_STATE_OFF;

}

/**
 * @brief Checks whether capture is end or not.
 * 
 * @param me Pointer to the databus fobject.
 * @return IsCaptureEnd Capture status.
 */
bool fFaraabinFobjectDataBus_IsCaptureEnd(sFaraabinFobjectDataBus *me) {

  if(!me->_init) {
    return true;
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
    return 0U;
  }
  
  return me->QueueItemCount;
}

/**
 * @brief Gets captured data.
 * 
 * @param me Pointer to the databus fobject.
 * @param index Index of the item in databus capture buffer.
 * @param value Pointer to the captured value.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_GetCaptureData(sFaraabinFobjectDataBus *me, uint32_t index, sFaraabinFobjectDataBus_CaptureValue *value) {

  if(!me->_init) {
    return FARAABIN_DB_NOT_INIT;
  }
  
  if(index >= me->QueueItemCount) {
    return FARAABIN_DB_CHANNEL_INDEX_GREATER_THAN_MAX;
  }
  
  return fQueueRead(me, index, (uint8_t*)&value);
}

/**
 * @brief Attaches a variable to a free channel in databus.
 * 
 * @param me Pointer to the databus fobject.
 * @param varPtr Pointer to the variable for attaching.
 * @param varSize Size of the variable.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AttachVariable(sFaraabinFobjectDataBus *me, uint32_t varPtr, uint16_t varSize, uint8_t varTypeArchitecture, uint8_t varPrimitiveId, uint8_t fobjectType) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}

  if(!me->_init) {
    return FARAABIN_DB_NOT_INIT;
  }

  for(uint16_t i = 0U; i < me->ChannelQty; i++) {

    if(me->_pBufferChannels[i].ItemFobjectPtr == 0U) {

      return fFaraabinFobjectDataBus_AttachVariableToChannel(me, i, varPtr, varSize, varTypeArchitecture, varPrimitiveId, fobjectType);
    }
  }

  return FARAABIN_DB_IS_FULL;
}

/**
 * @brief Attaches a variable to a specific channel of the databus.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number of the databus.
 * @param varPtr Pointer to the variable.
 * @param varSize Size of the variable.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AttachVariableToChannel(
  sFaraabinFobjectDataBus *me,
  uint16_t channel,
  uint32_t varPtr,
  uint16_t varSize,
  uint8_t varTypeArchitecture,
  uint8_t varPrimitiveId,
  uint8_t fobjectType) {
		
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}

  if(!me->_init) {
    return FARAABIN_DB_NOT_INIT;
  }

  if(channel >= me->ChannelQty) {
    return FARAABIN_DB_CHANNEL_INDEX_GREATER_THAN_MAX;
  }

  if(varPtr == 0U) {
    return FARAABIN_DB_ACTION_WITH_NULL_REFERENCE;
  }

  me->_pBufferChannels[channel].ItemFobjectType = fobjectType;
  me->_pBufferChannels[channel].ItemFobjectPtr = varPtr;
  me->_pBufferChannels[channel].ItemFobjectParam = varSize;
  me->_pBufferChannels[channel].VariableDataType = varTypeArchitecture;
  me->_pBufferChannels[channel].PrimitiveVariableId = varPrimitiveId;
  me->_pBufferChannels[channel].Enable = true;

  me->AttachedItemsQty++;
  me->AvailableItemsQty++;

  return FARAABIN_DB_OK;
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
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AttachEntityNumericalToChannel(sFaraabinFobjectDataBus *me, uint16_t channel, uint32_t varPtr, uint16_t varSize, uint8_t varTypeArchitecture, uint8_t varPrimitiveId) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}

  if(!me->_init) {
    return FARAABIN_DB_NOT_INIT;
  }

  if(channel >= me->ChannelQty) {
    return FARAABIN_DB_CHANNEL_INDEX_GREATER_THAN_MAX;
  }
  
  if(varPtr == 0U) {
    return FARAABIN_DB_ACTION_WITH_NULL_REFERENCE;
  }

  me->_pBufferChannels[channel].ItemFobjectType = (uint8_t)eFO_TYPE_ENTITY_NUMERICAL;
  me->_pBufferChannels[channel].ItemFobjectPtr = varPtr;
  me->_pBufferChannels[channel].ItemFobjectParam = varSize;
  me->_pBufferChannels[channel].VariableDataType = varTypeArchitecture;
  me->_pBufferChannels[channel].PrimitiveVariableId = varPrimitiveId;
  me->_pBufferChannels[channel].Enable = true;

  me->AttachedItemsQty++;
  me->AvailableItemsQty++;

  return FARAABIN_DB_OK;
}

/**
 * @brief Attaches a code block fobject to a free channel of the databus so the code block can sends its data and events through it.
 * 
 * @param me Pointer to the databus fobject.
 * @param cbPtr Pointer to the code block.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AttachCodeBlock(sFaraabinFobjectDataBus *me, uint32_t cbPtr) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}

  if(!me->_init) {
    return FARAABIN_DB_NOT_INIT;
  }

  for(uint16_t i = 0U; i < me->ChannelQty; i++) {

    if(me->_pBufferChannels[i].ItemFobjectPtr == 0U) {

      return fFaraabinFobjectDataBus_AttachCodeBlockToChannel(me, i, cbPtr);
    }
  }

  return FARAABIN_DB_IS_FULL;
}

/**
 * @brief Attaches a code block fobject to a specific channel of the databus so the code block can sends its data and events through it.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number.
 * @param cbPtr Pointer to the code block.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_AttachCodeBlockToChannel(sFaraabinFobjectDataBus *me, uint16_t channel, uint32_t cbPtr) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  if(!me->_init) {
    return FARAABIN_DB_NOT_INIT;
  }
  
  if(channel >= me->ChannelQty) {
    return FARAABIN_DB_CHANNEL_INDEX_GREATER_THAN_MAX;
  }
  
  if(cbPtr == 0U) {
    return FARAABIN_DB_ACTION_WITH_NULL_REFERENCE;
  }

  sCpuProcess *cb = (sCpuProcess*)cbPtr;
  if(cb->DataBusPtr != 0U) {
            
    return FARAABIN_DB_CODEBLOCK_CALLBACK_NOT_EMPTY;
  }

  cb->DataBusChannel = channel;
  cb->DataBusPtr = (uint32_t)me;

  me->_pBufferChannels[channel].ItemFobjectPtr = cbPtr;
  me->_pBufferChannels[channel].ItemFobjectType = (uint8_t)eFO_TYPE_CODE_BLOCK;
  me->_pBufferChannels[channel].ItemFobjectParam = 0U;
  me->_pBufferChannels[channel].Enable = true;

  me->AttachedItemsQty++;
  me->AvailableItemsQty++;

  return FARAABIN_DB_OK;
#else
	return FARAABIN_DB_NOT_INIT;
#endif
}

/**
 * @brief Detaches an item from specific channel of the databus.
 * 
 * @param me Pointer to the databus fobject.
 * @param channel Channel number.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_DetachFromChannel(sFaraabinFobjectDataBus *me, uint16_t channel) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}

  if(!me->_init) {
    return FARAABIN_DB_NOT_INIT;
  }

  if(channel >= me->ChannelQty) {
    return FARAABIN_DB_CHANNEL_INDEX_GREATER_THAN_MAX;
  }

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
  if(me->_pBufferChannels[channel].ItemFobjectType == (uint8_t)eFO_TYPE_CODE_BLOCK) {
    
    sCpuProcess *cb = (sCpuProcess*)(me->_pBufferChannels[channel].ItemFobjectPtr);
    cb->DataBusPtr = 0U;

  }
#endif

  me->_pBufferChannels[channel].ItemFobjectParam = 0U;
  me->_pBufferChannels[channel].ItemFobjectPtr = 0U;
  me->_pBufferChannels[channel].ItemFobjectType = 0U;
  me->_pBufferChannels[channel].Enable = false;

  me->AttachedItemsQty--;
  me->AvailableItemsQty--;

  return FARAABIN_DB_OK;
}

/**
 * @brief Detaches all attached items in a databus.
 * 
 * @param me Pointer to the databus fobject.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
uint8_t fFaraabinFobjectDataBus_DetachAllChannels(sFaraabinFobjectDataBus *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}

  if(!me->_init) {
    return FARAABIN_DB_NOT_INIT;
  }

  uint8_t result = FARAABIN_DB_OK;

  for(uint16_t i = 0; i < me->ChannelQty; i++) {

    result = fFaraabinFobjectDataBus_DetachFromChannel(me, i);

    if(result != FARAABIN_DB_OK) {
      return result;
    }
  }

  me->AttachedItemsQty = 0U;
  me->AvailableItemsQty = 0U;

  return FARAABIN_DB_OK;
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
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return;
	}

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
	
  sCpuProcess *cb = (sCpuProcess*)cbPtr;
  sFaraabinFobjectDataBus *db = (sFaraabinFobjectDataBus*)cb->DataBusPtr;

  if(db->Enable == false) {
    return;
  }
  
  if(!db->_pBufferChannels[cb->DataBusChannel].Enable) {
    return;
  }
  
  switch(db->CurrentState) {
    
    case eDATABUS_STATE_STREAM: {

      if(cb->IsSendingEventsToDbStreamEnabled) {

        uint32_t tmp[2];
        tmp[0] = startTick;
        tmp[1] = endTick;
        
        Faraabin_EventSystem_ParamEnd_(cbPtr, &cb->_seq, cb->_enable, eCB_EVENT_INFO_RUN, (uint8_t*)&tmp, 8);

      }
      
      break;
    }
    
    case eDATABUS_STATE_TIMER:
    case eDATABUS_STATE_TRIG_WAIT:
    case eDATABUS_STATE_TRIG_WINDOW: {

      if(cb->ISSendingEventsToDbCaptureEnabled) {

        FARAABIN_CRITICIAL_ENTER_;
        
        sFaraabinFobjectDataBus_CaptureValue tmp_capture;
            
        tmp_capture.FobjectType = (uint8_t)eFO_TYPE_CODE_BLOCK;
        tmp_capture.FobjectPtr = cbPtr;
        tmp_capture.CapturedTimeStamp = fChrono_GetTick();
        uByte8 tmp;
        tmp.U32[0] = startTick;
        tmp.U32[1] = endTick;
        tmp_capture.CapturedValue = tmp.U64;
        
        fQueueInsert(db, (uint8_t*)&(tmp_capture));
        
        FARAABIN_CRITICIAL_EXIT_;

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

/*
===============================================================================
              ##### faraabin_fobject_databus.c Private Functions #####
===============================================================================*/
/**
 * @brief Detects if the channel has been trigged.
 * 
 * @param me Pointer to the databus fobject.
 */
static void fDetectChannelTrig(sFaraabinFobjectDataBus *me) {
  
  if(!me->ChTrigEnable) {
    return;
  }
  
  if(me->ChTrigNo >= me->ChannelQty) {
    return;
  }
  
  if(!me->_pBufferChannels[me->ChTrigNo].Enable) {
    return;
  }
  
  uint8_t *ptr = (uint8_t*)me->_pBufferChannels[me->ChTrigNo].ItemFobjectPtr;
  uByte8 tmp = {0x00};
  
  switch((eFaraabinFobjectVarType_PrimitiveId)me->_pBufferChannels[me->ChTrigNo].PrimitiveVariableId) {
    
    case eVAR_DATA_TYPE_PRIMITIVE_BOOL: {
      
      bool value = (bool)ptr[0];
      
      switch(me->ChTrigType) {
        
        case eDATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.Byte[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value != me->_trigChannelPayload.Byte[0]) && (me->_trigChannelPayload.Byte[0] == false)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value != me->_trigChannelPayload.Byte[0]) && (me->_trigChannelPayload.Byte[0] == true)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_UINT8: {
      
      uint8_t value = (uint8_t)ptr[0];
      
      switch(me->ChTrigType) {
        
        case eDATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.Byte[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.Byte[0]) && (me->_trigChannelPayload.Byte[0] <= me->ChTrigThreshold.Byte[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.Byte[0]) && (me->_trigChannelPayload.Byte[0] >= me->ChTrigThreshold.Byte[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_INT8: {
      
      int8_t value = (int8_t)ptr[0];
      
      switch(me->ChTrigType) {
        
        case eDATABUS_CH_TRIG_CHANGE: {
          
          if(value != (int8_t)me->_trigChannelPayload.Byte[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value > (int8_t)me->ChTrigThreshold.Byte[0]) && ((int8_t)me->_trigChannelPayload.Byte[0] <= (int8_t)me->ChTrigThreshold.Byte[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value < (int8_t)me->ChTrigThreshold.Byte[0]) && ((int8_t)me->_trigChannelPayload.Byte[0] >= (int8_t)me->ChTrigThreshold.Byte[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_UINT16: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      uint16_t value = tmp.U16[0];
      
      switch(me->ChTrigType) {
        
        case eDATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.U16[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.U16[0]) && (me->_trigChannelPayload.U16[0] <= me->ChTrigThreshold.U16[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.U16[0]) && (me->_trigChannelPayload.U16[0] >= me->ChTrigThreshold.U16[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_INT16: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      int16_t value = tmp.I16[0];
      
      switch(me->ChTrigType) {
        
        case eDATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.I16[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.I16[0]) && (me->_trigChannelPayload.I16[0] <= me->ChTrigThreshold.I16[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.I16[0]) && (me->_trigChannelPayload.I16[0] >= me->ChTrigThreshold.I16[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_UINT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      uint32_t value = tmp.U32[0];
      
      switch(me->ChTrigType) {
        
        case eDATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.U32[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.U32[0]) && (me->_trigChannelPayload.U32[0] <= me->ChTrigThreshold.U32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.U32[0]) && (me->_trigChannelPayload.U32[0] >= me->ChTrigThreshold.U32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_INT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      int32_t value = tmp.I32[0];
      
      switch(me->ChTrigType) {
        
        case eDATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.I32[0]) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.I32[0]) && (me->_trigChannelPayload.I32[0] <= me->ChTrigThreshold.I32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.I32[0]) && (me->_trigChannelPayload.I32[0] >= me->ChTrigThreshold.I32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_UINT64: {
      
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
        
        case eDATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.U64) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.U64) && (me->_trigChannelPayload.U64 <= me->ChTrigThreshold.U64)) {
            
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.U64) && (me->_trigChannelPayload.U64 >= me->ChTrigThreshold.U64)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_INT64: {
      
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
        
        case eDATABUS_CH_TRIG_CHANGE: {
          
          if(value != me->_trigChannelPayload.I64) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.I64) && (me->_trigChannelPayload.I64 <= me->ChTrigThreshold.I64)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.I64) && (me->_trigChannelPayload.I64 >= me->ChTrigThreshold.I64)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_FLOAT32: {
      
      tmp.Byte[0] = ptr[0];
      tmp.Byte[1] = ptr[1];
      tmp.Byte[2] = ptr[2];
      tmp.Byte[3] = ptr[3];
      float32_t value = tmp.F32[0];
      
      switch(me->ChTrigType) {
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.F32[0]) && (me->_trigChannelPayload.F32[0] <= me->ChTrigThreshold.F32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.F32[0]) && (me->_trigChannelPayload.F32[0] >= me->ChTrigThreshold.F32[0])) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
    
    case eVAR_DATA_TYPE_PRIMITIVE_FLOAT64: {
      
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
        
        case eDATABUS_CH_TRIG_RISING: {
          
          if((value > me->ChTrigThreshold.F64) && (me->_trigChannelPayload.F64 <= me->ChTrigThreshold.F64)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
            me->_isTriggered = true;
            
          }
          
          break;
        }
        
        case eDATABUS_CH_TRIG_FALLING: {
          
          if((value < me->ChTrigThreshold.F64) && (me->_trigChannelPayload.F64 >= me->ChTrigThreshold.F64)) {
            
            me->_trigTimeStamp = fChrono_GetTick();
            me->LastTrigSource = eDATABUS_TRIG_SOURCE_CHANNEL;
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
  
  FARAABIN_CRITICIAL_ENTER_;
  
  // Send value
  for(uint16_t i = 0U; i < me->ChannelQty; i++) {

    if(me->_pBufferChannels[i].ItemFobjectPtr == 0U) {
      continue;
    }

    if(!me->_pBufferChannels[i].Enable) {
      continue;
    }
      
    switch(me->_pBufferChannels[i].ItemFobjectType) {
      
      case eFO_TYPE_VAR:
      case eFO_TYPE_ENTITY_NUMERICAL: {
        
        sFaraabinFobjectDataBus_CaptureValue tmp_capture;
        
        if(me->_pBufferChannels[i].PrimitiveVariableId != 0U) {
        
          tmp_capture.FobjectType = me->_pBufferChannels[i].ItemFobjectType;
          tmp_capture.FobjectPtr = me->_pBufferChannels[i].ItemFobjectPtr;
          tmp_capture.CapturedTimeStamp = fChrono_GetTick();
          
          uint8_t *src = (uint8_t*)me->_pBufferChannels[i].ItemFobjectPtr;
          uint8_t *dst = (uint8_t*)&(tmp_capture.CapturedValue);
          for(uint16_t j = 0; j < me->_pBufferChannels[i].ItemFobjectParam; j++) {
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
  
  FARAABIN_CRITICIAL_EXIT_;
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
  
  WRITE_ELEMENT_(me->_queueRearIndex, item);
  INCREMENT_REAR_INDEX_();
  
  if(over_write == 1U) {
    INCREMENT_FRONT_INDEX_();
  } else {
    INCREMENT_COUNT_();
  }
}

/**
 * @brief Reads items from the databus queue.
 * 
 * @param me Pointer to the databus fobject.
 * @param num Number of items to read.
 * @param item Pointer to a buffer for copying the read item.
 * @return result Could be one of items in FARAABIN_DB_RET group.
 */
static uint8_t fQueueRead(sFaraabinFobjectDataBus * const me, uint32_t num, uint8_t *item) {
  
  uint32_t offset;
  uint32_t index;
  
  if(me->QueueItemCount == 0U) {
    return FARAABIN_DB_QUEUE_EMPTY;
  }
  
  if(num >= me->QueueItemCount) {
    return FARAABIN_DB_CHANNEL_INDEX_GREATER_THAN_MAX;
  }
  
  index = (num + me->_queueFrontIndex) % me->BufferCaptureSize;
  
  READ_ELEMENT_(index, item);
  
  return FARAABIN_DB_OK;
}

/**
 * @brief Frees dynamically allocated memories of the databus.
 * 
 * @param me Pointer to the databus fobject.
 */
static void fFreeAllocatedMemory(sFaraabinFobjectDataBus * const me) {

  if(!me->_isBufferChannelsStatic) {
    free(me->_pBufferChannels);
  }

  if(!me->_isBufferCaptureStatic) {
    free(me->_pBufferCapture);
  }
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
