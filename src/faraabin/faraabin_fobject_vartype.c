/**
 ******************************************************************************
 * @file           : faraabin_fobject_vartype.c
 * @brief          : Variable type fobject.
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

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_vartype.h"

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
 * @return InitStat Returns zero on success, 1 on failure.
 */
uint8_t fFaraabinFobjectVarType_Init(sFaraabinFobjectVarType *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}
  
  if(fFaraabinDatabase_AddDict((uint32_t)me) != 0U) {
    return 1;
  }
  
	me->_init = false;
  me->_type = (uint8_t)eFO_TYPE_VAR_TYPE;
	
  me->Enable = true;
  me->_init = true;
  return 0;
}

/*
===============================================================================
            ##### faraabin_fobject_var_type.c Private Functions #####
===============================================================================*/

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
