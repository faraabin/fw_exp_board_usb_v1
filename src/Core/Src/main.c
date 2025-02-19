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
 * 
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usb_device.h"
#include "gpio.h"

#include "usbd_cdc_if.h"

#include "led_function.h"

#include "chrono.h"
#include "faraabin.h"

#include "arm_math.h"
#include <math.h>

/* Private define ------------------------------------------------------------*/
/**
 * @brief Period of the application's execution in microseconds.
 */
#define RUNTIME_PERIOD_US (1000)

/**
 * @brief Defines the angular frequency (omega) needed for generating a sine wave.
 */
#define OMEGA             (2.0 * (float64_t)PI * Frequency)

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
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

/* Private variables ---------------------------------------------------------*/
static eWaveType WaveType;  /*!< WaveType Type of the currently generated wave */
static bool Enable;         /*!< Enable Signal generator enable status: 'false' results in 0 output, 'true' generates signal as per user configurations */
static float64_t Frequency; /*!< Frequency Frequency of the generated wave */
static float64_t Amplitude; /*!< Amplitude Amplitude of the generated wave */
static float64_t Output;    /*!< Output Output of the signal generator */

static uint32_t Execution;  /*!< Execution Holds the execution time of the application in microseconds */
static uint32_t Interval;   /*!< Interval Holds the interval of the running application in microseconds */

static float64_t t;         /*!< t Main variable for holding the progress of time in the application, indicating time in seconds */
static float64_t OmegaT;    /*!< OmegaT Holds the product of omega and time (ω * t) for each step in generating the sine wave */

static sChrono Chrono;      /*!< Chrono Chrono object used for controlling the application's execution based on RUNTIME_PERIOD_US */

/**
 * @brief String array for printing the names of the wave types
 */
static char* WaveTypeString[] = {

  "eWAVE_TYPE_NONE",
  "eWAVE_TYPE_PULSE_TRAIN",
  "eWAVE_TYPE_SAW_TOOTH",
  "eWAVE_TYPE_SIN"

};

FARAABIN_CONTAINER_DEF_STATIC_(Container);        /*!< Container Faraabin container for introducing user variables to Faraabin */
FARAABIN_DATABUS_DEF_STATIC_(Databus);            /*!< Databus Faraabin databus for creating a data stream between MCU and PC to send data periodically */
FARAABIN_EVENT_GROUP_DEF_STATIC_(TypeEventGroup); /*!< TypeEventGroup Faraabin event group for sending wave type change events */
FARAABIN_EVENT_GROUP_DEF_STATIC_(FreqEventGroup); /*!< FreqEventGroup Faraabin event group for sending frequency change events */
FARAABIN_EVENT_GROUP_DEF_STATIC_(AmplEventGroup); /*!< AmplEventGroup Faraabin event group for sending amplitude change events */
FARAABIN_VAR_TYPE_DEF_STATIC_(eWaveType);         /*!< eWaveType Faraabin variable type object for introducing 'eWaveType' typedef to Faraabin */

/* Private function prototypes -----------------------------------------------*/
static void FaraabinReceiveFrameHandler(uint8_t *data, uint16_t size);
static void SystemClock_Config(void);

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
  /* MCU Configuration ------------------------------------------------------*/
  /* Reset all peripherals, initialize the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock. */
  SystemClock_Config();
  /* Initialize all configured peripherals. */
  MX_GPIO_Init();
  MX_USB_DEVICE_Init();
  /* Register USB receive callback to a function that handles Faraabin frames. */
  CDC_RegisterReceiveCallback_FS(FaraabinReceiveFrameHandler);

  /* Chrono Initialization --------------------------------------------------*/
  /* Initialize the chrono module. */
  /* HAL_GetTick function pointer is passed to chrono for measuring time ticks. */
  if (fChrono_Init(HAL_GetTick) != CHRONO_OK) {
    Error_Handler();
  }
  /* Initialize Faraabin as per user configurations in faraabin_config.h and user interface in faraabin_port.c */
  if (fFaraabin_Init() != FARAABIN_OK) {
    Error_Handler();
  }
   
  /* Faraabin Probes Initialization -----------------------------------------*/
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

  /* Set initial values for the variables and start the chrono ---------------*/
  WaveType = eWAVE_TYPE_SIN;
  Frequency = 1.0;
  Amplitude = 1.0;
  t = 0.0;
  
  fChrono_StartTimeoutUs(&Chrono, RUNTIME_PERIOD_US);
 
  /* Infinite loop */
  while (1) {
    /* This is the runtime in which the application runs periodically. */
    /* fChrono_StartTimeoutUs() is called immediately after fChrono_IsTimeout() to set the chrono for the next step of the application's execution. */
    if (fChrono_IsTimeout(&Chrono)) {
      fChrono_StartTimeoutUs(&Chrono, RUNTIME_PERIOD_US);
      
      /* lastTickBegin and tickBegin variables are used for execution and interval time measurements. */
      /* They store the last execution and current execution time tags. */
      static uint32_t lastTickBegin = 0;
      uint32_t tickBegin = fChrono_GetTick();

      /* Main variable for holding the progress of time in the application, indicating time in seconds. */
      t += ((float64_t)RUNTIME_PERIOD_US / 1000000.0);

      /* The signal generator application sets the value of the output based on the user configurations. */
      /* Users can set the output wave type, enable status, frequency, and amplitude. */
      if(Enable) {
        
        switch (WaveType) {

          case eWAVE_TYPE_NONE:
          default: {
            
            /* If the output type is 'eWAVE_TYPE_NONE', the output will be zero. */
            Output = 0;
            break;
          }
          case eWAVE_TYPE_PULSE_TRAIN: {

            /* 'eWAVE_TYPE_PULSE_TRAIN' generates a pulse train that alternates between -Amplitude and +Amplitude at the given Frequency. */
            static int8_t pulseSign = 1;
            if (fmod(t, (1.0 / Frequency)) >= 0.5) {
              pulseSign = -1;
            } else {
              pulseSign = 1;
            }

            Output = Amplitude * (float64_t)pulseSign;
            
            break;
          }
          case eWAVE_TYPE_SAW_TOOTH: {

            /* 'eWAVE_TYPE_SAW_TOOTH' generates a sawtooth signal that ramps up from zero to Amplitude at the given Frequency. */
            Output = Amplitude * fmod(t, (1.0 / Frequency));
            
            break;
          }
          case eWAVE_TYPE_SIN: {

            /* 'eWAVE_TYPE_SIN' generates a sine wave that oscillates between -Amplitude and +Amplitude at the given Frequency. */
            OmegaT = OMEGA * t;
            if (OmegaT > (2.0 * (float64_t)PI)) {
              OmegaT -= (2.0 * (float64_t)PI);
            }

            Output = Amplitude * arm_sin_f32(OmegaT);

            break;
          }
        }
      } else {
        
        /* If the signal generator Enable status is false, the output will always be zero regardless of the wave type. */
        Output = 0.0;
        
      }

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

 /**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void) {
  /* User can add his own implementation to report error return state */
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line) {
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */

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

/**
  * @brief System Clock Configuration
  * @retval None
  */
static void SystemClock_Config(void) {
  
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
 * in the RCC_OscInitTypeDef structure.
 */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
 */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
