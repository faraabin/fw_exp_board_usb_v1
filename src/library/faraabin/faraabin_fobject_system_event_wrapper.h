/**
 ******************************************************************************
 * @file           : faraabin_fobject_system_event_wrapper.h
 * @brief          : System events wrappers. These are for internal use only.
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
#ifndef FARAABIN_FOBJECT_SYSTEM_EVENT_WRAPPER_H
#define FARAABIN_FOBJECT_SYSTEM_EVENT_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_link_serializer.h"
	
#include "faraabin_config.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/**
 * @brief Sends an End system event to Faraabin.
 * 
 */
#define Faraabin_EventSystem_End_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, 0, false, NULL, NULL, true)

/**
 * @brief Sends an End system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExtended_End_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, 0, false, NULL, NULL, true)

/**
 * @brief Sends a Param End system event to Faraabin.
 * 
 */
#define Faraabin_EventSystem_ParamEnd_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, 0, false, NULL, NULL, true)

/**
 * @brief Sends a Param End system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExtended_ParamEnd_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, 0, false, NULL, NULL, true)

/**
 * @brief Sends a Response End system event to Faraabin.
 * 
 */
#define Faraabin_EventSystem_EndResponse_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, NULL, NULL, true)

/**
 * @brief Sends a Response End system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExtended_EndResponse_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, NULL, NULL, true)

/**
 * @brief Sends a Response system event to Faraabin.
 * 
 */
#define Faraabin_EventSystem_Response_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, NULL, NULL, false)

/**
 * @brief Sends a Response system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExtended_Response_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, NULL, NULL, false)

/**
 * @brief Sends a Param Response system event to Faraabin.
 * 
 */
#define Faraabin_EventSystem_ParamResponse_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, req_seq_, true, NULL, NULL, false)

/**
 * @brief Sends a Param Response system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExtended_ParamResponse_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, req_seq_, true, NULL, NULL, false)

/**
 * @brief Sends a Param Response End system event to Faraabin.
 * 
 */
#define Faraabin_EventSystem_ParamEndResponse_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, req_seq_, true, NULL, NULL, true)

/**
 * @brief Sends a Param Response End system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExtended_ParamEndResponse_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, req_seq_, true, NULL, NULL, true)

/**
 * @brief Sends a Response with payload system event to Faraabin.
 * 
 */
#define Faraabin_EventSystem_ResponsePayload_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, generatepayloadFunc_, generatePayloadParam_, req_seq_, isEnd_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, generatepayloadFunc_, generatePayloadParam_, isEnd_)

/**
 * @brief Sends a Response with payload system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExtended_ResponsePayload_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, generatepayloadFunc_, generatePayloadParam_, req_seq_, isEnd_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, generatepayloadFunc_, generatePayloadParam_, isEnd_)

/**
 * @brief Sends an Exception End system event to Faraabin.
 * 
 */
#define Faraabin_EventSystemException_End_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, 0, false, NULL, NULL, true)

/**
 * @brief Sends an Exception End system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExceptionExtended_End_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, 0, false, NULL, NULL, true)

/**
 * @brief Sends an Exception End with param system event to Faraabin.
 * 
 */
#define Faraabin_EventSystemException_ParamEnd_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, 0, false, NULL, NULL, true)

/**
 * @brief Sends an Exception End with param system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExceptionExtended_ParamEnd_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, 0, false, NULL, NULL, true)

/**
 * @brief Sends an Exception Response End system event to Faraabin.
 * 
 */
#define Faraabin_EventSystemException_EndResponse_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, NULL, NULL, true)

/**
 * @brief Sends an Exception Response End system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExceptionExtended_EndResponse_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, NULL, NULL, true)

/**
 * @brief Sends an Exception Response system event to Faraabin.
 * 
 */
#define Faraabin_EventSystemException_Response_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, NULL, NULL, false)

/**
 * @brief Sends an Exception Response system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExceptionExtended_Response_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, NULL, NULL, false)

/**
 * @brief Sends an Exception Response with param system event to Faraabin.
 * 
 */
#define Faraabin_EventSystemException_ParamResponse_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, req_seq_, true, NULL, NULL, false)

/**
 * @brief Sends an Exception Response with param system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExceptionExtended_ParamResponse_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, req_seq_, true, NULL, NULL, false)

/**
 * @brief Sends an Exception Response End with param system event to Faraabin.
 * 
 */
#define Faraabin_EventSystemException_ParamEndResponse_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, req_seq_, true, NULL, NULL, true)

/**
 * @brief Sends an Exception Response End with param system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExceptionExtended_ParamEndResponse_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, param_, param_size_, req_seq_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, param_, param_size_, req_seq_, true, NULL, NULL, true)

/**
 * @brief Sends an Exception Response with payload system event to Faraabin.
 * 
 */
#define Faraabin_EventSystemException_ResponsePayload_(fobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, generatepayloadFunc_, generatePayloadParam_, req_seq_, isEnd_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, 0U, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, generatepayloadFunc_, generatePayloadParam_, isEnd_)

/**
 * @brief Sends an Exception Response with payload system event with extended pointer to Faraabin.
 * 
 */
#define Faraabin_EventSystemExceptionExtended_ResponsePayload_(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, event_id_, generatepayloadFunc_, generatePayloadParam_, req_seq_, isEnd_) \
  fFaraabinLinkSerializer_SerializeEvent(fobjectPtr_, extendedFobjectPtr_, fobjectSeqPtr_, fobjectEnableState, eFB_COMMON_PROP_ID_EVENT_LIB_EXCEPTION, eFO_EVENT_SEVERITY_INFO, event_id_, NULL, 0, req_seq_, true, generatepayloadFunc_, generatePayloadParam_, isEnd_)  

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_SYSTEM_EVENT_WRAPPER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
