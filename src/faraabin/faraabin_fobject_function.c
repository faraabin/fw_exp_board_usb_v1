/**
 ******************************************************************************
 * @file           : faraabin_fobject_function.c
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

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_function.h"

#include "faraabin_database.h"
#include "faraabin_fobject.h"

#include <stdlib.h>

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
            ##### faraabin_fobject_function.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes a function group and adds its dictionary to the database.
 * 
 * @param me Pointer to the function group.
 * @return InitStat Returns '0' if successful, '1' if failed. 
 */
uint8_t fFaraabinFobjectFunctionGroupType_Init(sFaraabinFobjectFunctionGroupType *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}
  
  if(fFaraabinDatabase_AddDict((uint32_t)me) != 0U) {
    return 1;
  }
  
	me->_type = (uint8_t)eFO_TYPE_FUNCTION_GROUP_TYPE;
	
  me->_init = true;
  return 0;
}

/*
===============================================================================
              ##### faraabin_fobject_function.c Private Functions #####
===============================================================================*/


/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
