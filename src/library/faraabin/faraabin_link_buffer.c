/**
 ******************************************************************************
 * @file           : faraabin_link_buffer.c
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

/* Includes ------------------------------------------------------------------*/
#include "faraabin_link_buffer.h"

#include <stdlib.h>

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/**
 * @brief This is the main object for the faraabin circular buffer.
 * 
 */
sFaraabinLinkBuffer FbCircularBuffer;

/*
===============================================================================
                ##### fb_link_buffer.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes faraabin link buffer.
 * 
 * @param txBuffer Pointer to the TX buffer.
 * @param size Size allocated for the buffer.
 * @return InitStat Returns '1' if unsuccessful, otherwise '0'.
 */
uint8_t fFaraabinLinkBuffer_Init(uint8_t *txBuffer, uint32_t size) {

  if(size == 0U) {
    return 1;
  }
  FbCircularBuffer.Size = size;
  
  if(txBuffer != NULL) {
    FbCircularBuffer.Buffer = txBuffer;
  } else {

    FbCircularBuffer.Buffer = malloc(FbCircularBuffer.Size);

    if(FbCircularBuffer.Buffer == NULL) {

      return 1;
    }
  }

  fFaraabinLinkBuffer_Clear();
  
  return 0;
}

/**
 * @brief Returns the amount of RAM used by Faraabin link.
 * 
 * @return usage Amount of RAM used by Faraabin link in bytes.
 */
uint32_t fFaraabinLinkBuffer_GetRamUsage(void) {
	
	return sizeof(sFaraabinLinkBuffer) + FbCircularBuffer.Size;
}

void fFaraabinLinkBuffer_Clear(void) {
	
	FbCircularBuffer._head = 0U;
  FbCircularBuffer._tail = 0U;
  FbCircularBuffer._isFull = false;
  FbCircularBuffer._count = 0U;
  
  for(uint32_t i = 0; i < FbCircularBuffer.Size; i++) {
    FbCircularBuffer.Buffer[i] = 0x00U;
  }
}

/*
===============================================================================
                ##### fb_link_buffer.c Private Functions #####
===============================================================================*/

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
