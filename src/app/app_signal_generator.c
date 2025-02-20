/**
 ******************************************************************************
 * @file           : app_signal_generator.c
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

/* Includes ------------------------------------------------------------------*/
#include "app_signal_generator.h"

#include "arm_math.h"
#include <math.h>

/* Private define ------------------------------------------------------------*/
/**
 * @brief Defines the angular frequency (omega) needed for generating a sine wave.
 */
#define OMEGA             (2.0 * (float64_t)PI * frq)

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static bool Init;                 /*!< Init Checks if the module fAppSignalGenerator_Init() is called by user or not */

static float64_t t;               /*!< t Main variable for holding the progress of time in the application, indicating time in seconds */
static float64_t OmegaT;          /*!< OmegaT Holds the product of omega and time (ω * t) for each step in generating the sine wave */
static float64_t SamplingTimeUS;  /*!< SamplingTimeUS Holds the value of sampling time that this module is initialized with */

/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/
/**
 * @brief String array for printing the names of the wave types
 */
char* WaveTypeString[] = {

  "eWAVE_TYPE_NONE",
  "eWAVE_TYPE_PULSE_TRAIN",
  "eWAVE_TYPE_SAW_TOOTH",
  "eWAVE_TYPE_SIN"

};

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                          ##### Exported Functions #####                          ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/

/**
 * @brief This function initializes the signal generator.
 * 
 * @param samplingTimeUS The sampling time that user wants the module to be executed.
 * @return initStatus Returns 0 if successful.
 */
uint8_t fAppSignalGenerator_Init(float64_t samplingTimeUS) {

  Init = false;
  
  SamplingTimeUS = samplingTimeUS;

  Init = true;
  return 0;
}

/**
 * @brief Executes signal generator algorithm for one step of the sampling time specified by user.
 * 
 * @param enable Enable status of the signal generator. If it is false, signal generator output will be zero.
 * @param type Wave type specified by user. It can be one of the types in eWaveType.
 * @param frq Frequency of the wave signal.
 * @param amp Amplitude of the wave signal.
 * @return Output Generated output in current step of execution. 
 */
float64_t fAppSignalGenerator_Run(bool enable, eWaveType type, float64_t frq, float64_t amp) {

  float64_t output = 0.0;

  if(!Init) {
    return output;
  }
  
  /* Main variable for holding the progress of time in the application, indicating time in seconds. */
  t += ((float64_t)SamplingTimeUS / 1000000.0);

  /* The signal generator application sets the value of the output based on the user configurations. */
  /* Users can set the output wave type, enable status, frequency, and amplitude. */
  if(enable) {
    
    switch (type) {

      case eWAVE_TYPE_NONE:
      default: {
        
        /* If the output type is 'eWAVE_TYPE_NONE', the output will be zero. */
        output = 0;
        break;
      }
      case eWAVE_TYPE_PULSE_TRAIN: {

        /* 'eWAVE_TYPE_PULSE_TRAIN' generates a pulse train that alternates between -Amplitude and +Amplitude at the given Frequency. */
        static int8_t pulseSign = 1;
        if (fmod(t, (1.0 / frq)) >= 0.5) {
          pulseSign = -1;
        } else {
          pulseSign = 1;
        }

        output = amp * (float64_t)pulseSign;
        
        break;
      }
      case eWAVE_TYPE_SAW_TOOTH: {

        /* 'eWAVE_TYPE_SAW_TOOTH' generates a sawtooth signal that ramps up from zero to Amplitude at the given Frequency. */
        output = amp * fmod(t, (1.0 / frq));
        
        break;
      }
      case eWAVE_TYPE_SIN: {

        /* 'eWAVE_TYPE_SIN' generates a sine wave that oscillates between -Amplitude and +Amplitude at the given Frequency. */
        OmegaT = OMEGA * t;
        if (OmegaT > (2.0 * (float64_t)PI)) {
          OmegaT -= (2.0 * (float64_t)PI);
        }

        output = amp * arm_sin_f32(OmegaT);

        break;
      }
    }
  } else {
    
    /* If the signal generator Enable status is false, the output will always be zero regardless of the wave type. */
    output = 0.0;
    
  }
  
  return output;
}

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                            ##### Private Functions #####                         ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/


/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
