/**
 ******************************************************************************
 * @file           : faraabin_dependency.h
 * @brief          : Important typedefs that are used in faraabin core modules.
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
#ifndef FARAABIN_DEPENDENCY_H
#define FARAABIN_DEPENDENCY_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_config.h"
#include "faraabin_port.h"

#include "chrono.h"
#include "chrono_ex.h"

#ifdef FB_ADD_ON_FEATURE_FLAG_STATE_MACHINE
#include "state_machine_wrapper.h"
#endif

#ifdef FB_ADD_ON_FEATURE_FLAG_CPU_PROFILER
#include "cpu_profiler_wrapper.h"
#endif

#ifdef FB_ADD_ON_FEATURE_FLAG_UNITY
#include "unity.h"
#endif

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_DEPENDENCY_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
