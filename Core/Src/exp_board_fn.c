/**
******************************************************************************
* @file           : exp_board_fn.c
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
#include "exp_board_fn.h"
#include "chrono.h"

#if defined(FB_FEATURE_FLAG_MCU_CLI) && defined(FARAABIN_ENABLE)

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
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
 * @brief 
 * 
 */
FARAABIN_FUNCTION_LOOP_(ExpBoardFn, TransmitMessage, "help") {
  
  static int cnt = 0;
  
  FARAABIN_Function_LoopStartMs_(1000) {
    
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
FARAABIN_FUNCTION_GROUP_(ExpBoardFn, "help") {
  
  FARAABIN_FUNCTION_GROUP_ADD_(ExpBoardFn, TransmitMessage);

}

#endif

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
