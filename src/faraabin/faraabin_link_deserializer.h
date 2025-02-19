/**
 ******************************************************************************
 * @file           : faraabin_link_deserializer.h
 * @brief          : Faraabin link deserializer module heder file.
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
#ifndef FARAABIN_LINK_DESERIALIZER_H
#define FARAABIN_LINK_DESERIALIZER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
/** @defgroup DESERIALIZER_RESULT group.
 *  @{
 */

#define DESERIALIZE_OK                        (0U)  /*!< No error. */
#define DESERIALIZE_ERROR_MINIMUM_FRAME_SIZE  (1U)  /*!< Faraabin deserializer detected a frame smaller with size smaller than minimum frame size. */
#define DESERIALIZE_ERROR_DEESCAPE            (2U)  /*!< Faraabin deserializer detected a byte escaping error in the frame. */
#define DESERIALIZE_ERROR_CHECKSUM            (3U)  /*!< Faraabin deserializer detected a checksum error in the frame. */
#define DESERIALIZE_ERROR_MEMORY              (4U)  /*!< Clearing memory error. */

/** @} */ //End of DESERIALIZER_RESULT

/* Exported macro ------------------------------------------------------------*/
/**
 * @brief Generates control byte in client frame based on the input arguments.
 * 
 * @param reqSeq_ Request sequence.
 * @param priority_ Priority.
 * @param accessType_ Access type.
 */
#define ClientFrame_GenerateControlByte_(reqSeq_, priority_, accessType_) \
  (uint8_t)(((reqSeq_) & 0x0FU) + (((priority_) & 0x01U) << 5U) + (((accessType_) & 0x01U) << 6U))

#define ClientFrame_GetRequestSequence_(byte_)  (uint8_t)((byte_) & 0x0FU)
#define ClientFrame_GetPriority_(byte_)         (uint8_t)(((byte_) >> 5U) & 0x01U)
#define ClientFrame_GetAccessType_(byte_)       (uint8_t)(((byte_) >> 6U) & 0x01U)

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Client frame object.
 * 
 */
typedef struct {
  
  uint8_t Control;              /*!< Control field of the client frame. */

  uint8_t FobjectProperty;      /*!< Fobject property of the client frame. */

  uint32_t FobjectPtr;          /*!< Pointer to the fobject in client frame. */
  
  uint8_t *Payload;             /*!< Payload of the client frame. */
  
	uint16_t PayloadSize;         /*!< Payload size in client frame. */
  
}sClientFrame;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Deserilizes a frame.
 * 
 * @param buffer Pointer to the buffer that contains the data of the frame.
 * @param size Size of the frame.
 * @param deserializedFrame Pointer to the deserialized frame.
 * @return DeserializationStatus Can be one of the values in DESERIALIZER_RESULT group.
 */
uint8_t fFaraaninLinkDeserializer_Deserialize(uint8_t * const buffer, uint16_t size, sClientFrame *deserializedFrame);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_LINK_DESERIALIZER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
