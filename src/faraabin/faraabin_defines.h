/**
 ******************************************************************************
 * @file           : faraabin_defines.h
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
 * When a feature in faraabin_config.h is disabled, any pre-existing usage of the
 * corresponding API may cause the compiler to generate errors and warnings.
 * To prevent these issues, this file redefines all Faraabin macros, ensuring 
 * the compiler remains silent if the specific feature is disabled but has already been used.
 * 
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_DEFINES_H
#define FARAABIN_DEFINES_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_config.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#if !defined(FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP) || !defined(FARAABIN_ENABLE)
#define FARAABIN_Printf_(...)
#define FARAABIN_PrintfWarning_(...)
#define FARAABIN_PrintfError_(...)
#define FARAABIN_SendEvent_(...)
#define FARAABIN_SendEventWarning_(enumName_, eventId_)
#define FARAABIN_SendEventError_(enumName_, eventId_)
#endif

#if !defined(FARAABIN_ENABLE)
#define FARAABIN_CONTAINER_DEF_(containerName_)
#define FARAABIN_CONTAINER_DEF_STATIC_(containerName_)
#define FARAABIN_CONTAINER_DEF_EXTERN_(containerName_)
#define FARAABIN_CONTAINER_FUNC_(containerName_) \
static void __FARAABIN_CONTAINER_FUNC_##containerName_(void) {\
  (void)__FARAABIN_CONTAINER_FUNC_##containerName_;
#define FARAABIN_SET_PATH_(path_)
#define FARAABIN_CONTAINER_FUNC_END_ }
#define FARAABIN_Container_Init_WithPath(pContainer_, path_)
#define FARAABIN_Container_Init_(pContainer_)
#endif

#if !defined(FARAABIN_ENABLE)
#define FARAABIN_DATABUS_DEF_(databusName_)
#define FARAABIN_DATABUS_DEF_STATIC_(databusName_)
#define FARAABIN_DATABUS_DEF_EXTERN_(databusName_)
#define FARAABIN_DATABUS_SET_VALUE_(databusField_, value_)
#define FARAABIN_DataBus_Init_WithPath_(pDatabus_, path_)
#define FARAABIN_DataBus_Init_(pDatabus_)
#define FARAABIN_DataBus_Enable_(pDatabus_)
#define FARAABIN_DataBus_Disable_(pDatabus_)
#define FARAABIN_DataBus_StartStreamMode_(pDatabus_)
#define FARAABIN_DataBus_StartTriggerMode_(pDatabus_)
#define FARAABIN_DataBus_ForceTrigger_(pDatabus_)
#define FARAABIN_DataBus_Stop_(pDatabus_)
#define FARAABIN_DataBus_AttachVariable_BOOL_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_U8_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_I8_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_U16_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_I16_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_U32_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_I32_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_U64_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_I64_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_F32_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_F64_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_UserDefined_Enum_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachVariable_UserDefined_Struct_(pDatabus_, pVar_)
#define FARAABIN_DataBus_AttachCodeBlock_(pDatabus_, pCodeblock_)
#define FARAABIN_DataBus_DetachAllChannels_(pDatabus_)
#define FARAABIN_DataBus_Run_(pDatabus_)
#endif

#if !defined(FARAABIN_ENABLE)
#define FARAABIN_EVENT_GROUP_DEF_(egName_)
#define FARAABIN_EVENT_GROUP_DEF_STATIC_(egName_)
#define FARAABIN_EVENT_GROUP_DEF_EXTERN_(egName_)
#define FARAABIN_EVENT_GROUP_SET_VALUE_(egField_, value_)
#define FARAABIN_EventGroup_Init_WithPath_(pEg_, path_)
#define FARAABIN_EventGroup_Init_(pEg_)
#define FARAABIN_EventGroup_Enable_(pEg_)
#define FARAABIN_EventGroup_Disable_(pEg_)
#define FARAABIN_PrintfTo_(pEg_, ...)
#define FARAABIN_PrintfWarningTo_(pEg_, ...)
#define FARAABIN_PrintfErrorTo_(pEg_, ...)
#define FARAABIN_PrintfToTerminal_(pEg_, ...)
#define FARAABIN_PutsTo_(pEg_, pStr_)
#define FARAABIN_PutsWarningTo_(pEg_, pStr_)
#define FARAABIN_PutsErrorTo_(pEg_, pStr_)
#define FARAABIN_PutsToTerminal_(pEg_, pStr_)
#define FARAABIN_SendEventTo_(pEg_, enumName_, eventId_)
#define FARAABIN_SendEventWarningTo_(pEg_, enumName_, eventId_)
#define FARAABIN_SendEventErrorTo_(pEg_, enumName_, eventId_)
#endif

#if !defined(FB_FEATURE_FLAG_MCU_CLI) || !defined(FARAABIN_ENABLE)
#define FARAABIN_FUNCTION_GROUP_TYPE_DEF_(objectType_)
#define FARAABIN_FUNCTION_GROUP_PROTOTYPE_(groupName_)
#define FARAABIN_FUNCTION_GROUP_(groupName_, groupHelp_)  static void groupName##_##_func(void)
#define FARAABIN_FUNCTION_GROUP_ADD_(groupName_, functionName_)
#define FARAABIN_FunctionGroupType_Init_(pFunctionGroup_)
#define FARAABIN_FUNCTION_GROUP_OBJECT_DICT_WP_(groupName_, objectName_, path_)
#define FARAABIN_FUNCTION_GROUP_OBJECT_DICT_(groupName_, objectName_)
#define FARAABIN_FUNCTION_GROUP_DICT_WP_(groupName_, path_)
#define FARAABIN_FUNCTION_GROUP_DICT_(groupName_)
#define FARAABIN_FUNCTION_(groupName_, functionName_, functionHelp_)      static uint8_t groupName##_##functionName_##_func(void)
#define FARAABIN_FUNCTION_LOOP_(groupName_, functionName_, functionHelp_) static uint8_t groupName##_##functionName_##_func(void)
#define FARAABIN_Function_GetParam_(expected, format, ...)
#define FARAABIN_FUNCTION_END_()  return 0
#define FARAABIN_Function_LoopStartMs_(interval_)
#define FARAABIN_Function_LoopEnd_()
#endif

#if !defined(FB_FEATURE_FLAG_MCU_CLI) || !defined(FARAABIN_ENABLE)
#define FARAABIN_PrintfToFunction_(...)
#define FARAABIN_PutsToFunction_(pStr_)
#endif

#if !defined(FARAABIN_ENABLE)
#define FARAABIN_VAR_TYPE_DEF_(typeName_)
#define FARAABIN_VAR_TYPE_DEF_STATIC_(typeName_)
#define FARAABIN_VAR_TYPE_DEF_EXTERN_(typeName_)
#define FARAABIN_VAR_TYPE_SET_VALUE_(vtField_, value_)
#define FARAABIN_VAR_TYPE_STRUCT_INIT_WP_(typeName_, path_)
#define FARAABIN_VAR_TYPE_STRUCT_INIT_(typeName_)
#define FARAABIN_VAR_TYPE_STRUCT_FUNC_(typeName_) \
void __faraabin_vartype_member_func_##typeName_(void) {\
  (void)__faraabin_vartype_member_func_##typeName_;
#define FARAABIN_VAR_TYPE_STRUCT_FUNC_START_(typeName_)
#define FARAABIN_VAR_TYPE_STRUCT_FUNC_END_(typeName_) }
#define FARAABIN_VAR_TYPE_STRUCT_MEMBER_(memberType_, memberName_, memberArrayQty_)
#define FARAABIN_VAR_TYPE_ENUM_INIT_WP_(typeName_, path_)
#define FARAABIN_VAR_TYPE_ENUM_INIT_(typeName_)
#define FARAABIN_VAR_TYPE_ENUM_FUNC_(typeName_)\
void __faraabin_vartype_member_func_##typeName_(void) {\
  (void)__faraabin_vartype_member_func_##typeName_;
#define FARAABIN_VAR_TYPE_ENUM_FUNC_START_(typeName_)
#define FARAABIN_VAR_TYPE_ENUM_FUNC_END_(typeName_) }
#define FARAABIN_VAR_TYPE_ENUM_MEMBER_(memberName_)
#define FARAABIN_VAR_ENUM_DICT_WP_(varName_, type_, path_)
#define FARAABIN_VAR_ENUM_DICT_(varName_, type_)
#define FARAABIN_VAR_ENUM_DICT_CB_WP_(varName_, type_, fpAccessCb_, path_)
#define FARAABIN_VAR_ENUM_DICT_CB_(varName_, type_, fpAccessCb_)
#define FARAABIN_VAR_ENUM_ARRAY_DICT_WP_(varName_, type_, path_)
#define FARAABIN_VAR_ENUM_ARRAY_DICT_(varName_, type_)
#define FARAABIN_VAR_ENUM_ARRAY_DICT_CB_WP_(varName_, type_, fpAccessCb_, path_)
#define FARAABIN_VAR_ENUM_ARRAY_DICT_CB_(varName_, type_, fpAccessCb_)
#define FARAABIN_VAR_STRUCT_DICT_WP_(varName_, type_, path_)
#define FARAABIN_VAR_STRUCT_DICT_(varName_, type_)
#define FARAABIN_VAR_STRUCT_DICT_CB_WP_(varName_, type_, fpAccessCb_, path_)
#define FARAABIN_VAR_STRUCT_DICT_CB_(varName_, type_, fpAccessCb_)
#define FARAABIN_VAR_STRUCT_ARRAY_DICT_WP_(varName_, type_, path_)
#define FARAABIN_VAR_STRUCT_ARRAY_DICT_(varName_, type_)
#define FARAABIN_VAR_STRUCT_ARRAY_DICT_CB_WP_(varName_, type_, fpAccessCb_, path_)
#define FARAABIN_VAR_STRUCT_ARRAY_DICT_CB_(varName_, type_, fpAccessCb_)
#endif

#ifndef FARAABIN_ENABLE
#define FARAABIN_VAR_DICT_(varType_, varName_, pVar_, arrayQty_, isPtr_, fpAccessCb_, path_)
#define FARAABIN_VAR_ARRAY_DICT_(varType_, varName_, pVar_, isPtr_, fpAccessCb_, path_)
#define FARAABIN_VAR_BOOL_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_BOOL_DICT_(varName_)
#define FARAABIN_VAR_BOOL_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_BOOL_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_U8_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_U8_DICT_(varName_)
#define FARAABIN_VAR_U8_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U8_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_U16_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_U16_DICT_(varName_)
#define FARAABIN_VAR_U16_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U16_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_U32_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_U32_DICT_(varName_)
#define FARAABIN_VAR_U32_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U32_DICT_CB_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U64_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_U64_DICT_(varName_)
#define FARAABIN_VAR_U64_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U64_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_I8_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_I8_DICT_(varName_)
#define FARAABIN_VAR_I8_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I8_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_I16_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_I16_DICT_(varName_)
#define FARAABIN_VAR_I16_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I16_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_I32_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_I32_DICT_(varName_)
#define FARAABIN_VAR_I32_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I32_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_I64_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_I64_DICT_(varName_)
#define FARAABIN_VAR_I64_DICT_CB_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I64_DICT_CB_WP_(varName_, fpAccessCb_)
#define FARAABIN_VAR_F32_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_F32_DICT_(varName_)
#define FARAABIN_VAR_F32_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_F32_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_F64_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_F64_DICT_(varName_)
#define FARAABIN_VAR_F64_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_F64_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_BOOL_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_BOOL_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_BOOL_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_BOOL_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_U8_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_U8_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_U8_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U8_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_U16_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_U16_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_U16_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U16_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_U32_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_U32_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_U32_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U32_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_U64_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_U64_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_U64_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U64_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_I8_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_I8_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_I8_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I8_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_I16_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_I16_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_I16_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I16_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_I32_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_I32_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_I32_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I32_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_I64_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_I64_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_I64_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I64_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_F32_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_F32_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_F32_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_F32_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_F64_ARRAY_DICT_WP_(varName_, path_)
#define FARAABIN_VAR_F64_ARRAY_DICT_(varName_)
#define FARAABIN_VAR_F64_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_F64_ARRAY_DICT_CB_(varName_, fpAccessCb_)
#define FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(typeName_)
#define FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(typeName_)
#define FARAABIN_VAR_TYPE_PRIMITIVE_DEF_STATIC_(typeName_)
#define FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(typeName_)
#endif


/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_DEFINES_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
