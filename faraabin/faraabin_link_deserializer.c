/**
 ******************************************************************************
 * @file           : faraabin_link_deserializer.c
 * @brief          : Faraabin link deserializer module.
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

/* Includes ------------------------------------------------------------------*/
#include "faraabin_link_deserializer.h"

#include <string.h>

/* Private define ------------------------------------------------------------*/
/**
 * @brief Minimum size of frame in faraabin link.
 * 
 */
#define MINIMUM_FRAME_SIZE  (7U)

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static uint16_t fDeEscape(uint8_t *buffer, uint16_t size);
static bool fIsChecksumOk(uint8_t *buffer, uint16_t size);

/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
              ##### fb_link_deserializer.c Exported Functions #####
===============================================================================*/
/**
 * @brief Deserilizes a frame.
 * 
 * @param buffer Pointer to the buffer that contains the data of the frame.
 * @param size Size of the frame.
 * @param deserializedFrame Pointer to the deserialized frame.
 * @return DeserializationStatus Can be one of the values in DESERIALIZER_RESULT group.
 */
uint8_t fFaraaninLinkDeserializer_Deserialize(uint8_t * const buffer, uint16_t size, sClientFrame *deserializedFrame) {
  uint16_t deescapedSize = 0;

  deescapedSize = fDeEscape(buffer, size);
  if(deescapedSize == 0U) {
    return DESERIALIZE_ERROR_DEESCAPE;
  }
  if(deescapedSize < MINIMUM_FRAME_SIZE) {
    return DESERIALIZE_ERROR_MINIMUM_FRAME_SIZE;
  }

  if(!fIsChecksumOk(buffer, deescapedSize)) {
    return DESERIALIZE_ERROR_CHECKSUM;
  }
	
	deserializedFrame->Control = buffer[0];
	deserializedFrame->FobjectProperty = buffer[1];
	if(memcpy(&deserializedFrame->FobjectPtr, &buffer[2], sizeof(deserializedFrame->FobjectPtr)) == NULL) {
    return DESERIALIZE_ERROR_MEMORY;
  }
	deserializedFrame->Payload = &buffer[6];
	deserializedFrame->PayloadSize = deescapedSize - MINIMUM_FRAME_SIZE;

  return DESERIALIZE_OK;
}

/*
===============================================================================
                ##### fb_link_deserializer.c Private Functions #####
===============================================================================*/
/**
 * @brief Reverses the byte stuffing (byte escaping) sequences in the received frame.
 * 
 * @param buffer Pointer to the buffer that holds the frame.
 * @param size Size of frame.
 * @return uint16_t DeEscaping size.
 */
static uint16_t fDeEscape(uint8_t *buffer, uint16_t size) {
  
  uint16_t j = 0;
  uint16_t deescape_size = size;
  
  for (uint16_t i = 0; i < size;) {
    
    if (buffer[i] == (0x7DU)) {
    
      bool error = false;
      
      switch (buffer[i + 1U]) {
        
        case 0x5E: {
          buffer[j] = (0x7EU);
          
          deescape_size--;
          i += 2U;
          j++;
          break;
        }
        
        case 0x5D: {
          buffer[j] = (0x7DU);
          
          deescape_size--;
          i += 2U;
          j++;
          break;
        }
        
        default: {

          error = true;
          break;
        }
      }

      if(error) {
        return 0;
      }

    } else {
      
      buffer[j] = buffer[i];
      i++;
      j++;
    }
  }
  
  return deescape_size;
}

/**
 * @brief Calculates the checksum of the frame and checks if it is correct. 
 * 
 * @param buffer Pointer to the buffer of the frame.
 * @param size Size of the frame.
 * @return ChecksuIsValid true if valid and false if it is incorrect.
 */
static bool fIsChecksumOk(uint8_t *buffer, uint16_t size) {
  
  uint8_t checksum = 0;
  
  for (uint16_t i = 0; i < size; i++) {
    
    checksum += buffer[i];
    
  }
    
  if(checksum != 0xFFU) {

    return false;
    
  } else {

    return true;

  }
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
