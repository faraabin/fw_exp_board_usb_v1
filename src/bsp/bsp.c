/**
 ******************************************************************************
 * @file           : bsp.c
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
#include "bsp.h"

#include "chrono.h"

#include "stm32f1xx_hal.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

#include <stdbool.h>

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static bool Init;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
static uint8_t fBspInitLed(void);

/* Variables -----------------------------------------------------------------*/

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                          ##### Exported Functions #####                          ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/

/**
 * @brief Initializes MCU and board peripherals.
 * 
 * @return initStatus One of the Bsp function return types.
 */
uint8_t fBsp_Init(void) {

  Init = false;

  /* Reset all peripherals, initialize the Flash interface and the Systick. */
  HAL_Init();
  /* Configure the system clock. */
  SystemClock_Config();
  /* Initialize all configured peripherals. */
  fBspInitLed();
  MX_USB_DEVICE_Init();

  Init = true;
  return BSP_OK;
}

/**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t fBsp_TickInit(void) {
  return 0;
}

/**
 * @brief 
 * 
 * @return uint32_t 
 */
uint32_t fBsp_GetTick(void) {
  return SysTick->VAL;
}

/**
 * @brief Turns on-board LED on.
 * 
 * @return status One of the Bsp function return types.
 */
uint8_t fBsp_Led_On(void) {

  if(!Init) {
    return BSP_NOT_INIT;
  }

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);

  return BSP_OK;
}

/**
 * @brief Turns on-board LED off.
 * 
 * @return status One of the Bsp function return types.
 */
uint8_t fBsp_Led_Off(void) {

  if(!Init) {
    return BSP_NOT_INIT;
  }

  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

  return BSP_OK;
}

/**
 * @brief 
 * 
 * @param data 
 * @param size 
 * @return uint8_t 
 */
uint8_t fBsp_VCP_Send(uint8_t *data, uint16_t size) {

  if(!Init) {
    return BSP_NOT_INIT;
  }

  if(CDC_Transmit_FS(data, size) != USBD_OK) {
    return BSP_USB_ERROR;
  }

  return BSP_OK;
}

/**
 * @brief 
 * 
 * @param fptr 
 * @return uint8_t 
 */
uint8_t fBsp_VCP_RegisterFrameReceivedCallback(void(*fptr)(uint8_t *data, uint16_t size)) {

  if(!Init) {
    return BSP_NOT_INIT;
  }

  if(fptr == NULL) {
    return BSP_NULL_REF_ERROR;
  }

  if(CDC_RegisterReceiveCallback_FS(fptr) != USBD_OK) {
    return BSP_USB_ERROR;
  }

  return BSP_OK;
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

    fBsp_Led_Off();
    HAL_Delay(250);
    fBsp_Led_On();
    HAL_Delay(250);

  }
}

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                            ##### Private Functions #####                         ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/

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

/**
 * @brief 
 * 
 * @return uint8_t 
 */
static uint8_t fBspInitLed(void) {

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA2 */
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  return 0;
}

/**
 * @brief this function is defined here to prevent enabling systick timer.
 * 
 * @param TickPriority 
 * @return HAL_StatusTypeDef 
 */
HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority) {

  SysTick->LOAD  = (uint32_t)(0xFFFFFF - 1UL);                      /* set reload register */
  SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk;                        /* Enable SysTick IRQ and SysTick Timer */
  return HAL_OK;
}

/**
 * @brief This function provides minimum delay (in milliseconds) based
 *        on variable incremented.
 * @param Delay  specifies the delay time length, in milliseconds.
 * @retval None
 */
void HAL_Delay(volatile uint32_t Delay) {

  fChrono_DelayMs(Delay);
}

/**
 * @brief Provide a tick value in millisecond.
 * @retval tick value
 */
uint32_t HAL_GetTick(void) {

  return (uint32_t)fChrono_GetContinuousTickMs();
}

/**
  * Initializes the Global MSP.
  */
void HAL_MspInit(void)
{
  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

  /** ENABLE: Full SWJ (JTAG-DP + SW-DP): Reset State
  */
  __HAL_AFIO_REMAP_SWJ_ENABLE();
}

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
