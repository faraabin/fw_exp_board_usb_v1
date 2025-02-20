/**
 ******************************************************************************
 * @file           : faraabin_fobject_eventgroup_wrapper.h
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
#ifndef FARAABIN_FOBJECT_EVENTGROUP_WRAPPER_H
#define FARAABIN_FOBJECT_EVENTGROUP_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_eventgroup.h"

#include "faraabin_config.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifdef FARAABIN_ENABLE

/**
 * @brief Declares a event group fobject.
 * 
 * @param egName_ Name of the fobject
 */
#define FARAABIN_EVENT_GROUP_DEF_(egName_)  \
sFaraabinFobjectEventGroup egName_ = {\
  .Name = #egName_,\
  .Enable = false\
}

/**
 * @brief This macro defines an event group (just like FARAABIN_EVENT_GROUP_DEF_()) but statically.
 * 
 * @param egName_ Name of the fobject.
 */
#define FARAABIN_EVENT_GROUP_DEF_STATIC_(egName_) static FARAABIN_EVENT_GROUP_DEF_(egName_)

/**
 * @brief This macro is used to extern a previously defined event group for global access to it.
 * 
 * @param egName_ Name of the fobject.
 */
#define FARAABIN_EVENT_GROUP_DEF_EXTERN_(egName_) extern sFaraabinFobjectEventGroup egName_

/**
 * @brief Sets the value of an event group field. In order to be able to disable Faraabin completely,
 *        use this macro to assign values of the event group fobject.
 * 
 * @param egField_ Value of the event group to assign.
 * @param value_ Value to be assigned.
 */
#define FARAABIN_EVENT_GROUP_SET_VALUE_(egField_, value_) egField_ = value_

/**
 * @brief Initializes an event group with given path.
 * 
 * @param pEg_ Pointer to the event group fobject
 * @param path_ Path of the fobject
 */
#define FARAABIN_EventGroup_Init_WithPath_(pEg_, path_) \
  do{\
    (pEg_)->Path = path_;\
    (pEg_)->Filename = FILENAME__;\
    uint8_t ret = fFaraabinFobjectEventGroup_Init(pEg_);\
    (void)ret;\
  }while(0)

/**
 * @brief Initializes an event group with "root" path.
 * 
 * @param pEg_ Pointer to the event group fobject
 */
#define FARAABIN_EventGroup_Init_(pEg_)  FARAABIN_EventGroup_Init_WithPath_(pEg_, RootPath____)

/**
 * @brief Enables an event group.
 * 
 * @param pEg_ Pointer to the event group fobject
 */
#define FARAABIN_EventGroup_Enable_(pEg_)  \
  do {\
    (pEg_)->Enable = true;\
  }while(0)

/**
 * @brief Disables an event group.
 * 
 * @param pEg_ Pointer to the event group fobject
 */
#define FARAABIN_EventGroup_Disable_(pEg_)  \
  do {\
    (pEg_)->Enable = false;\
  }while(0)

/**
 * @brief Generates string type event and sends it to the specified event group.
 * 
 * @note This macro works just like printf() function for printing an stream of string.
 * 
 * @param pEg_ Pointer to the event group.
 */
#define FARAABIN_PrintfTo_(pEg_, ...) \
fFaraabinLinkSerializer_SerializeEventPrintf((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFB_COMMON_PROP_ID_EVENT_USER_DATA, eFO_EVENT_SEVERITY_INFO, __VA_ARGS__)

/**
 * @brief Generates a string type event with severity of WARNING and sends it to the specified event group.
 * 
 * @note This macro works just like printf() function for printing an stream of string.
 * 
 * @param pEg_ Pointer to the event group.
 */
#define FARAABIN_PrintfWarningTo_(pEg_, ...) \
fFaraabinLinkSerializer_SerializeEventPrintf((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFB_COMMON_PROP_ID_EVENT_USER_DATA, eFO_EVENT_SEVERITY_WARNING, __VA_ARGS__)

/**
 * @brief Generates a string type event with severity of ERROR and sends it to the specified event group.
 * 
 * @note This macro works just like printf() function for printing an stream of string.
 * 
 * @param pEg_ Pointer to the event group.
 */
#define FARAABIN_PrintfErrorTo_(pEg_, ...) \
fFaraabinLinkSerializer_SerializeEventPrintf((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFB_COMMON_PROP_ID_EVENT_USER_DATA, eFO_EVENT_SEVERITY_ERROR, __VA_ARGS__)

/**
 * @brief Generates a string type event and sends it to the terminal of the specified event group.
 * 
 * @note This macro works just like printf() function for printing an stream of string.
 * 
 * @param pEg_ Pointer to the event group.
 */
#define FARAABIN_PrintfToTerminal_(pEg_, ...) \
fFaraabinLinkSerializer_SerializeEventPrintf((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFB_COMMON_PROP_ID_EVENT_USER_TERMINAL, eFO_EVENT_SEVERITY_INFO, __VA_ARGS__)

#ifdef FB_FEATURE_FLAG_MCU_CLI

/**
 * @brief Generates a string type event and sends it to the event group of a function in function group.
 * 
 * @note This macro works just like printf() function for printing an stream of string.
 * 
 */
#define FARAABIN_PrintfToFunction_(...) \
fFaraabinLinkSerializer_SerializeEventPrintf((uint32_t)__funcPtr__, objectPtr, &(__funcPtr__->Seq), true, eFB_COMMON_PROP_ID_EVENT_USER_DATA, eFO_EVENT_SEVERITY_INFO, __VA_ARGS__)
#endif

/**
 * @brief Generates string type event and sends it to the specified event group.
 * 
 * @note This macro works just like fputs() function for printing an stream of string.
 * 
 * @param pEg_ Pointer to the event group.
 * @param pStr_ Pointer to the string.
 */
#define FARAABIN_PutsTo_(pEg_, pStr_) \
fFaraabinLinkSerializer_SerializeEventPuts((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFB_COMMON_PROP_ID_EVENT_USER_DATA, eFO_EVENT_SEVERITY_INFO, pStr_)

/**
 * @brief Generates string type event with severity of WARNING and sends it to the specified event group.
 * 
 * @note This macro works just like fputs() function for printing an stream of string.
 * 
 * @param pEg_ Pointer to the event group.
 * @param pStr_ Pointer to the string.
 */
#define FARAABIN_PutsWarningTo_(pEg_, pStr_) \
fFaraabinLinkSerializer_SerializeEventPuts((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFB_COMMON_PROP_ID_EVENT_USER_DATA, eFO_EVENT_SEVERITY_WARNING, pStr_)

/**
 * @brief Generates string type event with severity of ERROR and sends it to the specified event group.
 * 
 * @note This macro works just like fputs() function for printing an stream of string.
 * 
 * @param pEg_ Pointer to the event group.
 * @param pStr_ Pointer to the string.
 */
#define FARAABIN_PutsErrorTo_(pEg_, pStr_) \
fFaraabinLinkSerializer_SerializeEventPuts((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFB_COMMON_PROP_ID_EVENT_USER_DATA, eFO_EVENT_SEVERITY_ERROR, pStr_)

/**
 * @brief Generates string type event and sends it to the terminal of the event group.
 * 
 * @note This macro works just like fputs() function for printing an stream of string.
 * 
 * @param pEg_ Pointer to the event group.
 * @param pStr_ Pointer to the string.
 */
#define FARAABIN_PutsToTerminal_(pEg_, pStr_) \
fFaraabinLinkSerializer_SerializeEventPuts((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFB_COMMON_PROP_ID_EVENT_USER_TERMINAL, eFO_EVENT_SEVERITY_INFO, pStr_)

#ifdef FB_FEATURE_FLAG_MCU_CLI

/**
 * @brief Generates string type event and sends it to the event group of a function group.
 * 
 * @note This macro works just like fputs() function for printing an stream of string.
 * 
 * @param pStr_ Pointer to the string.
 */
#define FARAABIN_PutsToFunction_(pStr_) \
fFaraabinLinkSerializer_SerializeEventPuts((uint32_t)__funcPtr__, objectPtr, &(__funcPtr__->Seq), true, eFB_COMMON_PROP_ID_EVENT_USER_DATA, eFO_EVENT_SEVERITY_INFO, pStr_)
#endif

/**
 * @brief Sends an enumerated event to a specified event group.
 * 
 * @param pEg_ Pointer to the event group.
 * @param enumName_ Name of the defined enum type using varType fobject.
 * @param eventId_ ID of the event.
 */
#define FARAABIN_SendEventTo_(pEg_, enumName_, eventId_) \
fFaraabinLinkSerializer_SerializeEventEnumId((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFO_EVENT_SEVERITY_INFO, (uint32_t)&__faraabin_vartype_##enumName_, eventId_)

/**
 * @brief Sends an enumerated event with severity of WARNING to a specified event group.
 * 
 * @param pEg_ Pointer to the event group.
 * @param enumName_ Name of the defined enum type using varType fobject.
 * @param eventId_ ID of the event.
 */
#define FARAABIN_SendEventWarningTo_(pEg_, enumName_, eventId_) \
fFaraabinLinkSerializer_SerializeEventEnumId((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFO_EVENT_SEVERITY_WARNING, (uint32_t)&__faraabin_vartype_##enumName_, eventId_)

/**
 * @brief Sends an enumerated event with severity of ERROR to a specified event group.
 * 
 * @param pEg_ Pointer to the event group.
 * @param enumName_ Name of the defined enum type using varType fobject.
 * @param eventId_ ID of the event.
 */
#define FARAABIN_SendEventErrorTo_(pEg_, enumName_, eventId_) \
fFaraabinLinkSerializer_SerializeEventEnumId((uint32_t)pEg_, 0, &((pEg_)->Seq), (pEg_)->Enable, eFO_EVENT_SEVERITY_ERROR, (uint32_t)&__faraabin_vartype_##enumName_, eventId_)

#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_EVENTGROUP_WRAPPER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
