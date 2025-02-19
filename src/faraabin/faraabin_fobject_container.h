/**
 ******************************************************************************
 * @file           : faraabin_fobject_container.h
 * @brief          :
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 FaraabinCo.
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
#ifndef FARAABIN_FOBJECT_CONTAINER_H
#define FARAABIN_FOBJECT_CONTAINER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief container fobject definition.
 * 
 */
typedef struct {

  uint8_t _type;                          /*!< Type of the fobject. */
  
  bool _init;                             /*!< Init status of the fobject. */
  
  const char *Name;                       /*!< Name given to the fobject. */
  
  const char *Path;                       /*!< Path given to the fobject. */
  
  const char *Filename;                   /*!< Filename of the fobject. */
  
  uint8_t Seq;                            /*!< Sequence counter. */
  
  bool Enable;                            /*!< Enable status of the fobject. */
  
  void(*fpContainerFunc)(uint8_t reqSeq);  /*!< Function pointer to the container function. */
  
}sFaraabinFobjectContainer;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes a container fobject.
 * 
 * @param me Pointer to the Container.
 * @return InitStat Returns '1' if failed, '0' if successful.
 */
uint8_t fFaraabinFobjectContainer_Init(sFaraabinFobjectContainer *me);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_CONTAINER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
