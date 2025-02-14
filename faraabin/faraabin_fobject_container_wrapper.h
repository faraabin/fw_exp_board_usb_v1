/**
 ******************************************************************************
 * @file           : faraabin_fobject_container_wrapper.h
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
#ifndef FARAABIN_FOBJECT_CONTAINER_WRAPPER_H
#define FARAABIN_FOBJECT_CONTAINER_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_container.h"

#include "faraabin_config.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#ifdef FARAABIN_ENABLE

/**
 * @brief Defines a container.
 * 
 * @param containerName_ Name of the Container.
 */
#define FARAABIN_CONTAINER_DEF_(containerName_) \
  static void __FARAABIN_CONTAINER_FUNC_##containerName_(uint8_t reqSeq);\
  sFaraabinFobjectContainer containerName_ = \
  {\
    .Name = #containerName_,\
    .fpContainerFunc = __FARAABIN_CONTAINER_FUNC_##containerName_,\
  }

/**
 * @brief Defines a static container.
 * 
 * @param containerName_ Name of the Container.
 */
#define FARAABIN_CONTAINER_DEF_STATIC_(containerName_) \
  static void __FARAABIN_CONTAINER_FUNC_##containerName_(uint8_t reqSeq);\
  static sFaraabinFobjectContainer containerName_ = \
  {\
    .Name = #containerName_,\
    .fpContainerFunc = __FARAABIN_CONTAINER_FUNC_##containerName_,\
  }

/**
 * @brief Externs a container.
 * 
 * @param containerName_ Name of the Container.
 */
#define FARAABIN_CONTAINER_DEF_EXTERN_(containerName_) extern sFaraabinFobjectContainer containerName_

/**
 * @brief Function declaration of the Container.
 * 
 * @param containerName_ Name of Container.
 */
#define FARAABIN_CONTAINER_FUNC_(containerName_) \
  static void __FARAABIN_CONTAINER_FUNC_##containerName_(uint8_t reqSeq) {\
    char* __ContainerPath__ = (char*)RootPath____;\
    (void)__ContainerPath__;

/**
 * @brief Sets the path for the fobject once. When this macro is used, FARAABIN_VAR_x_DICT_(), FARAABIN_FUNCTION_GROUP_OBJECT_DICT_()
 *        and FARAABIN_FUNCTION_GROUP_DICT_() set their path to the value passed to FARAABIN_SET_PATH_().
 * 
 */
#define FARAABIN_SET_PATH_(path_) __ContainerPath__ = (path_)

/**
 * @brief Tags the end of a container function.
 * 
 * @note This macro should be placed at the end of container function.
 * 
 */
#define FARAABIN_CONTAINER_FUNC_END_ }

/**
 * @brief Initializes a fobject container.
 * 
 */
#define FARAABIN_Container_Init_WithPath_(pContainer_, path_) \
  do {\
		(pContainer_)->Path = (char*)path_;\
    (pContainer_)->Filename = FILENAME__;\
    uint8_t ret = fFaraabinFobjectContainer_Init(pContainer_);\
    (void)ret;\
  }while(0)

/**
 * @brief This is the same as FARAABIN_Container_Init_WithPath() but with default path "root".
 * 
 */
#define FARAABIN_Container_Init_(pContainer_) FARAABIN_Container_Init_WithPath_(pContainer_, RootPath____)

/**
 * @brief For support of legacy macros
 * 
 */
#define FARAABIN_DICT_GROUP_DEF_(containerName_)        FARAABIN_CONTAINER_DEF_(containerName_)
#define FARAABIN_DICT_GROUP_DEF_STATIC_(containerName_) FARAABIN_CONTAINER_DEF_STATIC_(containerName_)
#define FARAABIN_DICT_GROUP_DEF_EXTERN_(containerName_) FARAABIN_CONTAINER_DEF_EXTERN_(containerName_)
#define FARAABIN_DICT_GROUP_FUNC_(containerName_)       FARAABIN_CONTAINER_FUNC_(containerName_)
#define FARAABIN_DICT_GROUP_FUNC_END_                   FARAABIN_CONTAINER_FUNC_END_
#define FARAABIN_DictGroup_Init_WithPath_(pContainer_, path_) FARAABIN_Container_Init_WithPath_(pContainer_, path_)
#define FARAABIN_DictGroup_Init_(pContainer_)           FARAABIN_Container_Init_(pContainer_)

#endif

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_FOBJECT_CONTAINER_WRAPPER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
