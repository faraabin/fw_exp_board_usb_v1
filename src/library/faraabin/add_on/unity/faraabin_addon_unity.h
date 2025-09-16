/**
 ******************************************************************************
 * @file           : faraabin_addon_unity.h
 * @brief          :
 * @note           :
 * @copyright      : COPYRIGHT© 2022 FaraabinCo
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright© 2022 FaraabinCo.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_ADDON_UNITY_H
#define FARAABIN_ADDON_UNITY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>

/* Exported defines ----------------------------------------------------------*/
#define FARAABIN_UNITY_OK ((uint8_t)0U)  /*!< No error. */

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
uint8_t fFaraabinAddOn_Unity_Init(void);

void fFaraabinAddOn_Unity_Char(uint8_t c);
void fFaraabinAddOn_Unity_Flush(void);
void fFaraabinAddOn_Unity_Start(void);
void fFaraabinAddOn_Unity_Complete(void);

void fFaraabinAddOn_Unity_ExeTimeStart(void);
void fFaraabinAddOn_Unity_ExeTimeStop(void);
void fFaraabinAddOn_Unity_ExeTimePrint(void);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __FARAABIN_ADDON_UNITY_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
