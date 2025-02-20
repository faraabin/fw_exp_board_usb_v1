/**
 ******************************************************************************
 * @file           : faraabin_fobject_vartype_wrapper.h
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
#ifndef FARAABIN_FOBJECT_VARTYPE_WRAPPER_H
#define FARAABIN_FOBJECT_VARTYPE_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_vartype.h"

#include "faraabin_config.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifdef FARAABIN_ENABLE

/**
 * @brief Initializes a variable type fobject.
 * 
 * @param typeName_ Name of the variable type fobject.
 */
#define FARAABIN_VAR_TYPE_PRIMITIVE_INIT_(typeName_) \
  do{\
    __faraabin_vartype_##typeName_.Name = #typeName_;\
    __faraabin_vartype_##typeName_.Filename = FILENAME__;\
    __faraabin_vartype_##typeName_.Path = "System";\
    __faraabin_vartype_##typeName_.Size = sizeof(typeName_);\
    __faraabin_vartype_##typeName_.DataType = eVAR_DATA_TYPE_PRIMITIVE;\
    __faraabin_vartype_##typeName_.fpSendMember = NULL;\
    uint8_t ret = fFaraabinFobjectVarType_Init(&(__faraabin_vartype_##typeName_));\
    (void)ret;\
  }while(0)

/**
 * @brief Defines a primitive variable type. This macro generates the fobject globally.
 * 
 * @note User does not need to utilize this macro.
 *       All primitive types are defined in faraabin library internally.
 * 
 * @param typeName_ Name of the variable type fobject.
 */
#define FARAABIN_VAR_TYPE_PRIMITIVE_DEF_(typeName_)         sFaraabinFobjectVarType __faraabin_vartype_##typeName_

/**
 * @brief Defines a primitive variable type. This macro generates the fobject statically.
 * 
 * @note User does not need to utilize this macro.
 *       All primitive types are defined in faraabin library internally.
 * 
 * @param typeName_ Name of the variable type fobject.
 */
#define FARAABIN_VAR_TYPE_PRIMITIVE_DEF_STATIC_(typeName_)  static sFaraabinFobjectVarType __faraabin_vartype_##typeName_

/**
 * @brief This macro is used to extern a defined variable type.
 * 
 * @note User does not need to utilize this macro.
 *       All primitive types are defined in faraabin library internally.
 * 
 * @param typeName_ Name of the variable type fobject.
 */
#define FARAABIN_VAR_TYPE_PRIMITIVE_DEF_EXTERN_(typeName_)  extern sFaraabinFobjectVarType __faraabin_vartype_##typeName_

/**
 * @brief Defines a user-defined typedef globally.
 * 
 * @param typeName_ Name of the data type.
 */
#define FARAABIN_VAR_TYPE_DEF_(typeName_)  \
  sFaraabinFobjectVarType __faraabin_vartype_##typeName_;\
  void __faraabin_vartype_member_func_##typeName_(uint32_t structPtr, uint8_t reqSeq)

/**
 * @brief Defines a user-defined typedef statically.
 * 
 * @param typeName_ Name of the data type.
 */
#define FARAABIN_VAR_TYPE_DEF_STATIC_(typeName_)  \
  static sFaraabinFobjectVarType __faraabin_vartype_##typeName_;\
  void __faraabin_vartype_member_func_##typeName_(uint32_t structPtr, uint8_t reqSeq)

/**
 * @brief externs a global user-defined typedef.
 * 
 * @param typeName_ Name of the data type.
 */
#define FARAABIN_VAR_TYPE_DEF_EXTERN_(typeName_)  extern  sFaraabinFobjectVarType __faraabin_vartype_##typeName_

/**
 * @brief Initiates a structure variable type.
 * 
 * @note After defining a variable type, user needs to execute this macro once to initialize varType fobject.
 * 
 * @param typeName_ Name of the variable type.
 * @param path_ Path given to the variable type.
 */
#define FARAABIN_VAR_TYPE_STRUCT_INIT_WP_(typeName_, path_) \
  do{\
    __faraabin_vartype_##typeName_.Name = #typeName_;\
    __faraabin_vartype_##typeName_.Filename = FILENAME__;\
    __faraabin_vartype_##typeName_.Path = (char*)path_;\
    __faraabin_vartype_##typeName_.Size = sizeof(typeName_);\
    __faraabin_vartype_##typeName_.DataType = eVAR_DATA_TYPE_USER_DEFINED_STRUCT;\
    __faraabin_vartype_##typeName_.fpSendMember = __faraabin_vartype_member_func_##typeName_;\
    uint8_t ret = fFaraabinFobjectVarType_Init(&(__faraabin_vartype_##typeName_));\
    (void)ret;\
  }while(0)

/**
 * @brief This macro is the same as FARAABIN_VAR_TYPE_STRUCT_INIT_WP_().
 *        The only difference is that it does not get path from user and assigns "root" as the default path.
 * 
 * @note After defining a variable type, user needs to execute this macro once to initialize varType fobject.
 * 
 * @param typeName_ Name of the variable type.
 */
#define FARAABIN_VAR_TYPE_STRUCT_INIT_(typeName_) FARAABIN_VAR_TYPE_STRUCT_INIT_WP_(typeName_, RootPath____)

/**
 * @brief Declares the variable type dictionary function.
 * 
 * @note Variable types need to send their dictionary information in a function.
 *       Whenever faraabin wants to send these info to the application, these functions
 *       will be called.
 * 
 * @param typeName_ Name of the variable type.
 */
#define FARAABIN_VAR_TYPE_STRUCT_FUNC_(typeName_) \
  void __faraabin_vartype_member_func_##typeName_(uint32_t structPtr, uint8_t reqSeq)\

/**
 * @brief Declares a local pointer to the user-defined type.
 * 
 * @note This macro can be used as a start tag inside FARAABIN_VAR_TYPE_STRUCT_FUNC_()
 *       before defining structure members (fields) using FARAABIN_VAR_TYPE_STRUCT_MEMBER_().
 * 
 * @param typeName_ Name of the variable type.
 */
#define FARAABIN_VAR_TYPE_STRUCT_FUNC_START_(typeName_) typeName_ *__faraabin_type_ptr

/**
 * @brief This macro can be used as an end tag inside FARAABIN_VAR_TYPE_STRUCT_FUNC_()
 *        after defining all structure members (fields) using FARAABIN_VAR_TYPE_STRUCT_MEMBER_().
 * 
 * @param typeName_ Name of the variable type.
 */
#define FARAABIN_VAR_TYPE_STRUCT_FUNC_END_(typeName_)

/**
 * @brief Declares each member (field) of the structure inside variable type dictionary function.
 * 
 * @param memberType_ Type of the member inside the structure. It can be a primitive type or
 *                    a user-defined type that has been already introduced to faraabin.
 * @param memberName_ Name of the structure member.
 * @param memberArrayQty_ Number of elements if corresponding member is an array.
 */
#define FARAABIN_VAR_TYPE_STRUCT_MEMBER_(memberType_, memberName_, memberArrayQty_) \
  fFaraabinLinkSerializer_VarTypeStructMemberDict(\
    structPtr,\
    (uint32_t)&(__faraabin_vartype_##memberType_),\
    (uint32_t)&(__faraabin_type_ptr->memberName_),\
    memberArrayQty_,\
    false,\
    #memberName_,\
    reqSeq)

/**
 * @brief Initiates an enumeration variable type.
 * 
 * @note After defining a variable type, user needs to execute this macro once to initialize varType fobject.
 * 
 * @param typeName_ Name of the variable type.
 * @param path_ Path given to the variable type.
 */
#define FARAABIN_VAR_TYPE_ENUM_INIT_WP_(typeName_, path_) \
  do{\
    __faraabin_vartype_##typeName_.Name = #typeName_;\
    __faraabin_vartype_##typeName_.Filename = FILENAME__;\
    __faraabin_vartype_##typeName_.Path = (char*)path_;\
    __faraabin_vartype_##typeName_.Size = sizeof(typeName_);\
    __faraabin_vartype_##typeName_.DataType = eVAR_DATA_TYPE_USER_DEFINED_ENUM;\
    __faraabin_vartype_##typeName_.fpSendMember = __faraabin_vartype_member_func_##typeName_;\
    uint8_t ret = fFaraabinFobjectVarType_Init(&(__faraabin_vartype_##typeName_));\
    (void)ret;\
  }while(0)

/**
 * @brief This macro is the same as FARAABIN_VAR_TYPE_ENUM_INIT_WP_().
 *        The only difference is that it does not get path from user and assigns "root" as the default path.
 * 
 * @note After defining a variable type, user needs to execute this macro once to initialize varType fobject.
 * 
 * @param typeName_ Name of the variable type.
 */
#define FARAABIN_VAR_TYPE_ENUM_INIT_(typeName_) FARAABIN_VAR_TYPE_ENUM_INIT_WP_(typeName_, RootPath____)

/**
 * @brief Declares the variable type dictionary function.
 * 
 * @note Variable types need to send their dictionary information in a function.
 *       Whenever faraabin wants to send these info to the application, these functions
 *       will be called.
 * 
 * @param typeName_ Name of the variable type.
 */
#define FARAABIN_VAR_TYPE_ENUM_FUNC_(typeName_) \
  void __faraabin_vartype_member_func_##typeName_(uint32_t enumPtr, uint8_t reqSeq)\

/**
 * @brief Declares a local pointer to the user-defined type.
 * 
 * @note This macro can be used as a start tag inside FARAABIN_VAR_TYPE_ENUM_FUNC_()
 *       before defining enumeration members using FARAABIN_VAR_TYPE_ENUM_MEMBER_().
 * 
 * @param typeName_ Name of the variable type.
 */
#define FARAABIN_VAR_TYPE_ENUM_FUNC_START_(typeName_) 

/**
 * @brief This macro can be used as an end tag inside FARAABIN_VAR_TYPE_ENUM_FUNC_()
 *        after defining all enumeration members using FARAABIN_VAR_TYPE_ENUM_MEMBER_().
 * 
 * @param typeName_ Name of the variable type.
 */
#define FARAABIN_VAR_TYPE_ENUM_FUNC_END_(typeName_)

/**
 * @brief Declares each member of the enumeration inside variable type dictionary function.
 * 
 * @param memberName_ Name of the enumeration member.
 */
#define FARAABIN_VAR_TYPE_ENUM_MEMBER_(memberName_) \
  fFaraabinLinkSerializer_VarTypeEnumMemberDict(\
    enumPtr,\
    (uint16_t)memberName_,\
    (#memberName_),\
    reqSeq)

#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_VARTYPE_WRAPPER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
