/**
 ******************************************************************************
 * @file           : faraabin_fobject_databus_wrapper.h
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
#ifndef FARAABIN_FOBJECT_DATABUS_WRAPPER_H
#define FARAABIN_FOBJECT_DATABUS_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_databus.h"

#include "faraabin_config.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifdef FARAABIN_ENABLE

/**
 * @brief Constructs a databus fobject with default initial values.
 * 
 * @param databusName_ Name of the fobject
 */
#define FARAABIN_DATABUS_DEF_(databusName_)  \
  sFaraabinFobjectDataBus databusName_ = {\
    .Name = #databusName_,\
    .StreamDivideBy = 1,\
    .TimerDivideBy = 1,\
    .TimerWindowMs = 100,\
    .TrigDivideBy = 1,\
    .TimeAfterTrigMs = 100,\
    .ChannelQty = 10,\
    ._pBufferChannels = NULL,\
    ._isBufferChannelsStatic = false,\
    .BufferCaptureSize = 0,\
    ._pBufferCapture = NULL,\
    ._isBufferCaptureStatic = false,\
    .Enable = false,\
    ._init = false\
  }

/**
 * @brief Same as FARAABIN_DATABUS_DEF_() but declares the databus as static variable.
 * 
 * @param databusName_ Name of the fobject
 */
#define FARAABIN_DATABUS_DEF_STATIC_(databusName_)  static FARAABIN_DATABUS_DEF_(databusName_)

/**
 * @brief Externs the databus.
 * 
 * @param databusName_ Name of the fobject
 */
#define FARAABIN_DATABUS_DEF_EXTERN_(databusName_)  extern sFaraabinFobjectDataBus databusName_

/**
 * @brief Sets a field of databus to userdefined value.
 * 
 * @note This macro is used as a setter if user needs to Enable/Disable databus feature (or Faraabin features) very often.
 * 
 * @param databusField_ Field of databus to assign.
 * @param value_ New value for assigning.
 */
#define FARAABIN_DATABUS_SET_VALUE_(databusField_, value_)  databusField_ = value_

/**
 * @brief Initializes databus fobject.
 * 
 * @param pDatabus_ Pointer to databus
 * @param path_ Path of the fobject
 */
#define FARAABIN_DataBus_Init_WithPath_(pDatabus_, path_) \
  do{\
    (pDatabus_)->Path = path_;\
    (pDatabus_)->Filename = FILENAME__;\
    uint8_t ret = fFaraabinFobjectDataBus_Init(pDatabus_);\
    (void)ret;\
  }while(0)

/**
 * @brief Same as FARAABIN_DataBus_Init_WithPath_() but with default path "root".
 * 
 * @param pDatabus_ Pointer to the databus fobject.
 */
#define FARAABIN_DataBus_Init_(pDatabus_) FARAABIN_DataBus_Init_WithPath_(pDatabus_, RootPath____)

/**
 * @brief Enables the databus.
 * 
 * @param pDatabus_ Pointer to databus
 */
#define FARAABIN_DataBus_Enable_(pDatabus_) \
  do {\
    (pDatabus_)->Enable = true;\
  }while(0)

/**
 * @brief Disables the databus.
 * 
 * @param pDatabus_ Pointer to databus
 */
#define FARAABIN_DataBus_Disable_(pDatabus_) \
  do {\
    (pDatabus_)->Enable = false;\
  }while(0)

/**
 * @brief Starts databus in streaming mode.
 * 
 * @param pDatabus_ Pointer to databus
 */
#define FARAABIN_DataBus_StartStreamMode_(pDatabus_)  fFaraabinFobjectDataBus_StartStream(pDatabus_)

/**
 * @brief Starts databus in trigger mode.
 * 
 * @param pDatabus_ Pointer to databus
 */
#define FARAABIN_DataBus_StartTriggerMode_(pDatabus_)  fFaraabinFobjectDataBus_StartTrigger(pDatabus_)

/**
 * @brief Forces a trigger.
 * 
 * @param pDatabus_ Pointer to databus
 */
#define FARAABIN_DataBus_ForceTrigger_(pDatabus_)  fFaraabinFobjectDataBus_ForceTrigger(pDatabus_)

/**
 * @brief Completely stops the databus from executing.
 * 
 * @param pDatabus_ Pointer to databus
 */
#define FARAABIN_DataBus_Stop_(pDatabus_)  fFaraabinFobjectDataBus_Stop(pDatabus_)

/**
 * @brief Attaches a variable to the first free channel of the databus.
 * 
 * @note Choose the right macro for each type when attaching variables.
 * 
 * @param pDatabus_ Pointer to databus
 * @param pVar_ Pointer to variable
 */
#define FARAABIN_DataBus_AttachVariable_BOOL_(pDatabus_, pVar_) \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_BOOL, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_BOOL_ARRAY_(pDatabus_, pVar_) \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_BOOL, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_U8_(pDatabus_, pVar_) \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_UINT8, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_U8_ARRAY_(pDatabus_, pVar_) \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_UINT8, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_I8_(pDatabus_, pVar_) \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_INT8, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_I8_ARRAY_(pDatabus_, pVar_) \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_INT8, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_U16_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_UINT16, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_U16_ARRAY_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_UINT16, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_I16_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_INT16, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_I16_ARRAY_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_INT16, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_U32_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_UINT32, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_U32_ARRAY_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_UINT32, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_I32_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_INT32, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_I32_ARRAY_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_INT32, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_U64_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_UINT64, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_U64_ARRAY_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_UINT64, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_I64_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_INT64, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_I64_ARRAY_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_INT64, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_F32_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_FLOAT32, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_F32_ARRAY_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_FLOAT32, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_F64_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_FLOAT64, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_F64_ARRAY_(pDatabus_, pVar_)  \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_PRIMITIVE, eVAR_DATA_TYPE_PRIMITIVE_FLOAT64, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_UserDefined_Enum_(pDatabus_, pVar_) \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_USER_DEFINED_ENUM, 0, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_UserDefined_Enum_ARRAY_(pDatabus_, pVar_) \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_USER_DEFINED_ENUM, 0, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_UserDefined_Struct_(pDatabus_, pVar_) \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(*pVar_), eVAR_DATA_TYPE_USER_DEFINED_ENUM, 0, eFO_TYPE_VAR)

#define FARAABIN_DataBus_AttachVariable_UserDefined_Struct_ARRAY_(pDatabus_, pVar_) \
  fFaraabinFobjectDataBus_AttachVariable(pDatabus_, (uint32_t)pVar_, sizeof(pVar_), eVAR_DATA_TYPE_USER_DEFINED_ENUM, 0, eFO_TYPE_VAR)

/**
 * @brief Attaches a codeblock to the first free channel of the databus.
 * 
 * @param pDatabus_ Pointer to databus
 * @param pCodeblock_ Pointer to codeblock
 */
#define FARAABIN_DataBus_AttachCodeBlock_(pDatabus_, pCodeblock_)  fFaraabinFobjectDataBus_AttachCodeBlock(pDatabus_, (uint32_t)pCodeblock_)

/**
 * @brief Detaches fobjects from all channels.
 * 
 * @param pDatabus_ Pointer to databus
 */
#define FARAABIN_DataBus_DetachAllChannels_(pDatabus_) fFaraabinFobjectDataBus_DetachAllChannels(pDatabus_)

/**
 * @brief Runs the databus.
 * 
 * @param pDatabus_ Pointer to databus
 */
#define FARAABIN_DataBus_Run_(pDatabus_) fFaraabinFobjectDataBus_Run(pDatabus_)

/**
 * @brief Allocates static buffer for the module.
 * 
 * @param pDatabus_ Pointer to the databus object.
 * @param fBuffer_ Pointer to the buffer that user defined statically.
 */
#define FARAABIN_DataBus_AdvFeat_SetBufferChannelsStatically_(pDatabus_, buffer_) \
  do{\
    (pDatabus_)->_pBufferChannels = (buffer_);\
    (pDatabus_)->_isBufferChannelsStatic = true;\
  }while(0)

#define FARAABIN_DataBus_AdvFeat_SetBufferCaptureStatically_(pDatabus_, buffer_) \
  do{\
    (pDatabus_)->_pBufferCapture = (buffer_);\
    (pDatabus_)->_isBufferCaptureStatic = true;\
  }while(0)

#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_DATABUS_WRAPPER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
