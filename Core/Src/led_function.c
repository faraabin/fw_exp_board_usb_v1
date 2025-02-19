/**
******************************************************************************
* @file           : led_function.c
* @brief          :
* @note           :
* @copyright      : COPYRIGHT© 2025 FaraabinCo
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright© 2025 FaraabinCo.
* All rights reserved.</center></h2>
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
* @verbatim
* @endverbatim
*/

/* Includes ------------------------------------------------------------------*/
#include "led_function.h"
#include "chrono.h"

#include "stm32f1xx_hal.h"

#if defined(FB_FEATURE_FLAG_MCU_CLI) && defined(FARAABIN_ENABLE)

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/**
 * @brief These macroes are written to control the state of the on-board LED.
 * 
 */
#define LED_ON_()   HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_RESET);
#define LED_OFF_()  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_2, GPIO_PIN_SET);

/* Private typedef -----------------------------------------------------------*/
FARAABIN_FUNCTION_GROUP_TYPE_DEF_(void*);

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                          ##### Exported Functions #####                          ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/

/**
 * @brief Faraabin function that can be called from Faraabin UI.
 * 
 */
FARAABIN_FUNCTION_(LedFunction, TurnOn, "Turns on-board LED on.") {

  LED_ON_();
  
  FARAABIN_FUNCTION_END_();
}

/**
 * @brief Faraabin function that can be called from Faraabin UI.
 * 
 */
FARAABIN_FUNCTION_(LedFunction, TurnOff, "Turns on-board LED off.") {

  LED_OFF_();
  
  FARAABIN_FUNCTION_END_();
}

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                           ##### Private Functions #####                          ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/
/**
 * @brief 
 * 
 */
FARAABIN_FUNCTION_GROUP_(LedFunction, "help") {
  
  FARAABIN_FUNCTION_GROUP_ADD_(LedFunction, TurnOn);
  FARAABIN_FUNCTION_GROUP_ADD_(LedFunction, TurnOff);

}

#endif

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
