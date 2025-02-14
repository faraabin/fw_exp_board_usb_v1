/**
 ******************************************************************************
 * @file           : faraabin_fobject_container.c
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

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_container.h"

#include "faraabin_fobject.h"
#include "faraabin_database.h"
#include "faraabin_link_serializer.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
            ##### faraabin_fobject_container.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes a container fobject.
 * 
 * @param me Pointer to the Container.
 * @return InitStat Returns '1' if failed, '0' if successful.
 */
uint8_t fFaraabinFobjectContainer_Init(sFaraabinFobjectContainer *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}
  
  if(fFaraabinDatabase_AddDict((uint32_t)me) != 0U) {
    return 1;
  }
	
	me->_init = false;
  me->_type = (uint8_t)eFO_TYPE_CONTAINER;
  
  me->Enable = true;
	me->_init = true;
	
  if(fFaraabinLinkSerializer_CountDictsFrames((uint32_t)me) == 0U) {
    return 0;
  }
  
  return 0;
}

/*
===============================================================================
            ##### faraabin_fobject_container.c Private Functions #####
===============================================================================*/


/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
