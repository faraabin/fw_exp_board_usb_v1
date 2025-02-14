/**
 ******************************************************************************
 * @file           : faraabin_fobject_function_wrapper.h
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_FOBJECT_FUNCTION_WRAPPER_H
#define FARAABIN_FOBJECT_FUNCTION_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_function.h"

#include "faraabin_config.h"
#include "add_on/runtime_scaler/runtime_scaler.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#if defined(FB_FEATURE_FLAG_MCU_CLI) && defined(FARAABIN_ENABLE)

/**
 * @brief Defines the object type for objective oriented functionality of function groups.
 * 
 * @note If user does not want this capability, fill objectType_ with 'void*'.
 * 
 * @param objectType_ Type of the object that this function group can have.
 */
#define FARAABIN_FUNCTION_GROUP_TYPE_DEF_(objectType_) typedef objectType_ __faraabin_function_group_obj_type

/**
 * @brief Declares the prototype of a function group to be used in other files.
 * 
 * @param groupName_ Name of function group.
 */
#define FARAABIN_FUNCTION_GROUP_PROTOTYPE_(groupName_) \
  extern sFaraabinFobjectFunctionGroupType groupName_

/**
 * @brief Declares the function group for adding each function members to the group.
 * 
 * @param groupName_ Name of function group.
 * @param groupHelp_ A help text that can be shown in Faraabin for helping users on PC.
 */
#define FARAABIN_FUNCTION_GROUP_(groupName_, groupHelp_) \
  static void groupName##_##_func(uint32_t fobjectPtr, uint8_t reqSeq);\
  sFaraabinFobjectFunctionGroupType groupName_ = {\
    ._type = eFO_TYPE_FUNCTION_GROUP_TYPE,\
    .Name = #groupName_,\
    .Seq = 0,\
    .fpSendMember = groupName##_##_func,\
    .Help = groupHelp_\
  };\
  static void groupName##_##_func(uint32_t fobjectPtr, uint8_t reqSeq)

/**
 * @brief Adds a function member to the function group.
 * 
 * @param groupName_ Name of function group.
 * @param functionName_ Name of function to add to the group.
 */
#define FARAABIN_FUNCTION_GROUP_ADD_(groupName_, functionName_) \
  fFaraabinLinkSerializer_FunctionGroupTypeMemberDict((uint32_t)&(groupName_), &(groupName_##_##functionName_##_obj), reqSeq)

/**
 * @brief Initializes function group.
 * 
 * @param pFunctionGroup_ Pointer to the function group fobject.
 */
#define FARAABIN_FunctionGroupType_Init_(pFunctionGroup_) \
  do{\
    (pFunctionGroup_)->Filename = FILENAME__;\
    uint8_t ret = fFaraabinFobjectFunctionGroupType_Init(pFunctionGroup_);\
    (void)ret;\
  }while(0)

/**
 * @brief Adds dictionary information of a function group with its object for object oriented use to a container.
 * 
 * @param groupName_ Name of the function group.
 * @param objectName_ Name of the object that this function group is attached to for object oreinted using.
 * @param path_ Path of the function group.
 */
#define FARAABIN_FUNCTION_GROUP_OBJECT_DICT_WP_(groupName_, objectName_, path_) \
  fFaraabinLinkSerializer_FunctionGroupDict(\
  path_,\
  FILENAME__,\
  #objectName_,\
  (uint32_t)&objectName_,\
  (uint32_t)&groupName_,\
  reqSeq)

/**
 * @brief This macro does the same as FARAABIN_FUNCTION_GROUP_OBJECT_DICT_WP_() but the path is set in container using FARAABIN_SET_PATH_().
 * 
 * @param groupName_ Name of the function group.
 * @param objectName_ Name of the object that this function group is attached to for object oreinted using.
 */
#define FARAABIN_FUNCTION_GROUP_OBJECT_DICT_(groupName_, objectName_) FARAABIN_FUNCTION_GROUP_OBJECT_DICT_WP_(groupName_, objectName_, __ContainerPath__)

/**
 * @brief Adds dictionary information of a function group to a container.
 * 
 * @param groupName_ Name of the function group.
 * @param path_ Path of the function group.
 */
#define FARAABIN_FUNCTION_GROUP_DICT_WP_(groupName_, path_) \
  fFaraabinLinkSerializer_FunctionGroupDict(\
  path_,\
  FILENAME__,\
  "NoName",\
  0,\
  (uint32_t)&groupName_,\
  reqSeq)

/**
 * @brief This macro does the same as FARAABIN_FUNCTION_GROUP_DICT_WP_() but the path is set in container using FARAABIN_SET_PATH_().
 * 
 * @param groupName_ Name of the function group.
 */
#define FARAABIN_FUNCTION_GROUP_DICT_(groupName_) FARAABIN_FUNCTION_GROUP_DICT_WP_(groupName_, __ContainerPath__)

/**
 * @brief Declares a run-to-completion function to be added to a function group.
 * 
 * @param groupName_ Name of function group that this function belongs to.
 * @param functionName_ Name of the function.
 * @param functionHelp_ A help text for this function to be shown in Faraabin to the user.
 */
#define FARAABIN_FUNCTION_(groupName_, functionName_, functionHelp_)  \
  static uint8_t groupName##_##functionName_##_func(uint32_t objectPtr, char *param, bool isFirstRun);\
  static sFaraabinFobjectFunction groupName_##_##functionName_##_obj = {\
    ._type = eFO_TYPE_FUNCTION,\
    .Name = #functionName_,\
    .Help = functionHelp_,\
    .Seq = 0,\
    .GroupTypePtr = &(groupName_),\
    .FunctionPtr = (groupName##_##functionName_##_func)\
  };\
  static uint8_t groupName##_##functionName_##_func(uint32_t objectPtr, char *__param__, bool isFirstRun) {\
    sFaraabinFobjectFunction *__funcPtr__ = &(groupName_##_##functionName_##_obj);\
    (void)(__funcPtr__);\
    __faraabin_function_group_obj_type *me = (__faraabin_function_group_obj_type*)(objectPtr);\
    (void)(me);\
    static bool isLoop = false;\

/**
 * @brief Declares a looping function to be added to a function group.
 * 
 * @param groupName_ Name of function group that this function belongs to.
 * @param functionName_ Name of the function.
 * @param functionHelp_ A help text for this function to be shown in Faraabin to the user.
 */
#define FARAABIN_FUNCTION_LOOP_(groupName_, functionName_, functionHelp_)  \
  FARAABIN_FUNCTION_(groupName_, functionName_, functionHelp_)\
    isLoop = true;\
    if(!isFirstRun) {\
      goto _function_loop;\
    }\
    
/**
 * @brief Gets parameters from PC to be used in a function.
 * 
 * @note This macro works just like scanf() for getting parameters from outside world.
 * 
 * @param expected Expected number of arguments to get from PC user.
 * @param format Format specifier of the parameters.
 */
#define FARAABIN_Function_GetParam_(expected, format, ...) \
  if((expected) != sscanf(__param__, (format), __VA_ARGS__)) { \
    FARAABIN_PrintfToFunction_("\n_GET PARAM ERROR: "); \
    FARAABIN_PrintfToFunction_("Enter %u Parameter.", (expected)); \
    return FUNCTION_ERROR_PARAM_QTY; \
  }\

/**
 * @brief Tags the end of a function.
 * 
 */
#define FARAABIN_FUNCTION_END_() \
    if(isLoop) {\
      return FUNCTION_CONTINUE;\
    } else {\
    return FUNCTION_TERMINATE_OK;\
    }\
  }

/**
 * @brief Tags the start point of the loop in a function that was declared using FARAABIN_FUNCTION_LOOP_().
 * 
 * @note Remeber that executing these functions are not timely accurate. Faraabin runs them whenever fFaraabin_Run() is executed.
 *       One iteration is executed and the next one will be deferred until next execution of fFaraabin_Run().
 * 
 * @param interval_ Loop interval in milli-seconds.
 */
#define FARAABIN_Function_LoopStartMs_(interval_) \
  _function_loop: \
  RUN_EVERY_BASE_(test, interval_, &(__funcPtr__->_chrono), Ms, FALSE) {\
  
#define FARAABIN_Function_LoopEnd_() \
    RUN_END_;\
    return FUNCTION_CONTINUE;\
  }

#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_FUNCTION_WRAPPER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
