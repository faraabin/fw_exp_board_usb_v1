/**
 ******************************************************************************
 * @file           : faraabin_type.h
 * @brief          : Important typedefs that are used in faraabin core modules.
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2012-2025 FaraabinCo.
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
#ifndef FARAABIN_TYPE_H
#define FARAABIN_TYPE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
  
/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/**
 * @brief This macro is used to suppress compiler warnings for unused variables.
 * 
 */
#ifndef UNUSED_
#define UNUSED_(x_)  (void)(x_)
#endif

/* Exported types ------------------------------------------------------------*/
#ifndef float32_t__
#define float32_t__
typedef float   float32_t;
#endif

#ifndef float64_t__
#define float64_t__
typedef double  float64_t;
#endif

#ifndef bool_t__
#define bool_t__

typedef bool    bool_t;

#ifndef FALSE
#define FALSE ((bool_t)0)
#endif  //FALSE

#ifndef TRUE
#define TRUE  ((bool_t)1)
#endif  //TRUE

#endif

/**
 * @brief Union for 2 bytes data for easy manipulation of the bytes.
 * 
 * @note MISRA C:2012 Rule 19.2 deviation.
 * 
 */
#ifndef uByte2__
#define uByte2__
typedef union {
  
  uint8_t Byte[2];
  uint16_t U16;
  int16_t I16;

}uByte2;
#endif

/**
 * @brief Union for 4 bytes data for easy manipulation of the bytes.
 * 
 * @note MISRA C:2012 Rule 19.2 deviation.
 * 
 */
#ifndef uByte4__
#define uByte4__
typedef union {
  
  uint8_t Byte[4];
  uint16_t U16[2];
  int16_t I16[2];
  uint32_t U32;
  int32_t I32;
  float32_t F32;

}uByte4;
#endif

/**
 * @brief Union for 8 bytes data for easy manipulation of the bytes.
 * 
 * @note MISRA C:2012 Rule 19.2 deviation.
 * 
 */
#ifndef uByte8__
#define uByte8__
typedef union {
  
  uint8_t Byte[8];
  uint16_t U16[4];
  int16_t I16[4];
  uint32_t U32[2];
  int32_t I32[2];
  float32_t F32[2];
  uint64_t U64;
  int64_t I64;
  float64_t F64;

}uByte8;
#endif

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_TYPE_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
