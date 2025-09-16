/**
 ******************************************************************************
 * @file           : faraabin_addon_state_machine.c
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
  @verbatim
  
  This fobject requires state machine library (sold separately) and integrates
  it with Faraabin to enhance traceability and controllability over state machines
  in embedded softwares.

  @attention
  Users should not change or modify these functions.
  If users have already implemented these functions in their software, 
  those implementations should be removed to use Faraabin capabilities.
  In the state machine library, most callbacks and events can be registered
  via corresponding functions. Refer to the state machine manual for more information.

  @endverbatim
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin_addon_state_machine.h"

#include "../../faraabin.h"

#ifdef FARAABIN_ENABLE
#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
        ##### faraabin_fobject_state_machine.c Exported Functions #####
===============================================================================*/
/**
 * @brief Implementation of state machine's InitCpltedInternalCallback function
 *        for use in faraabin.
 * 
 * @param sender Sender of the event.
 * @param e Event arguments.
 */
void fStateMachine_InitCpltedInternalCallback(void *sender, void* e) {
	
  UNUSED_(e);

	sStateMachine *sm = (sStateMachine*)sender;
	sm->_type = (uint8_t)FO_TYPE_STATE_MACHINE;
	
	if(fDatabase_AddDict((uint32_t)sender) != 0) {
    return;
  }
}

/**
 * @brief Implementation of state machine's TransitionOccuredInternalCallback function
 *        for use in faraabin.
 * 
 * @param sender Sender of the event.
 * @param e Event arguments.
 */
void fStateMachine_TransitionOccuredInternalCallback(void *sender, sTransitionEventArgs *e) {
	
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
	
	sStateMachine *me = (sStateMachine*)sender;
	
	uint8_t eventParam[17];
	
	uByte4 srcStatePtr;
	srcStatePtr.U32 = (uint32_t)e->SrcState;
	eventParam[0] = srcStatePtr.Byte[0];
	eventParam[1] = srcStatePtr.Byte[1];
	eventParam[2] = srcStatePtr.Byte[2];
	eventParam[3] = srcStatePtr.Byte[3];
	
	uByte4 dstStatePtr;
	dstStatePtr.U32 = (uint32_t)e->DstState;
	eventParam[4] = dstStatePtr.Byte[0];
	eventParam[5] = dstStatePtr.Byte[1];
	eventParam[6] = dstStatePtr.Byte[2];
	eventParam[7] = dstStatePtr.Byte[3];
	
	uByte4 transitionPtr;
	transitionPtr.U32 = (uint32_t)e->TransitionPtr;
	eventParam[8] = transitionPtr.Byte[0];
	eventParam[9] = transitionPtr.Byte[1];
	eventParam[10] = transitionPtr.Byte[2];
	eventParam[11] = transitionPtr.Byte[3];
	
	uByte4 currentStatePtr;
	currentStatePtr.U32 = (uint32_t)me->_currentState;
	eventParam[12] = currentStatePtr.Byte[0];
	eventParam[13] = currentStatePtr.Byte[1];
	eventParam[14] = currentStatePtr.Byte[2];
	eventParam[15] = currentStatePtr.Byte[3];
	
	eventParam[16] = (uint8_t)e->TransitionType;
	
	Faraabin_EventSystem_ParamEnd_((uint32_t)me, &me->_seq, me->_enable, eSM_EVENT_INFO_TRANSITION_OCCURRED, eventParam, 17U);
}

/**
 * @brief Implementation of state machine's TransitionOccuredInternalCallback function
 *        for use in faraabin.
 * 
 * @param sender Sender of the event.
 * @param e Event arguments.
 */
void fStateMachine_TransitionRejectedInternalCallback(void *sender, sTransitionEventArgs *e) {
	
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
	
	sStateMachine *me = (sStateMachine*)sender;
	
	Faraabin_EventSystem_ParamEnd_((uint32_t)me, &me->_seq, me->_enable, eSM_EVENT_INFO_TRANSITION_REJECTED, (uint8_t*)&(e->TransitionPtr), 4U);
}
/**
 * @brief Implementation of state machine's ErrorOccuredInternalCallback function
 *        for use in faraabin.
 * 
 * @param sender Sender of the event.
 * @param e Event arguments.
 */
void fStateMachine_ErrorOccuredInternalCallback(void *sender, sExecutionErrorArgs *e) {

	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
	
	sStateMachine *me = (sStateMachine*)sender;
	uint8_t errorCode = (uint8_t)e->ErrorCode;
	
	fFaraabinLink_Serialize_Event((uint32_t)me, 0, &me->_seq, me->_enable, 
		FB_COMMON_PROP_ID_EVENT_LIB, FO_EVENT_SEVERITY_ERROR, 
		eSM_EVENT_INFO_TRANSITION_REJECTED, &(errorCode), 1, 0, false, NULL, NULL, true, false);
}

/**
 * @brief Implementation of state machine's StateEnteredInternalCallback function
 *        for use in faraabin.
 * 
 * @param sender Sender of the event.
 * @param e Event arguments.
 */
void fStateMachine_StateEnteredInternalCallback(void *sender, sStateEventArgs *e) {
  
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
	
	sStateMachine *me = (sStateMachine*)sender;
	uint32_t statePtr = (uint32_t)e->State;
	
	Faraabin_EventSystem_ParamEnd_((uint32_t)me, &me->_seq, me->_enable, eSM_EVENT_INFO_STATE_ENTERED, (uint8_t*)&statePtr, 4U);
  
}

/**
 * @brief Implementation of state machine's StateExitedInternalCallback function
 *        for use in faraabin.
 * 
 * @param sender Sender of the event.
 * @param e Event arguments.
 */
void fStateMachine_StateExitedInternalCallback(void *sender, sStateEventArgs *e) {
  
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
	
	sStateMachine *me = (sStateMachine*)sender;
	uint32_t statePtr = (uint32_t)e->State;
	
	Faraabin_EventSystem_ParamEnd_((uint32_t)me, &me->_seq, me->_enable, eSM_EVENT_INFO_STATE_EXITED, (uint8_t*)&statePtr, 4U);
  
}

#endif
#endif
/*
===============================================================================
          ##### faraabin_fobject_state_machine.c Private Functions #####
===============================================================================*/


/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
