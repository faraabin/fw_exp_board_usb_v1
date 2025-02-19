/**
 ******************************************************************************
 * @file           : faraabin_fobject_eventgroup.h
 * @brief          : Faraabin event group fobject header file.
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
#ifndef FARAABIN_FOBJECT_EVENTGROUP_H
#define FARAABIN_FOBJECT_EVENTGROUP_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief Event group system events.
 * 
 */
typedef enum {
  
  eEG_EVENT_INFO_USER_DATA_RECEIVED = 0,
  
  eEG_EVENT_ERROR_UNSUPPORTED_FOBJECT_PROPERTY,
  
}eFaraabinFobjectEventGroup_SystemEventId;

/**
 * @brief Faraabin event group fobject definition.
 * 
 */
typedef struct {

  uint8_t _type;                                                            /*!< Type of the fobject. */
  
  bool _init;                                                               /*!< Init status of the fobject. */
	
	bool Enable;                                                              /*!< Enable of the fobject. */
  
  const char *Name;                                                         /*!< Name given to the fobject. */
  
  const char *Path;                                                         /*!< Path given to the fobject. */
  
  const char *Filename;                                                     /*!< Filename of the fobject. */
  
  uint8_t Seq;                                                              /*!< Sequence counter. */
  
  void(*fpUserTerminalCallback)(uint8_t *userData, uint16_t userDataSize);  /*!< Function pointer for user terminal callback. */
  
}sFaraabinFobjectEventGroup;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes Faraabin event group fobject and adds its dictionary to database.
 * 
 * @param me Pointer to the event group fobject.
 * @return InitResult '1' if failed, '0' if successful.
 */
uint8_t fFaraabinFobjectEventGroup_Init(sFaraabinFobjectEventGroup *me);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_EVENTGROUP_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
