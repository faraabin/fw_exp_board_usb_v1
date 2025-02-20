/**
 ******************************************************************************
 * @file           : app_signal_generator.h
 * @brief          :
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 FaraabinCo.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component
 * 
 * https://faraabinco.ir/
 * https://github.com/FaraabinCo
 *
 ******************************************************************************
 * @verbatim
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef app_signal_generator_H
#define app_signal_generator_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Exported defines ----------------------------------------------------------*/
#ifndef float64_t
    typedef double  float64_t;
#endif

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief Defines possible wave types that the user can select,
 *        which the application will generate.
 */
typedef enum {

  eWAVE_TYPE_NONE = 0,
  eWAVE_TYPE_PULSE_TRAIN,
  eWAVE_TYPE_SAW_TOOTH,
  eWAVE_TYPE_SIN

} eWaveType;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
uint8_t fAppSignalGenerator_Init(float64_t samplingTimeUS);
float64_t fAppSignalGenerator_Run(bool enable, eWaveType type, float64_t frq, float64_t amp);

/* Exported variables --------------------------------------------------------*/
extern char* WaveTypeString[];

#ifdef __cplusplus
}
#endif

#endif /* APP_SIGNAL_GENERATOR_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
