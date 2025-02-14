/**
 ******************************************************************************
 * @file           : faraabin_link_buffer.h
 * @brief          : This module handles the buffer of the faraabin.
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
 * This file manages adding data to and getting data from the faraabin buffer.
 * All main functions are written as macroes to improve performance.
 * The data structure for implementing is a circular buffer.
 * All of these functions and macroes are used internally by other faraabin modules.
 * 
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_LINK_BUFFER_H
#define FARAABIN_LINK_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/**
 * @brief Checks whether faraabin buffer is empty or not.
 * 
 */
#define IsBufferEmpty_()  (!FbCircularBuffer._isFull && (FbCircularBuffer._head == FbCircularBuffer._tail))

#ifdef FB_FEATURE_FLAG_BUFFER_OVF

/**
 * @brief Checks if the buffer is overflowed
 * 
 */
#define BufferOvfStatus_() \
  if(FbCircularBuffer._isFull) {\
    FaraabinFlags.Status.Bitfield.BufferOverflow = 1U;\
  }
#else
#define BufferOvfStatus_()
#endif

/**
 * @brief Increments the pointer of the buffer.
 * 
 */
#define BufferIncrementPtr_()  \
do {\
  if(FbCircularBuffer._isFull) {\
    if(++(FbCircularBuffer._tail) == FbCircularBuffer.Size)\
    {\
      FbCircularBuffer._tail = 0U;\
    }\
  }\
  if(++(FbCircularBuffer._head) == FbCircularBuffer.Size) {\
    FbCircularBuffer._head = 0U;\
  }\
  FbCircularBuffer._count++;\
  if(FbCircularBuffer._count > FbCircularBuffer.Size) {\
    FbCircularBuffer._count = FbCircularBuffer.Size;\
  }\
  FbCircularBuffer._isFull = (FbCircularBuffer._head == FbCircularBuffer._tail);\
  BufferOvfStatus_();\
}while(0)

/**
 * @brief Decrements the pointer of the buffer.
 * 
 */
#define BufferDecrementPtr_()  \
do {\
  FbCircularBuffer._count--;\
  FbCircularBuffer._isFull = false;\
  if(++(FbCircularBuffer._tail) == FbCircularBuffer.Size) {\
    FbCircularBuffer._tail = 0;\
  }\
}while(0)

/**
 * @brief Puts an amount of bytes in the buffer,
 * 
 * @param pData_ Pointer to the data.
 * @param size_ Size of the data to put in the buffer.
 */
#define fFaraabinLinkBuffer_Put_(pData_, size_)  \
do {\
  for(uint16_t i__ = 0; i__ < size_; i__++) {\
    FbCircularBuffer.Buffer[FbCircularBuffer._head] = (pData_)[i__];\
    BufferIncrementPtr_();\
  }\
}while(0)

/**
 * @brief Gets an amount of bytes from the buffer,
 * 
 * @param pData_ Pointer to the data.
 * @param size_ Size of the data to get from the buffer.
 */
#define fFaraabinLinkBuffer_Get_(pData_, size_)  \
do {\
  for(uint16_t i__ = 0; i__ < size_; i__++) {\
    if(!IsBufferEmpty_()) {\
      (pData_)[i__] = FbCircularBuffer.Buffer[FbCircularBuffer._tail];\
      BufferDecrementPtr_();\
    } else {\
      break;\
    }\
  }\
}while(0)

/**
 * @brief Flushes the buffer.
 * 
 */
#define fFaraabinLinkBuffer_FlushByPointer_(ppBuff_, pRetSize_)  \
do {\
  if(!IsBufferEmpty_()) {\
    *(ppBuff_) = &FbCircularBuffer.Buffer[FbCircularBuffer._tail];\
  } else {\
    *(ppBuff_) = NULL;\
    *(pRetSize_) = 0U;\
    break;\
  }\
  if(FbCircularBuffer._isFull) {\
    *(pRetSize_) = FbCircularBuffer.Size - FbCircularBuffer._tail;\
  } else {\
    if(FbCircularBuffer._head > FbCircularBuffer._tail) {\
      *(pRetSize_) = FbCircularBuffer._head - FbCircularBuffer._tail;\
    } else {\
      *(pRetSize_) = FbCircularBuffer.Size - FbCircularBuffer._tail;\
    }\
  }\
  FbCircularBuffer._tail += *(pRetSize_);\
  FbCircularBuffer._tail %= FbCircularBuffer.Size;\
  FbCircularBuffer._count -= *(pRetSize_);\
  FbCircularBuffer._isFull = false;\
}while(0)

/* Exported types ------------------------------------------------------------*/
/**
 * @brief Data structure for implementing a ring (circular) buffer.
 * 
 */
typedef struct {
  
  uint8_t *Buffer;  /*!< Pointer to the buffer for saving bytes. */
  uint32_t Size;    /*!< Size of the buffer in bytes. */
  uint32_t _head;   /*!< Index of the head in buffer. */
  uint32_t _tail;   /*!< Index of the tail in buffer. */
  uint32_t _count;  /*!< Internal counter of the elements in buffer. */
  bool _isFull;     /*!< Full flag of the link buffer. */
  
}sFaraabinLinkBuffer;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes faraabin link buffer.
 * 
 * @param txBuffer Pointer to the TX buffer.
 * @param size Size allocated for the buffer.
 * @return InitStat Returns '1' if unsuccessful, otherwise '0'.
 */
uint8_t fFaraabinLinkBuffer_Init(uint8_t *buffer, uint32_t size);

/**
 * @brief Returns the amount of RAM used by Faraabin link.
 * 
 * @return usage Amount of RAM used by Faraabin link in bytes.
 */
uint32_t fFaraabinLinkBuffer_GetRamUsage(void);

void fFaraabinLinkBuffer_Clear(void);

/* Exported variables --------------------------------------------------------*/
extern sFaraabinLinkBuffer FbCircularBuffer;

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_LINK_BUFFER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
