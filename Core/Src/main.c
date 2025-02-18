/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "exp_board_fn.h"

#include "chrono.h"
#include "faraabin.h"

#include "arm_math.h"
#include <math.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/**
 * @brief enum for defining generated wave type.
 * 
 */
typedef enum {

  eWAVE_TYPE_NONE = 0,
  eWAVE_TYPE_PULSE_TRAIN,
  eWAVE_TYPE_SAW_TOOTH,
  eWAVE_TYPE_SIN,

}eWaveType;

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define RUNTIME_PERIOD_US (1000)
#define OMEGA             (2.0 * (double)PI * Frequency)
  
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
static eWaveType WaveType;
static float64_t Frequency;
static float64_t Amplitude;
static float64_t Output;
static uint32_t Execution;
static uint32_t Interval;

static double t;
static double OmegaT;
static sChrono RuntimeChrono;
static sChrono FreqChrono;
static uint32_t TickBegin;
static uint32_t TickEnd;

FARAABIN_CONTAINER_DEF_STATIC_(ExpBoardContainer);
FARAABIN_DATABUS_DEF_STATIC_(ExpBoardDatabus);
FARAABIN_EVENT_GROUP_DEF_STATIC_(ExpBoardEvType);
FARAABIN_EVENT_GROUP_DEF_STATIC_(ExpBoardEvFreq);
FARAABIN_EVENT_GROUP_DEF_STATIC_(ExpBoardEvAmp);
FARAABIN_EVENT_GROUP_DEF_STATIC_(ExpBoardEvDuty);
FARAABIN_VAR_TYPE_DEF_STATIC_(eWaveType);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
static uint32_t GetTickValue(void);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM1_Init();
  MX_TIM2_Init();
  MX_USART1_UART_Init();
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN 2 */
  
  htim2.Instance->SMCR &= ~(1 << 4);
  HAL_TIM_Base_Start(&htim2);
  HAL_TIM_Base_Start(&htim1);
  if(fChrono_Init(GetTickValue) != CHRONO_OK) {
    Error_Handler();
  }
  
  if(fFaraabin_Init() != FARAABIN_OK) {
    Error_Handler();
  }
  
  FARAABIN_Container_Init_(&ExpBoardContainer);

  FARAABIN_VAR_TYPE_ENUM_INIT_(eWaveType);
  
  FARAABIN_DATABUS_SET_VALUE_(ExpBoardDatabus.ChannelQty, 5);
  FARAABIN_DATABUS_SET_VALUE_(ExpBoardDatabus.BufferCaptureSize, 100);
  FARAABIN_DATABUS_SET_VALUE_(ExpBoardDatabus.StreamDivideBy, 10);
  FARAABIN_DataBus_Init_(&ExpBoardDatabus);
  FARAABIN_DataBus_StartStreamMode_(&ExpBoardDatabus);
  FARAABIN_DataBus_AttachVariable_F64_(&ExpBoardDatabus, &Output);
  FARAABIN_DataBus_Enable_(&ExpBoardDatabus);

  FARAABIN_EventGroup_Init_(&ExpBoardEvType);
  FARAABIN_EventGroup_Init_(&ExpBoardEvAmp);
  FARAABIN_EventGroup_Init_(&ExpBoardEvFreq);
  FARAABIN_EventGroup_Init_(&ExpBoardEvDuty);

  FARAABIN_FunctionGroupType_Init_(&ExpBoardFn);

  WaveType = eWAVE_TYPE_NONE;
  Frequency = 0.0;
  Amplitude = 0.0;
  t = 0.0;
  
  fChrono_StartTimeoutUs(&RuntimeChrono, RUNTIME_PERIOD_US);
  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    
    if(fChrono_IsTimeout(&RuntimeChrono)) {

      static uint32_t lastTickBegin = 0;
      
      TickBegin = fChrono_GetTick();

      fChrono_StartTimeoutUs(&RuntimeChrono, RUNTIME_PERIOD_US);
      t = t + ((double)RUNTIME_PERIOD_US / 1000000.0);

      switch (WaveType) {
      case eWAVE_TYPE_NONE: {
        
        Output = 0;
        break;
      }
      case eWAVE_TYPE_PULSE_TRAIN: {
        
        if(fChrono_IsTimeout(&FreqChrono)) {
          
          timeUs_t interval = (timeUs_t)((1.0 / Frequency) * 1000000) / 2;
          if(Frequency == 0.0) {
            interval = 0;
          }
          fChrono_StartTimeoutUs(&FreqChrono, interval);

          static bool pulseState = false;
          pulseState = !pulseState;
          
          Output = Amplitude * (double)pulseState;
          
        }
        
        break;
      }
      case eWAVE_TYPE_SAW_TOOTH: {

        Output = Amplitude * fmod(t, (1.0 / Frequency));
        
        break;
      }
      case eWAVE_TYPE_SIN: {

        OmegaT = OMEGA * t;
        if(OmegaT > (2.0 * (double)PI)) {
          OmegaT = OmegaT - (2.0 * (double)PI);
        }

        Output = Amplitude * arm_sin_f32(OmegaT);

        break;
      }
      default: {
        Output = 0.0;
        break;
      }
      }

      static eWaveType OldType = eWAVE_TYPE_NONE;
      if(OldType != WaveType) {
        
        FARAABIN_SendEventTo_(&ExpBoardEvType, eWaveType, WaveType);
        OldType = WaveType;

      }

      static double OldAmp = 0.0;
      if(OldAmp != Amplitude) {
        
        FARAABIN_PrintfWarningTo_(&ExpBoardEvAmp, "Amplitude changed from %lf to %lf", OldAmp, Amplitude);
        OldAmp = Amplitude;

      }

      static double OldFreq = 0.0;
      if(OldFreq != Frequency) {
        
        FARAABIN_PrintfWarningTo_(&ExpBoardEvFreq, "Frequency changed from %lf to %lf", OldFreq, Frequency);
        OldFreq = Frequency;
        timeUs_t interval = (timeUs_t)((1.0 / Frequency) * 1000000) / 2;
        if(Frequency == 0.0) {
          interval = 0;
        }
        fChrono_StartTimeoutUs(&FreqChrono, interval);

      }
      
      FARAABIN_DataBus_Run_(&ExpBoardDatabus);
      
      TickEnd = fChrono_GetTick();

      Interval = fChrono_TimeSpanUs(lastTickBegin, TickBegin);
      Execution = fChrono_TimeSpanUs(TickBegin, TickEnd);
      
      lastTickBegin= TickBegin;

    }
    
    fFaraabin_Run();
    
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
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

/* USER CODE BEGIN 4 */
/**
  * @brief Returns the 32-bit tick value from timer1 and timer2 count registers.
  * @retval uint32_t CurrentTick
  */
static uint32_t GetTickValue(void) {
  
  uint16_t high = htim1.Instance->CNT;
  uint16_t low = htim2.Instance->CNT;
  
  return (high << 16) + low;
}

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
 * @brief This function is used to define all variables in the container to faraabin.
 * 
 */
FARAABIN_CONTAINER_FUNC_(ExpBoardContainer) {

  FARAABIN_VAR_ENUM_(WaveType, eWaveType);

  FARAABIN_VAR_F64_(Frequency);
  FARAABIN_VAR_F64_(Amplitude);
  FARAABIN_VAR_F64_(Output);

  FARAABIN_VAR_U32_(Execution);
  FARAABIN_VAR_U32_(Interval);

  FARAABIN_FUNCTION_GROUP_DICT_(ExpBoardFn);

  FARAABIN_CONTAINER_FUNC_END_;
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
