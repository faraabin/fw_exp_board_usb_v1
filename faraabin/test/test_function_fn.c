/**
******************************************************************************
* @file           : test_function_fn.c
* @brief          :
* @note           :
* @copyright      : COPYRIGHT© 2024 FaraabinCo
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright© 2024 FaraabinCo.
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
#include "test_function_fn.h"
#include "chrono.h"

#if defined(FB_FEATURE_FLAG_MCU_CLI) && defined(FARAABIN_ENABLE)

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
FARAABIN_FUNCTION_GROUP_TYPE_DEF_(float);

/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/*
╔══════════════════════════════════════════════════════════════════════════════════╗
║                          ##### Exported Functions #####                          ║
╚══════════════════════════════════════════════════════════════════════════════════╝
*/
/**
 * @brief 
 * 
 */
FARAABIN_FUNCTION_(TestFunctionFn, LOOP_BACK, "help") {
  
  int param;
  
  FARAABIN_Function_GetParam_(1, "%u", &param);
  
  FARAABIN_PrintfToFunction_("\r\nTestLoop: %u,", param);
  
  FARAABIN_FUNCTION_END_();
}

/**
 * @brief 
 * 
 */
FARAABIN_FUNCTION_LOOP_(TestFunctionFn, WHILE, "help") {
  
  static int cnt = 0;
  
  FARAABIN_Function_LoopStartMs_(500) {
    
    cnt++;
    FARAABIN_PrintfToFunction_("\r\nTestWhile: %u", cnt);
    
    FARAABIN_Function_LoopEnd_();
  }
  
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
FARAABIN_FUNCTION_GROUP_(TestFunctionFn, "help") {
  
  FARAABIN_FUNCTION_GROUP_ADD_(TestFunctionFn, LOOP_BACK);
  FARAABIN_FUNCTION_GROUP_ADD_(TestFunctionFn, WHILE);

}

#endif

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
