/**
 ******************************************************************************
 * @file           : main.c
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
 * 
 * @verbatim
 * In this example, a simple software signal generator is used to demonstrate some of
 * Faraabin's probes and basic features.
 *
 * The signal generator has four inputs:
 * - Enable: Determines whether to output the generated signal.
 * - Wave type: Specifies the type of generated output signal (nothing, pulse train, sawtooth, or sine wave).
 * - Amplitude: Sets the amplitude of the generated signal.
 * - Frequency: Sets the frequency of the generated signal.
 *
 * Users can configure these parameters in Faraabin and observe their effects on the generated output.
 * This example illustrates:
 * - How to declare and introduce your variables and typedefs to Faraabin
 * - Sending data periodically to Faraabin
 * - Sending messages via different categories to Faraabin
 * - Sending commands to the MCU from a PC to run a function while the main application is executing
 * 
 * You can access the Faraabin documentation at github and find out more about other features, probes and add-ons.
 * @endverbatim
 * 
 */

/* Includes ------------------------------------------------------------------*/
#include "app_signal_generator.h"
#include "led_function.h"

#include "bsp.h"

#include "chrono.h"
#include "faraabin.h"

/* Private define ------------------------------------------------------------*/
/**
 * @brief Period of the application's execution in microseconds.
 */
#define RUNTIME_PERIOD_US (1000)

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static eWaveType WaveType;  /*!< WaveType Type of the currently generated wave */
static bool Enable;         /*!< Enable Signal generator enable status: 'false' results in 0 output, 'true' generates signal as per user configurations */
static float64_t Frequency; /*!< Frequency Frequency of the generated wave */
static float64_t Amplitude; /*!< Amplitude Amplitude of the generated wave */
static float64_t Output;    /*!< Output Output of the signal generator */

static uint32_t Execution;  /*!< Execution Holds the execution time of the application in microseconds */
static uint32_t Interval;   /*!< Interval Holds the interval of the running application in microseconds */

static sChrono Chrono;      /*!< Chrono Chrono object used for controlling the application's execution based on RUNTIME_PERIOD_US */

FARAABIN_CONTAINER_DEF_STATIC_(Container);        /*!< Container Faraabin container for introducing user variables to Faraabin */
FARAABIN_DATABUS_DEF_STATIC_(Databus);            /*!< Databus Faraabin databus for creating a data stream between MCU and PC to send data periodically */
FARAABIN_EVENT_GROUP_DEF_STATIC_(TypeEventGroup); /*!< TypeEventGroup Faraabin event group for sending wave type change events */
FARAABIN_EVENT_GROUP_DEF_STATIC_(FreqEventGroup); /*!< FreqEventGroup Faraabin event group for sending frequency change events */
FARAABIN_EVENT_GROUP_DEF_STATIC_(AmplEventGroup); /*!< AmplEventGroup Faraabin event group for sending amplitude change events */
FARAABIN_VAR_TYPE_DEF_STATIC_(eWaveType);         /*!< eWaveType Faraabin variable type object for introducing 'eWaveType' typedef to Faraabin */

/* Private function prototypes -----------------------------------------------*/
static void FaraabinReceiveFrameHandler(uint8_t *data, uint16_t size);

/* Variables -----------------------------------------------------------------*/

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                          ##### Exported Functions #####                          ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/

/**
  * @brief  The application's entry point.
  * @retval int
  */
int main(void) {
  
  /* Board Initialization -----------------------------------------------------*/
  /* Initialize the tick of the system. */
  fBsp_TickInit();
  /* Initialize chrono module. */
  if (fChrono_Init(fBsp_GetTick) != CHRONO_OK) {
    Error_Handler();
  }
  /* Initialize board peripherals. */
  fBsp_Init();
  
  /* Faraabin Initialization --------------------------------------------------*/
  /* Register USB receive callback to a function that handles Faraabin frames. */
  fBsp_VCP_RegisterFrameReceivedCallback(FaraabinReceiveFrameHandler);
  /* Initialize Faraabin as per user configurations in faraabin_config.h and user interface in faraabin_port.c */
  if (fFaraabin_Init() != FARAABIN_OK) {
    Error_Handler();
  }
  
  /* Faraabin Probes Initialization -------------------------------------------*/
  /* Initialize the container that introduces user variables and functions to Faraabin. */
  FARAABIN_Container_Init_(&Container);
  /* Initialize eWaveType variable type object. Faraabin needs this information to know the type of eWaveType. */
  FARAABIN_VAR_TYPE_ENUM_INIT_(eWaveType);

  /* Initialize Faraabin databus with 5 channels and a division factor of 5. */
  /* The databus will start in stream mode while the 'Output' variable is already attached to it. */
  FARAABIN_DATABUS_SET_VALUE_(Databus.ChannelQty, 5);
  FARAABIN_DATABUS_SET_VALUE_(Databus.StreamDivideBy, 5);
  FARAABIN_DataBus_Init_(&Databus);
  FARAABIN_DataBus_StartStreamMode_(&Databus);
  FARAABIN_DataBus_AttachVariable_F64_(&Databus, &Output);
  FARAABIN_DataBus_Enable_(&Databus);

  /* Initialize Faraabin event groups for sending type, amplitude, and frequency change events. */
  FARAABIN_EventGroup_Init_(&TypeEventGroup);
  FARAABIN_EventGroup_Init_(&AmplEventGroup);
  FARAABIN_EventGroup_Init_(&FreqEventGroup);

  /* Initialize LedFunction group and introduce it to Faraabin. */
  FARAABIN_FunctionGroupType_Init_(&LedFunction);
  
  /* Initializing signal generator and setting initial values -----------------*/
  fAppSignalGenerator_Init(RUNTIME_PERIOD_US);
  WaveType = eWAVE_TYPE_SIN;
  Frequency = 1.0;
  Amplitude = 1.0;
  
  fChrono_StartTimeoutUs(&Chrono, RUNTIME_PERIOD_US);
  
  /* Infinite loop ------------------------------------------------------------*/
  while (1) {
    /* This is the runtime in which the application runs periodically. */
    /* fChrono_StartTimeoutUs() is called immediately after fChrono_IsTimeout() to set the chrono for the next step of the application's execution. */
    if (fChrono_IsTimeout(&Chrono)) {
      fChrono_StartTimeoutUs(&Chrono, RUNTIME_PERIOD_US);
      
      /* lastTickBegin and tickBegin variables are used for execution and interval time measurements. */
      /* They store the last execution and current execution time tags. */
      static uint32_t lastTickBegin = 0;
      uint32_t tickBegin = fChrono_GetTick();

      /* Run signal generator with given inputs. Result is written onto Output. */
      Output = fAppSignalGenerator_Run(Enable, WaveType, Frequency, Amplitude);

      /* This part detects any change in the 'WaveType' variable value. If a change is detected, a message is sent to Faraabin. */
      static eWaveType OldType = eWAVE_TYPE_NONE;
      if (OldType != WaveType) {
        
        FARAABIN_PrintfWarningTo_(&TypeEventGroup, "Wave type changed from %s to %s", WaveTypeString[OldType], WaveTypeString[WaveType]);
        OldType = WaveType;

      }

      /* This part detects any change in the 'Amplitude' variable value. If a change is detected, a message is sent to Faraabin. */
      static float64_t OldAmp = 0.0;
      if (OldAmp != Amplitude) {
        
        FARAABIN_PrintfWarningTo_(&AmplEventGroup, "Amplitude changed from %lf to %lf", OldAmp, Amplitude);
        OldAmp = Amplitude;

      }

      /* This part detects any change in the 'Frequency' variable value. If a change is detected, a message is sent to Faraabin. */
      static float64_t OldFreq = 0.0;
      if (OldFreq != Frequency) {
        
        FARAABIN_PrintfWarningTo_(&FreqEventGroup, "Frequency changed from %lf to %lf", OldFreq, Frequency);
        OldFreq = Frequency;

      }
      
      /* Faraabin databus is running in the same runtime as the application. */
      /* Users can obtain 'Output' data with this runtime frequency divided by the 'Databus.StreamDivideBy' value. */
      FARAABIN_DataBus_Run_(&Databus);
      
      /* At the end of each step, Interval and Execution times are calculated to evaluate algorithm times. */
      Interval = fChrono_TimeSpanUs(lastTickBegin, tickBegin);
      Execution = fChrono_TimeSpanUs(tickBegin, fChrono_GetTick());
      
      lastTickBegin = tickBegin;

    }
    
    /* fFaraabin_Run() function is executed in a low priority part of the main loop. Whenever the CPU has time, it will execute this part. */
    fFaraabin_Run();

  }
}

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                            ##### Private Functions #####                         ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/

/**
 * @brief This function is used to define eWaveType fields to faraabin.
 * 
 */
FARAABIN_VAR_TYPE_ENUM_FUNC_(eWaveType) {

  FARAABIN_VAR_TYPE_ENUM_FUNC_START_(eWaveType);

  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eWAVE_TYPE_NONE);
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eWAVE_TYPE_PULSE_TRAIN);
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eWAVE_TYPE_SAW_TOOTH);
  FARAABIN_VAR_TYPE_ENUM_MEMBER_(eWAVE_TYPE_SIN);

  FARAABIN_VAR_TYPE_ENUM_FUNC_END_(eWaveType);
}

/**
 * @brief This function is used to define all variables and functions to faraabin.
 * 
 */
FARAABIN_CONTAINER_FUNC_(Container) {

  FARAABIN_VAR_ENUM_(WaveType, eWaveType);

  FARAABIN_VAR_BOOL_(Enable);
  FARAABIN_VAR_F64_(Frequency);
  FARAABIN_VAR_F64_(Amplitude);
  FARAABIN_VAR_F64_(Output);

  FARAABIN_VAR_U32_(Execution);
  FARAABIN_VAR_U32_(Interval);

  FARAABIN_FUNCTION_GROUP_DICT_(LedFunction);

  FARAABIN_CONTAINER_FUNC_END_;
}

/**
  * @brief Passes received frame from the PC to fFaraabin_CharReceived() function for parsing the frame.
  * @param data Pointer to the received frame buffer
  * @param size Size of the received frame
  * @retval None
  */
static void FaraabinReceiveFrameHandler(uint8_t *data, uint16_t size) {
  
  for(uint16_t i = 0; i < size; i++) {
    
    fFaraabin_CharReceived(data[i]);
    
  }
  
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
