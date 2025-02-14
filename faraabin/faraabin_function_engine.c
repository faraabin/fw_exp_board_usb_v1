/**
 ******************************************************************************
 * @file           : faraabin_function_engine.c
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
 * @endverbatim
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin_function_engine.h"

#include "faraabin_config.h"
#include "faraabin_fobject.h"
#include "faraabin_fobject_system_event_wrapper.h"

#include <stdlib.h>

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/**
 * @brief Function engine item object.
 * 
 */
typedef struct {

  sFaraabinFobjectFunction* Function;           /*!< Pointer to the function fobject. */
  
  uint8_t Param[FB_FUNCTION_PARAM_BUFFER_SIZE]; /*!< Function arguments buffer. */
  
  bool IsFirstRun;                              /*!< Is first run flag. */
  
  sChrono _chrono;                              /*!< Internal chrono for time measurement puroposes. */
  
  uint8_t _result;                              /*!< Function execution result. */
  
  void(*fpUserTerminalCallback)(uint8_t *userData, uint16_t userDataSize);  /*!< Function pointer of the user terminal callback. */
  
  eFaraabinFobjectFunction_StatusId Status;     /*!< Function execution status ID. */
  
  uint32_t ObjectPtr;                           /*!< Pointer to the object attached to the function. */
  
  bool IsBusy;                                  /*!< Is busy falg. */
  
  bool IsRunning;                               /*!< Is function running flag. */
  
}sFaraabinFunctionEngine_Item;

/* Private variables ---------------------------------------------------------*/
static sFaraabinFunctionEngine_Item FunctionsList[FB_MAX_CONCURRENT_FUNCTION];

/* Private function prototypes -----------------------------------------------*/
static sFaraabinFunctionEngine_Item* fFindFunction(uint32_t functionPtr, uint32_t objectPtr);
static sFaraabinFunctionEngine_Item* fFindRunningFunction(uint32_t functionPtr, uint32_t objectPtr);
static uint8_t fFaraabinFn_GetFreeIndex(void);

/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
            ##### faraabin_fobject_function.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes the function engine that executes all functions and function groups in Faraabin.
 * 
 * @return InitResult Returns '0' if successful and '1' if failed.
 */
uint8_t fFaraabinFunctionEngine_Init(void) {

  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    FunctionsList[i].Function = NULL;
    if(memset(FunctionsList[i].Param, 0x00, sizeof(FunctionsList[i].Param)) == NULL) {
      return 1;
    }
    FunctionsList[i].IsFirstRun = false;
    FunctionsList[i]._result = 0U;
    FunctionsList[i].fpUserTerminalCallback = NULL;
    FunctionsList[i].Status = eFN_STATUS_STOP;
    FunctionsList[i].ObjectPtr = 0U;
    FunctionsList[i].IsBusy = false;
    FunctionsList[i].IsRunning = false;
    
    fChrono_Stop(&FunctionsList[i]._chrono);

  }
  
  return 0;
}

/**
 * @brief Runs Faraabin function engine.
 * 
 */
void fFaraabinFunctionEngine_Run(void) {
  
  // Run Function
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    sFaraabinFunctionEngine_Item *item = &(FunctionsList[i]);
    
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
				
        Faraabin_EventSystemExtended_ParamEnd_((uint32_t)item->Function, item->ObjectPtr, &(item->Function->Seq), true, eFN_EVENT_INFO_STOP, arg, sizeof(arg));
        
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
 * @return result A value form FUNCTION_ENGINE_RESULT group.
 */
uint8_t fFaraabinFunctionEngine_Start(uint32_t functionPtr, uint32_t objectPtr, uint8_t* arg, uint16_t argSize) {

  if(fFindRunningFunction(functionPtr, objectPtr) != NULL) {
    
    return FB_FENGINE_FUNCTION_ALREADY_RUNNING;
  }

  uint8_t freeIndex = fFaraabinFn_GetFreeIndex();
  if(freeIndex == 255U) {

    return FB_FENGINE_MAX_CONCURRENT_LIMIT_REACHED;
  }

  for(uint16_t i = 0; i < argSize; i++) {
    
    FunctionsList[freeIndex].Param[i] = arg[i];
  }

  FunctionsList[freeIndex].Param[argSize] = 0x00U;
  
  FunctionsList[freeIndex].ObjectPtr = objectPtr;
  FunctionsList[freeIndex].IsFirstRun = true;
  
  FunctionsList[freeIndex].Function = (sFaraabinFobjectFunction*)functionPtr;
  FunctionsList[freeIndex].IsBusy = true;
  FunctionsList[freeIndex].IsRunning = true;

  return FB_FENGINE_FUNCTION_CMD_OK;
}

/**
 * @brief Stops a function fobject from execution.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @return result A value form FUNCTION_ENGINE_RESULT group.
 */
uint8_t fFaraabinFunctionEngine_Stop(uint32_t functionPtr, uint32_t objectPtr) {

  sFaraabinFunctionEngine_Item* item = (sFaraabinFunctionEngine_Item*)fFindFunction(functionPtr, objectPtr);

  if(item == NULL) {
    return FB_FENGINE_FUNCTION_NOT_IN_LIST;
  }

  item->IsBusy = false;
  item->IsRunning = false;

  return FB_FENGINE_FUNCTION_CMD_OK;
}

/**
 * @brief Pauses function fobject from execution.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @return result A value form FUNCTION_ENGINE_RESULT group.
 */
uint8_t fFaraabinFunctionEngine_Pause(uint32_t functionPtr, uint32_t objectPtr) {

  sFaraabinFunctionEngine_Item* item = (sFaraabinFunctionEngine_Item*)fFindFunction(functionPtr, objectPtr);

  if(item == NULL) {
    return FB_FENGINE_FUNCTION_NOT_IN_LIST;
  }

  item->IsBusy = true;
  item->IsRunning = false;

  return FB_FENGINE_FUNCTION_CMD_OK;
}

/**
 * @brief Resumes an already paused function.
 * 
 * @param functionPtr Pointer to the function fobject.
 * @param objectPtr Pointer to the object attached to the function group.
 * @return result A value form FUNCTION_ENGINE_RESULT group.
 */
uint8_t fFaraabinFunctionEngine_Resume(uint32_t functionPtr, uint32_t objectPtr) {

  sFaraabinFunctionEngine_Item* item = (sFaraabinFunctionEngine_Item*)fFindFunction(functionPtr, objectPtr);

  if(item == NULL) {
    return FB_FENGINE_FUNCTION_NOT_IN_LIST;
  }

  item->IsBusy = true;
  item->IsRunning = true;

  return FB_FENGINE_FUNCTION_CMD_OK;
}

/**
 * @brief Gets the number of concurrent running function fobjects.
 * 
 * @return numberOfFuncs Number of running functions.
 */
uint16_t fFaraabinFunctionEngine_GetQtyOfRunningFunctions(void) {
  
  uint16_t cnt = 0;
  
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    if(FunctionsList[i].IsRunning == true) {
      
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
 * @return result If function is in the list returns '0', otherwise returns '1'.
 */
uint8_t fFaraabinFunctionEngine_GetPtrsOfRunningFunction(uint16_t index, uint32_t *ptr, uint32_t *objectPtr) {
  
  if(index >= fFaraabinFunctionEngine_GetQtyOfRunningFunctions()) {
    return 1;
  }
  
  uint16_t cnt = 0;
  
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    if(FunctionsList[i].IsRunning == true) {
      
      if(cnt == index) {
        
        *ptr = (uint32_t)FunctionsList[i].Function;
        *objectPtr = FunctionsList[i].ObjectPtr;
        
        break;
      }
      
      cnt++;
    }
  }
  
  return 0;
}

/*
===============================================================================
                ##### faraabin_fobject_function.c Private Functions #####
===============================================================================*/

/**
 * @brief Finds a function in engine's list.
 * 
 * @param functionPtr Pointer of the function.
 * @return objectPtr Pointer of the object attached to function group.
 */
static sFaraabinFunctionEngine_Item* fFindFunction(uint32_t functionPtr, uint32_t objectPtr) {
  
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    if(((uint32_t)(FunctionsList[i].Function) == functionPtr) && ((uint32_t)(FunctionsList[i].ObjectPtr == objectPtr))) {
      return &(FunctionsList[i]);
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
static sFaraabinFunctionEngine_Item* fFindRunningFunction(uint32_t functionPtr, uint32_t objectPtr) {
  
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    if(((uint32_t)(FunctionsList[i].Function) == functionPtr) && ((uint32_t)(FunctionsList[i].ObjectPtr == objectPtr))) {
      
      if(FunctionsList[i].IsRunning) {
        return &(FunctionsList[i]);
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
static uint8_t fFaraabinFn_GetFreeIndex(void) {
  
  for(uint16_t i = 0; i < FB_MAX_CONCURRENT_FUNCTION; i++) {
    
    if(FunctionsList[i].IsBusy == false) {
      
      return i;
    }
  }
  
  return 255;
}

/**
 * @brief Returns RAM usage of the function feature in Faraabin.
 * 
 * @return usage Amount of bytes dedicated to Faraabin function feature. 
 */
uint32_t fFaraabinFunctionEngine_GetRamUsage(void) {
	
	return sizeof(sFaraabinFunctionEngine_Item) * FB_MAX_CONCURRENT_FUNCTION;
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
