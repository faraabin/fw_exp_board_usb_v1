/**
 ******************************************************************************
 * @file           : faraabin_addon_cpu_profiler.c
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
 * Code block is used for managing and tracing the execution of parts of the user
 * code in embedded applications. Use the wrappers in faraabin_fobject_codeblock_wrapper.h
 * file for an easy experience.
 * 
 * To use this fobject:
 * 1- Declare a code block fobject using FARAABIN_CODE_BLOCK_DEF_().
 * 2- Initialize the fobject using FARAABIN_CodeBlock_Init_WithPath_().
 * 3- Set settings of the code block like RunType, SectionQty, interval and execution thersholds, etc.
 * 4- Enable the code block using FARAABIN_CodeBlock_Enable_().
 * 5- Tag start of the code block of your application using FARAABIN_CodeBlock_Start_().
 * 6- Tag end of the code block in your application using FARAABIN_CodeBlock_End_().
 * 7- Use FARAABIN_CODE_BLOCK_SECTION_() if you want to tag a new section whithin code block.
 * 
 * @code 
  
  FARAABIN_CODE_BLOCK_DEF_(CodeBlock);

  int main(void) {

    // Initialize code block with default path
    FARAABIN_CodeBlock_Init_(&CodeBlock);

    // Config code block
    FARAABIN_CODE_BLOCK_SET_VALUE_(CodeBlock.RunType, eCB_RUN_DIVIDEBY);
    FARAABIN_CODE_BLOCK_SET_VALUE_(CodeBlock.RunDivideBy, 1);
    FARAABIN_CODE_BLOCK_SET_VALUE_(CodeBlock.RunTimeMs, 100);
    FARAABIN_CODE_BLOCK_SET_VALUE_(CodeBlock.IntervalThresholdMaxUs, 1000);
    FARAABIN_CODE_BLOCK_SET_VALUE_(CodeBlock.IntervalThresholdMinUs, 0U);
    FARAABIN_CODE_BLOCK_SET_VALUE_(CodeBlock.ExecuteThresholdMaxUs, 150);
    FARAABIN_CODE_BLOCK_SET_VALUE_(CodeBlock.ExecuteThresholdMinUs, 0U);
    FARAABIN_CODE_BLOCK_SET_VALUE_(CodeBlock.SectionQty, 2);

    FARAABIN_CodeBlock_Enable_(&CodeBlock);

    while(true) {

      FARAABIN_CodeBlock_Start_(&CodeBlock) {

        FARAABIN_CODE_BLOCK_SECTION_(section1);
        // User code for the first section in code block...

        FARAABIN_CODE_BLOCK_SECTION_(section2);
        // User code for the second section in code block...

        FARAABIN_CodeBlock_End_(&CodeBlock);
      }

      // User code that is out of scope of the CodeBlock...
    }
  }
 
 * @endcode
 * 
 * @endverbatim
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin_addon_cpu_profiler.h"

#include "../../faraabin.h"

#ifdef FARAABIN_ENABLE
#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static bool SendingEnable = false;

/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
        ##### faraabin_addon_cpu_profiler.c Exported Functions #####
===============================================================================*/
/**
 * @brief Callback for indicating initialization of a process.
 * 
 * @param sender Pointer to the process.
 * @param e Event arguments.
 */
void fCpuProfiler_ProcessInitCpltedInternalCallback(void *sender, void* e) {
	
	UNUSED_(e);

	sCpuProcess *process = (sCpuProcess*)sender;
	process->_type = (uint8_t)FO_TYPE_CODE_BLOCK;
	
	if(fDatabase_AddDict((uint32_t)sender) != 0) {
    return;
  }
}

/**
 * @brief Callback for indicating exiting from a process.
 * 
 * @param sender Pointer to the process.
 * @param e Event arguments.
 */
void fCpuProfiler_ProcessExitedInternalCallback(void *sender, sCpuProfilerProcessExitEventArgs *e) {
	
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
	
	sCpuProcess *process = (sCpuProcess*)sender;
	
	if(process->DataBusPtr != 0U) {
    
    fFaraabinFobjectDataBus_RunCodeBlock((uint32_t)process, process->_startTick, process->_endTick);
  }
}

/**
 * @brief Callback for indicating calculating new cpu usage.
 * 
 */
void fCpuProfiler_LoadCalculationInternalCallback(void) {
	
	if(!fFaraabin_IsAllowEvent()) {
		return;
	}
	
	if(SendingEnable) {
		
		fFaraabinLink_Serialize_McuSendProfiler((uint32_t)0xFFFFFFFFU);
	}
}

void fFaraabinAddOn_CpuProfiler_SendingEnable(void) {
	
	SendingEnable = true;
}

void fFaraabinAddOn_CpuProfiler_SendingDisable(void) {
	
	SendingEnable = false;
}

bool fFaraabinAddOn_CpuProfiler_IsSending(void) {
	
	return SendingEnable;
}

#endif
#endif

/*
===============================================================================
          ##### faraabin_addon_cpu_profiler.c Private Functions #####
===============================================================================*/

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
