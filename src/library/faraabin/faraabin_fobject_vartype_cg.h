/**
 ******************************************************************************
 * @file           : faraabin_fobject_vartype_cg.h
 * @brief          : These macroes are used along with Faraabin varType code
 *                   generate tool to facilitate utilizing varType fobjects.
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
 * This file and its macros automate the user-defined type definition in Faraabin.
 * They allow users to define the data type structure and introduce it to Faraabin simultaneously.
 * 
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifdef __cplusplus
extern "C"{
#endif

/* Includes ------------------------------------------------------------------*/
/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
#undef typedef_struct_
#undef typedef_struct_end_
#undef typedef_enum_
#undef typedef_enum_end_

#undef sm_
#undef sma_

#undef em_
#undef emv_

#if defined(TYPE_DICT)

    #define typedef_struct_(typeName_)                          FARAABIN_VAR_TYPE_DEF_(typeName_);\
                                                                static FARAABIN_VAR_TYPE_STRUCT_FUNC_(typeName_) {\
                                                                FARAABIN_VAR_TYPE_STRUCT_FUNC_START_(typeName_);
                                                                  
    #define sm_(memberTypeName_, memberName_)                   FARAABIN_VAR_TYPE_STRUCT_MEMBER_(memberTypeName_, memberName_, 1)
    #define sma_(memberTypeName_, memberName_, arraySize_)      FARAABIN_VAR_TYPE_STRUCT_MEMBER_(memberTypeName_, memberName_, arraySize_)
    #define typedef_struct_end_(typeName_)                      FARAABIN_VAR_TYPE_STRUCT_FUNC_END_(typeName_);}

    #define typedef_enum_(typeName_)                            FARAABIN_VAR_TYPE_DEF_(typeName_);\
                                                                static FARAABIN_VAR_TYPE_ENUM_FUNC_(typeName_) {\
                                                                FARAABIN_VAR_TYPE_ENUM_FUNC_START_(typeName_);
                                                                  
    #define em_(memberName_)                                    FARAABIN_VAR_TYPE_ENUM_MEMBER_(memberName_);
    #define emv_(memberName_, memberValue_)                     FARAABIN_VAR_TYPE_ENUM_MEMBER_(memberName_);
    #define typedef_enum_end_(typeName_)                        FARAABIN_VAR_TYPE_ENUM_FUNC_END_(typeName_);}
                                                                                                                                
#else

    #define typedef_struct_(typeName_)                          typedef struct typeName_
    #define sm_(memberTypeName_, memberName_)                   memberTypeName_ memberName_
    #define sma_(memberTypeName_, memberName_, arraySize_)      memberTypeName_ memberName_[arraySize_]
    #define typedef_struct_end_(typeName_)                      typeName_; \
                                                                
                                                                
    #define typedef_enum_(typeName_)                            typedef enum typeName_
    #define em_(memberName_)                                    memberName_,
    #define emv_(memberName_, memberValue_)                     memberName_ = memberValue_,
    #define typedef_enum_end_(typeName_)                        typeName_; \
                                                               
#endif
    
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/ 

/**End of Group_Name
  * @}
  */ 

#ifdef __cplusplus
} // extern "C"
#endif  /* FARAABIN_FOBJECT_VARTYPE_CG_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
