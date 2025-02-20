/**
******************************************************************************
* @file           : faraabin_fobject_eventgroup.c
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
  @verbatim

  This fobject is used to generate and send events to the Faraabin application.
  Events can be:
  - System events, which are only used by faraabin library and user does not need to 
    utilize them.
  - Text events, that can send user defined strings to Faraabin.
  - Enumerated events, that are defined both for faraabin lib and faraabin app and are
    more efficient that sending text to the PC.
  
  In Faraabin, a default eventgroup is instantiated in fFaraabin_Init(). But users can
  declare their own eventgroups and send events specifically to those groups.

  To send events to faraabin, use macroes defined in faraabin_fobject_eventgroup_wrapper.h
  file.

  @note In Faraabin, each fobject has its own event group in addition to user-defined event groups.
        This allows users to send messages not only to event groups but also directly to the fobject.

  @endverbatim
 */

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_eventgroup.h"

#include "faraabin_fobject.h"
#include "faraabin_database.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
          ##### faraabin_fobject_event_group.c Exported Functions #####
===============================================================================*/
/**
 * @brief Initializes Faraabin event group fobject and adds its dictionary to database.
 * 
 * @param me Pointer to the event group fobject.
 * @return InitResult '1' if fails, '0' if successful.
 */
uint8_t fFaraabinFobjectEventGroup_Init(sFaraabinFobjectEventGroup *me) {
	
	if(!FaraabinInit___) {
		FaraabinFlags.Status.Bitfield.UninitializedFaraabin = 1;
		return 1;
	}

  if(fFaraabinDatabase_AddDict((uint32_t)me) != 0U) {
    return 1;
  }
  
	me->_type = (uint8_t)eFO_TYPE_EVENT_GROUP;
	
	me->Enable = true;
  me->_init = true;
  return 0;
}

/*
===============================================================================
          ##### faraabin_fobject_event_group.c Private Functions #####
===============================================================================*/


/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
