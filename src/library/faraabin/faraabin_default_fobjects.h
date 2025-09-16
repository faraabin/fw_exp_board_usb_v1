/**
 ******************************************************************************
 * @file           : faraabin_default_fobjects.h
 * @brief          :
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2012-2025 FaraabinCo.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 *
 * https://faraabinco.ir/
 * https://github.com/FaraabinCo
 *
 ******************************************************************************
 * @verbatim
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_DEFAULT_FOBJECTS_H
#define FARAABIN_DEFAULT_FOBJECTS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
#if defined(FB_FEATURE_FLAG_DEFAULT_DATABUS) && defined(FARAABIN_ENABLE)
FARAABIN_DATABUS_DEF_EXTERN_(DefaultDatabus);
#endif

#if defined(FB_FEATURE_FLAG_DEFAULT_EVENT_GROUP) && defined(FARAABIN_ENABLE)
FARAABIN_EVENT_GROUP_DEF_EXTERN_(DefaultEventGroup);
#endif

FARAABIN_EVENT_GROUP_DEF_EXTERN_(NotificationEventGroup);

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_DEFAULT_FOBJECTS_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
