/**
 ******************************************************************************
 * @file           : faraabin_fobject_var_wrapper.h
 * @brief          : Wrappers for declaring variables inside a container.
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
#ifndef FARAABIN_FOBJECT_VAR_WRAPPER_H
#define FARAABIN_FOBJECT_VAR_WRAPPER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_fobject_var.h"

#include "faraabin_config.h"

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
#if defined(FARAABIN_ENABLE)

/**
 * @brief Adds a bool variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_BOOL_CB_WP_(varName_, fpAccessCb_, path_)         FARAABIN_VAR_DICT_(bool_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_BOOL_(varName_)                                   FARAABIN_VAR_BOOL_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_BOOL_CB_(varName_, fpAccessCb_)                   FARAABIN_VAR_BOOL_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_BOOL_WP_(varName_, path_)                         FARAABIN_VAR_BOOL_DICT_CB_WP_(varName_, 0, path_)
  
#define FARAABIN_VAR_BOOL_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)   FARAABIN_VAR_DICT_(bool_t, varName_, varName_, (sizeof(varName_) / sizeof(bool)), false, fpAccessCb_, path_)
#define FARAABIN_VAR_BOOL_ARRAY_(varName_)                             FARAABIN_VAR_BOOL_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_BOOL_ARRAY_CB_(varName_, fpAccessCb_)             FARAABIN_VAR_BOOL_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_BOOL_ARRAY_WP_(varName_, path_)                   FARAABIN_VAR_BOOL_ARRAY_DICT_CB_WP_(varName_, 0, path_)



/**
 * @brief Adds a uint8_t variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_U8_CB_WP_(varName_, fpAccessCb_, path_)           FARAABIN_VAR_DICT_(uint8_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_U8_(varName_)                                     FARAABIN_VAR_U8_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_U8_CB_(varName_, fpAccessCb_)                     FARAABIN_VAR_U8_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_U8_WP_(varName_, path_)                           FARAABIN_VAR_U8_DICT_CB_WP_(varName_, 0, path_)

#define FARAABIN_VAR_U8_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)     FARAABIN_VAR_ARRAY_DICT_(uint8_t, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_U8_ARRAY_(varName_)                               FARAABIN_VAR_U8_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_U8_ARRAY_CB_(varName_, fpAccessCb_)               FARAABIN_VAR_U8_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_U8_ARRAY_WP_(varName_, path_)                     FARAABIN_VAR_U8_ARRAY_DICT_CB_WP_(varName_, 0, path_)



/**
 * @brief Adds a int8_t variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_I8_CB_WP_(varName_, fpAccessCb_, path_)           FARAABIN_VAR_DICT_(int8_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_I8_(varName_)                                     FARAABIN_VAR_I8_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_I8_CB_(varName_, fpAccessCb_)                     FARAABIN_VAR_I8_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_I8_WP_(varName_, path_)                           FARAABIN_VAR_I8_DICT_CB_WP_(varName_, 0, path_)

#define FARAABIN_VAR_I8_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)     FARAABIN_VAR_ARRAY_DICT_(int8_t, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_I8_ARRAY_(varName_)                               FARAABIN_VAR_I8_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_I8_ARRAY_CB_(varName_, fpAccessCb_)               FARAABIN_VAR_I8_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_I8_ARRAY_WP_(varName_, path_)                     FARAABIN_VAR_I8_ARRAY_DICT_CB_WP_(varName_, 0, path_)



/**
 * @brief Adds a uint16_t variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_U16_CB_WP_(varName_, fpAccessCb_, path_)          FARAABIN_VAR_DICT_(uint16_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_U16_(varName_)                                    FARAABIN_VAR_U16_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_U16_CB_(varName_, fpAccessCb_)                    FARAABIN_VAR_U16_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_U16_WP_(varName_, path_)                          FARAABIN_VAR_U16_DICT_CB_WP_(varName_, 0, path_)

#define FARAABIN_VAR_U16_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)    FARAABIN_VAR_ARRAY_DICT_(uint16_t, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_U16_ARRAY_(varName_)                              FARAABIN_VAR_U16_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_U16_ARRAY_CB_(varName_, fpAccessCb_)              FARAABIN_VAR_U16_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_U16_ARRAY_WP_(varName_, path_)                    FARAABIN_VAR_U16_ARRAY_DICT_CB_WP_(varName_, 0, path_)



/**
 * @brief Adds a int16_t variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_I16_CB_WP_(varName_, fpAccessCb_, path_)          FARAABIN_VAR_DICT_(int16_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_I16_(varName_)                                    FARAABIN_VAR_I16_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_I16_CB_(varName_, fpAccessCb_)                    FARAABIN_VAR_I16_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_I16_WP_(varName_, path_)                          FARAABIN_VAR_I16_DICT_CB_WP_(varName_, 0, path_)

#define FARAABIN_VAR_I16_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)    FARAABIN_VAR_ARRAY_DICT_(int16_t, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_I16_ARRAY_(varName_)                              FARAABIN_VAR_I16_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_I16_ARRAY_CB_(varName_, fpAccessCb_)              FARAABIN_VAR_I16_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_I16_ARRAY_WP_(varName_, path_)                    FARAABIN_VAR_I16_ARRAY_DICT_CB_WP_(varName_, 0, path_)



/**
 * @brief Adds a uint32_t variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_U32_CB_WP_(varName_, fpAccessCb_, path_)          FARAABIN_VAR_DICT_(uint32_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_U32_(varName_)                                    FARAABIN_VAR_U32_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_U32_CB_(varName_, fpAccessCb_)                    FARAABIN_VAR_U32_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_U32_WP_(varName_, path_)                          FARAABIN_VAR_U32_DICT_CB_WP_(varName_, 0, path_)

#define FARAABIN_VAR_U32_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)    FARAABIN_VAR_ARRAY_DICT_(uint32_t, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_U32_ARRAY_(varName_)                              FARAABIN_VAR_U32_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_U32_ARRAY_CB_(varName_, fpAccessCb_)              FARAABIN_VAR_U32_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_U32_ARRAY_WP_(varName_, path_)                    FARAABIN_VAR_U32_ARRAY_DICT_CB_WP_(varName_, 0, path_)



/**
 * @brief Adds a int32_t variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_I32_CB_WP_(varName_, fpAccessCb_, path_)          FARAABIN_VAR_DICT_(int32_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_I32_(varName_)                                    FARAABIN_VAR_I32_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_I32_CB_(varName_, fpAccessCb_)                    FARAABIN_VAR_I32_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_I32_WP_(varName_, path_)                          FARAABIN_VAR_I32_DICT_CB_WP_(varName_, 0, path_)

#define FARAABIN_VAR_I32_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)    FARAABIN_VAR_ARRAY_DICT_(int32_t, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_I32_ARRAY_(varName_)                              FARAABIN_VAR_I32_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_I32_ARRAY_CB_(varName_, fpAccessCb_)              FARAABIN_VAR_I32_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_I32_ARRAY_WP_(varName_, path_)                    FARAABIN_VAR_I32_ARRAY_DICT_CB_WP_(varName_, 0, path_)



/**
 * @brief Adds a uint64_t variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_U64_CB_WP_(varName_, fpAccessCb_, path_)          FARAABIN_VAR_DICT_(uint64_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_U64_(varName_)                                    FARAABIN_VAR_U64_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_U64_CB_(varName_, fpAccessCb_)                    FARAABIN_VAR_U64_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_U64_WP_(varName_, path_)                          FARAABIN_VAR_U64_DICT_CB_WP_(varName_, 0, path_)

#define FARAABIN_VAR_U64_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)    FARAABIN_VAR_ARRAY_DICT_(uint64_t, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_U64_ARRAY_(varName_)                              FARAABIN_VAR_U64_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_U64_ARRAY_CB_(varName_, fpAccessCb_)              FARAABIN_VAR_U64_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_U64_ARRAY_WP_(varName_, path_)                    FARAABIN_VAR_U64_ARRAY_DICT_CB_WP_(varName_, 0, path_)




/**
 * @brief Adds a int64_t variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_I64_CB_WP_(varName_, fpAccessCb_, path_)          FARAABIN_VAR_DICT_(int64_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_I64_(varName_)                                    FARAABIN_VAR_I64_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_I64_CB_(varName_, fpAccessCb_)                    FARAABIN_VAR_I64_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_I64_WP_(varName_, path_)                          FARAABIN_VAR_I64_DICT_CB_WP_(varName_, 0, path_)

#define FARAABIN_VAR_I64_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)    FARAABIN_VAR_ARRAY_DICT_(int64_t, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_I64_ARRAY_(varName_)                              FARAABIN_VAR_I64_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_I64_ARRAY_CB_(varName_, fpAccessCb_)              FARAABIN_VAR_I64_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_I64_ARRAY_WP_(varName_, path_)                    FARAABIN_VAR_I64_ARRAY_DICT_CB_WP_(varName_, 0, path_)



/**
 * @brief Adds a float32_t variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_F32_CB_WP_(varName_, fpAccessCb_, path_)          FARAABIN_VAR_DICT_(float32_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_F32_(varName_)                                    FARAABIN_VAR_F32_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_F32_CB_(varName_, fpAccessCb_)                    FARAABIN_VAR_F32_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_F32_WP_(varName_, path_)                          FARAABIN_VAR_F32_DICT_CB_WP_(varName_, 0, path_)

#define FARAABIN_VAR_F32_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)    FARAABIN_VAR_ARRAY_DICT_(float32_t, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_F32_ARRAY_(varName_)                              FARAABIN_VAR_F32_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_F32_ARRAY_CB_(varName_, fpAccessCb_)              FARAABIN_VAR_F32_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_F32_ARRAY_WP_(varName_, path_)                    FARAABIN_VAR_F32_ARRAY_DICT_CB_WP_(varName_, 0, path_)



/**
 * @brief Adds a float64_t variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_F64_CB_WP_(varName_, fpAccessCb_, path_)          FARAABIN_VAR_DICT_(float64_t, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_F64_(varName_)                                    FARAABIN_VAR_F64_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_F64_CB_(varName_, fpAccessCb_)                    FARAABIN_VAR_F64_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_F64_WP_(varName_, path_)                          FARAABIN_VAR_F64_DICT_CB_WP_(varName_, 0, path_)

#define FARAABIN_VAR_F64_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)    FARAABIN_VAR_ARRAY_DICT_(float64_t, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_F64_ARRAY_(varName_)                              FARAABIN_VAR_F64_ARRAY_DICT_CB_WP_(varName_, 0, __ContainerPath__)
#define FARAABIN_VAR_F64_ARRAY_CB_(varName_, fpAccessCb_)              FARAABIN_VAR_F64_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_F64_ARRAY_WP_(varName_, path_)                    FARAABIN_VAR_F64_ARRAY_DICT_CB_WP_(varName_, 0, path_)



/**
 * @brief Adds a enum variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_ENUM_CB_WP_(varName_, type_, fpAccessCb_, path_)        FARAABIN_VAR_DICT_(type_, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_ENUM_(varName_, type_)                                  FARAABIN_VAR_ENUM_DICT_CB_WP_(varName_, type_, 0, __ContainerPath__)
#define FARAABIN_VAR_ENUM_CB_(varName_, type_, fpAccessCb_)                  FARAABIN_VAR_ENUM_DICT_CB_WP_(varName_, type_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_ENUM_WP_(varName_, type_, path_)                        FARAABIN_VAR_ENUM_DICT_CB_WP_(varName_, type_, 0, path_)

#define FARAABIN_VAR_ENUM_ARRAY_CB_WP_(varName_, type_, fpAccessCb_, path_)  FARAABIN_VAR_ARRAY_DICT_(type_, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_ENUM_ARRAY_(varName_, type_)                            FARAABIN_VAR_ENUM_ARRAY_DICT_CB_WP_(varName_, type_, 0, __ContainerPath__)
#define FARAABIN_VAR_ENUM_ARRAY_CB_(varName_, type_, fpAccessCb_)            FARAABIN_VAR_ENUM_ARRAY_DICT_CB_WP_(varName_, type_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_ENUM_ARRAY_WP_(varName_, type_, path_)                  FARAABIN_VAR_ENUM_ARRAY_DICT_CB_WP_(varName_, type_, 0, path_)



/**
 * @brief Adds a struct variable to the fobject container.
 *        Macros ending with CB_WP_ require the variable name, path, and a pointer to the access callback function.
 *        Macros ending with CB_ take the variable name along with a pointer to a function for handling read/write access. The path is set in container using FARAABIN_SET_PATH_()
 *        Macros ending with WP_ require both the variable name and its specified path.
 * @note The access callback must be a pointer to uint8_t(*FaraabinVarAccessCallback)(eFaraabinVarAccessType accessType, uint32_t varPtr, uint8_t *data, uint16_t size).
 * @param varName_ The name of the variable.
 * @param fpAccessCb_ Pointer to the access callback function.
 * @param path_ The path associated with the variable fobject.
*/
#define FARAABIN_VAR_STRUCT_CB_WP_(varName_, type_, fpAccessCb_, path_)        FARAABIN_VAR_DICT_(type_, varName_, &varName_, 1, false, fpAccessCb_, path_)
#define FARAABIN_VAR_STRUCT_(varName_, type_)                                  FARAABIN_VAR_STRUCT_DICT_CB_WP_(varName_, type_, 0, __ContainerPath__)
#define FARAABIN_VAR_STRUCT_CB_(varName_, type_, fpAccessCb_)                  FARAABIN_VAR_STRUCT_DICT_CB_WP_(varName_, type_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_STRUCT_WP_(varName_, type_, path_)                        FARAABIN_VAR_STRUCT_DICT_CB_WP_(varName_, type_, 0, path_)

#define FARAABIN_VAR_STRUCT_ARRAY_CB_WP_(varName_, type_, fpAccessCb_, path_)  FARAABIN_VAR_ARRAY_DICT_(type_, varName_, varName_, false, fpAccessCb_, path_)
#define FARAABIN_VAR_STRUCT_ARRAY_(varName_, type_)                            FARAABIN_VAR_STRUCT_ARRAY_DICT_CB_WP_(varName_, type_, 0, __ContainerPath__)
#define FARAABIN_VAR_STRUCT_ARRAY_CB_(varName_, type_, fpAccessCb_)            FARAABIN_VAR_STRUCT_ARRAY_DICT_CB_WP_(varName_, type_, fpAccessCb_, __ContainerPath__)
#define FARAABIN_VAR_STRUCT_ARRAY_WP_(varName_, type_, path_)                  FARAABIN_VAR_STRUCT_ARRAY_DICT_CB_WP_(varName_, type_, 0, path_)



/**
 * @brief This is the main macro for defining variables in containers.
 *        User is advised to utilize specific wrappers for variables with types provided for them
 *        in this file.
 * 
 */
#define FARAABIN_VAR_DICT_(varType_, varName_, pVar_, arrayQty_, isPtr_, fpAccessCb_, path_) \
  fFaraabinLinkSerializer_VarDict(\
  (uint32_t)&__faraabin_vartype_##varType_,\
  (uint32_t)pVar_,\
  arrayQty_,\
  (uint32_t)fpAccessCb_,\
	0,\
  isPtr_,\
  #varName_,\
  path_,\
  FILENAME__,\
  reqSeq);\

/**
 * @brief This is the main macro for defining arrays in containers.
 *        User is advised to utilize specific wrappers for array with types provided for them
 *        in this file.
 * 
 */
#define FARAABIN_VAR_ARRAY_DICT_(varType_, varName_, pVar_, isPtr_, fpAccessCb_, path_) \
  FARAABIN_VAR_DICT_(varType_, varName_, pVar_, (sizeof(varName_) / sizeof(varType_)), isPtr_, fpAccessCb_, path_)

/**
 * @brief Support legacy macros
 * 
 */
#define FARAABIN_VAR_BOOL_DICT_CB_WP_(varName_, fpAccessCb_, path_)                 FARAABIN_VAR_BOOL_CB_WP_(varName_, fpAccessCb_, path_)     
#define FARAABIN_VAR_BOOL_DICT_(varName_)                                           FARAABIN_VAR_BOOL_(varName_)                               
#define FARAABIN_VAR_BOOL_DICT_CB_(varName_, fpAccessCb_)                           FARAABIN_VAR_BOOL_CB_(varName_, fpAccessCb_)               
#define FARAABIN_VAR_BOOL_DICT_WP_(varName_, path_)                                 FARAABIN_VAR_BOOL_WP_(varName_, path_)                     

#define FARAABIN_VAR_BOOL_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)           FARAABIN_VAR_BOOL_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_BOOL_ARRAY_DICT_(varName_)                                     FARAABIN_VAR_BOOL_ARRAY_(varName_)                         
#define FARAABIN_VAR_BOOL_ARRAY_DICT_CB_(varName_, fpAccessCb_)                     FARAABIN_VAR_BOOL_ARRAY_CB_(varName_, fpAccessCb_)         
#define FARAABIN_VAR_BOOL_ARRAY_DICT_WP_(varName_, path_)                           FARAABIN_VAR_BOOL_ARRAY_WP_(varName_, path_)               

#define FARAABIN_VAR_U8_DICT_CB_WP_(varName_, fpAccessCb_, path_)                   FARAABIN_VAR_U8_CB_WP_(varName_, fpAccessCb_, path_)       
#define FARAABIN_VAR_U8_DICT_(varName_)                                             FARAABIN_VAR_U8_(varName_)                                 
#define FARAABIN_VAR_U8_DICT_CB_(varName_, fpAccessCb_)                             FARAABIN_VAR_U8_CB_(varName_, fpAccessCb_)                 
#define FARAABIN_VAR_U8_DICT_WP_(varName_, path_)                                   FARAABIN_VAR_U8_WP_(varName_, path_)                       

#define FARAABIN_VAR_U8_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)             FARAABIN_VAR_U8_ARRAY_CB_WP_(varName_, fpAccessCb_, path_) 
#define FARAABIN_VAR_U8_ARRAY_DICT_(varName_)                                       FARAABIN_VAR_U8_ARRAY_(varName_)                           
#define FARAABIN_VAR_U8_ARRAY_DICT_CB_(varName_, fpAccessCb_)                       FARAABIN_VAR_U8_ARRAY_CB_(varName_, fpAccessCb_)           
#define FARAABIN_VAR_U8_ARRAY_DICT_WP_(varName_, path_)                             FARAABIN_VAR_U8_ARRAY_WP_(varName_, path_)                 

#define FARAABIN_VAR_I8_DICT_CB_WP_(varName_, fpAccessCb_, path_)                   FARAABIN_VAR_I8_CB_WP_(varName_, fpAccessCb_, path_)       
#define FARAABIN_VAR_I8_DICT_(varName_)                                             FARAABIN_VAR_I8_(varName_)                                 
#define FARAABIN_VAR_I8_DICT_CB_(varName_, fpAccessCb_)                             FARAABIN_VAR_I8_CB_(varName_, fpAccessCb_)                 
#define FARAABIN_VAR_I8_DICT_WP_(varName_, path_)                                   FARAABIN_VAR_I8_WP_(varName_, path_)                       

#define FARAABIN_VAR_I8_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)             FARAABIN_VAR_I8_ARRAY_CB_WP_(varName_, fpAccessCb_, path_) 
#define FARAABIN_VAR_I8_ARRAY_DICT_(varName_)                                       FARAABIN_VAR_I8_ARRAY_(varName_)                           
#define FARAABIN_VAR_I8_ARRAY_DICT_CB_(varName_, fpAccessCb_)                       FARAABIN_VAR_I8_ARRAY_CB_(varName_, fpAccessCb_)           
#define FARAABIN_VAR_I8_ARRAY_DICT_WP_(varName_, path_)                             FARAABIN_VAR_I8_ARRAY_WP_(varName_, path_)                 

#define FARAABIN_VAR_U16_DICT_CB_WP_(varName_, fpAccessCb_, path_)                  FARAABIN_VAR_U16_CB_WP_(varName_, fpAccessCb_, path_)      
#define FARAABIN_VAR_U16_DICT_(varName_)                                            FARAABIN_VAR_U16_(varName_)                                
#define FARAABIN_VAR_U16_DICT_CB_(varName_, fpAccessCb_)                            FARAABIN_VAR_U16_CB_(varName_, fpAccessCb_)                
#define FARAABIN_VAR_U16_DICT_WP_(varName_, path_)                                  FARAABIN_VAR_U16_WP_(varName_, path_)                      

#define FARAABIN_VAR_U16_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)            FARAABIN_VAR_U16_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U16_ARRAY_DICT_(varName_)                                      FARAABIN_VAR_U16_ARRAY_(varName_)                          
#define FARAABIN_VAR_U16_ARRAY_DICT_CB_(varName_, fpAccessCb_)                      FARAABIN_VAR_U16_ARRAY_CB_(varName_, fpAccessCb_)          
#define FARAABIN_VAR_U16_ARRAY_DICT_WP_(varName_, path_)                            FARAABIN_VAR_U16_ARRAY_WP_(varName_, path_)                

#define FARAABIN_VAR_I16_DICT_CB_WP_(varName_, fpAccessCb_, path_)                  FARAABIN_VAR_I16_CB_WP_(varName_, fpAccessCb_, path_)      
#define FARAABIN_VAR_I16_DICT_(varName_)                                            FARAABIN_VAR_I16_(varName_)                                
#define FARAABIN_VAR_I16_DICT_CB_(varName_, fpAccessCb_)                            FARAABIN_VAR_I16_CB_(varName_, fpAccessCb_)                
#define FARAABIN_VAR_I16_DICT_WP_(varName_, path_)                                  FARAABIN_VAR_I16_WP_(varName_, path_)                      

#define FARAABIN_VAR_I16_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)            FARAABIN_VAR_I16_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I16_ARRAY_DICT_(varName_)                                      FARAABIN_VAR_I16_ARRAY_(varName_)                          
#define FARAABIN_VAR_I16_ARRAY_DICT_CB_(varName_, fpAccessCb_)                      FARAABIN_VAR_I16_ARRAY_CB_(varName_, fpAccessCb_)          
#define FARAABIN_VAR_I16_ARRAY_DICT_WP_(varName_, path_)                            FARAABIN_VAR_I16_ARRAY_WP_(varName_, path_)                

#define FARAABIN_VAR_U32_DICT_CB_WP_(varName_, fpAccessCb_, path_)                  FARAABIN_VAR_U32_CB_WP_(varName_, fpAccessCb_, path_)      
#define FARAABIN_VAR_U32_DICT_(varName_)                                            FARAABIN_VAR_U32_(varName_)                                
#define FARAABIN_VAR_U32_DICT_CB_(varName_, fpAccessCb_)                            FARAABIN_VAR_U32_CB_(varName_, fpAccessCb_)         
#define FARAABIN_VAR_U32_DICT_WP_(varName_, path_)                                  FARAABIN_VAR_U32_WP_(varName_, path_)                      

#define FARAABIN_VAR_U32_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)            FARAABIN_VAR_U32_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U32_ARRAY_DICT_(varName_)                                      FARAABIN_VAR_U32_ARRAY_(varName_)                          
#define FARAABIN_VAR_U32_ARRAY_DICT_CB_(varName_, fpAccessCb_)                      FARAABIN_VAR_U32_ARRAY_CB_(varName_, fpAccessCb_)          
#define FARAABIN_VAR_U32_ARRAY_DICT_WP_(varName_, path_)                            FARAABIN_VAR_U32_ARRAY_WP_(varName_, path_)                

#define FARAABIN_VAR_I32_DICT_CB_WP_(varName_, fpAccessCb_, path_)                  FARAABIN_VAR_I32_CB_WP_(varName_, fpAccessCb_, path_)      
#define FARAABIN_VAR_I32_DICT_(varName_)                                            FARAABIN_VAR_I32_(varName_)                                
#define FARAABIN_VAR_I32_DICT_CB_(varName_, fpAccessCb_)                            FARAABIN_VAR_I32_CB_(varName_, fpAccessCb_)                
#define FARAABIN_VAR_I32_DICT_WP_(varName_, path_)                                  FARAABIN_VAR_I32_WP_(varName_, path_)                      

#define FARAABIN_VAR_I32_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)            FARAABIN_VAR_I32_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I32_ARRAY_DICT_(varName_)                                      FARAABIN_VAR_I32_ARRAY_(varName_)                          
#define FARAABIN_VAR_I32_ARRAY_DICT_CB_(varName_, fpAccessCb_)                      FARAABIN_VAR_I32_ARRAY_CB_(varName_, fpAccessCb_)          
#define FARAABIN_VAR_I32_ARRAY_DICT_WP_(varName_, path_)                            FARAABIN_VAR_I32_ARRAY_WP_(varName_, path_)                

#define FARAABIN_VAR_U64_DICT_CB_WP_(varName_, fpAccessCb_, path_)                  FARAABIN_VAR_U64_CB_WP_(varName_, fpAccessCb_, path_)      
#define FARAABIN_VAR_U64_DICT_(varName_)                                            FARAABIN_VAR_U64_(varName_)                                
#define FARAABIN_VAR_U64_DICT_CB_(varName_, fpAccessCb_)                            FARAABIN_VAR_U64_CB_(varName_, fpAccessCb_)                
#define FARAABIN_VAR_U64_DICT_WP_(varName_, path_)                                  FARAABIN_VAR_U64_WP_(varName_, path_)                      

#define FARAABIN_VAR_U64_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)            FARAABIN_VAR_U64_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_U64_ARRAY_DICT_(varName_)                                      FARAABIN_VAR_U64_ARRAY_(varName_)                          
#define FARAABIN_VAR_U64_ARRAY_DICT_CB_(varName_, fpAccessCb_)                      FARAABIN_VAR_U64_ARRAY_CB_(varName_, fpAccessCb_)          
#define FARAABIN_VAR_U64_ARRAY_DICT_WP_(varName_, path_)                            FARAABIN_VAR_U64_ARRAY_WP_(varName_, path_)                

#define FARAABIN_VAR_I64_DICT_CB_WP_(varName_, fpAccessCb_, path_)                  FARAABIN_VAR_I64_CB_WP_(varName_, fpAccessCb_, path_)      
#define FARAABIN_VAR_I64_DICT_(varName_)                                            FARAABIN_VAR_I64_(varName_)                                
#define FARAABIN_VAR_I64_DICT_CB_(varName_, fpAccessCb_)                            FARAABIN_VAR_I64_CB_(varName_, fpAccessCb_)                
#define FARAABIN_VAR_I64_DICT_WP_(varName_, path_)                                  FARAABIN_VAR_I64_WP_(varName_, path_)                      

#define FARAABIN_VAR_I64_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)            FARAABIN_VAR_I64_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_I64_ARRAY_DICT_(varName_)                                      FARAABIN_VAR_I64_ARRAY_(varName_)                          
#define FARAABIN_VAR_I64_ARRAY_DICT_CB_(varName_, fpAccessCb_)                      FARAABIN_VAR_I64_ARRAY_CB_(varName_, fpAccessCb_)          
#define FARAABIN_VAR_I64_ARRAY_DICT_WP_(varName_, path_)                            FARAABIN_VAR_I64_ARRAY_WP_(varName_, path_)                

#define FARAABIN_VAR_F32_DICT_CB_WP_(varName_, fpAccessCb_, path_)                  FARAABIN_VAR_F32_CB_WP_(varName_, fpAccessCb_, path_)      
#define FARAABIN_VAR_F32_DICT_(varName_)                                            FARAABIN_VAR_F32_(varName_)                                
#define FARAABIN_VAR_F32_DICT_CB_(varName_, fpAccessCb_)                            FARAABIN_VAR_F32_CB_(varName_, fpAccessCb_)                
#define FARAABIN_VAR_F32_DICT_WP_(varName_, path_)                                  FARAABIN_VAR_F32_WP_(varName_, path_)                      

#define FARAABIN_VAR_F32_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)            FARAABIN_VAR_F32_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_F32_ARRAY_DICT_(varName_)                                      FARAABIN_VAR_F32_ARRAY_(varName_)                          
#define FARAABIN_VAR_F32_ARRAY_DICT_CB_(varName_, fpAccessCb_)                      FARAABIN_VAR_F32_ARRAY_CB_(varName_, fpAccessCb_)          
#define FARAABIN_VAR_F32_ARRAY_DICT_WP_(varName_, path_)                            FARAABIN_VAR_F32_ARRAY_WP_(varName_, path_)                

#define FARAABIN_VAR_F64_DICT_CB_WP_(varName_, fpAccessCb_, path_)                  FARAABIN_VAR_F64_CB_WP_(varName_, fpAccessCb_, path_)      
#define FARAABIN_VAR_F64_DICT_(varName_)                                            FARAABIN_VAR_F64_(varName_)                                
#define FARAABIN_VAR_F64_DICT_CB_(varName_, fpAccessCb_)                            FARAABIN_VAR_F64_CB_(varName_, fpAccessCb_)                
#define FARAABIN_VAR_F64_DICT_WP_(varName_, path_)                                  FARAABIN_VAR_F64_WP_(varName_, path_)                      

#define FARAABIN_VAR_F64_ARRAY_DICT_CB_WP_(varName_, fpAccessCb_, path_)            FARAABIN_VAR_F64_ARRAY_CB_WP_(varName_, fpAccessCb_, path_)
#define FARAABIN_VAR_F64_ARRAY_DICT_(varName_)                                      FARAABIN_VAR_F64_ARRAY_(varName_)                          
#define FARAABIN_VAR_F64_ARRAY_DICT_CB_(varName_, fpAccessCb_)                      FARAABIN_VAR_F64_ARRAY_CB_(varName_, fpAccessCb_)          
#define FARAABIN_VAR_F64_ARRAY_DICT_WP_(varName_, path_)                            FARAABIN_VAR_F64_ARRAY_WP_(varName_, path_)                

#define FARAABIN_VAR_ENUM_DICT_CB_WP_(varName_, type_, fpAccessCb_, path_)          FARAABIN_VAR_ENUM_CB_WP_(varName_, type_, fpAccessCb_, path_)      
#define FARAABIN_VAR_ENUM_DICT_(varName_, type_)                                    FARAABIN_VAR_ENUM_(varName_, type_)                                
#define FARAABIN_VAR_ENUM_DICT_CB_(varName_, type_, fpAccessCb_)                    FARAABIN_VAR_ENUM_CB_(varName_, type_, fpAccessCb_)                
#define FARAABIN_VAR_ENUM_DICT_WP_(varName_, type_, path_)                          FARAABIN_VAR_ENUM_WP_(varName_, type_, path_)                      

#define FARAABIN_VAR_ENUM_ARRAY_DICT_CB_WP_(varName_, type_, fpAccessCb_, path_)    FARAABIN_VAR_ENUM_ARRAY_CB_WP_(varName_, type_, fpAccessCb_, path_)
#define FARAABIN_VAR_ENUM_ARRAY_DICT_(varName_, type_)                              FARAABIN_VAR_ENUM_ARRAY_(varName_, type_)                          
#define FARAABIN_VAR_ENUM_ARRAY_DICT_CB_(varName_, type_, fpAccessCb_)              FARAABIN_VAR_ENUM_ARRAY_CB_(varName_, type_, fpAccessCb_)          
#define FARAABIN_VAR_ENUM_ARRAY_DICT_WP_(varName_, type_, path_)                    FARAABIN_VAR_ENUM_ARRAY_WP_(varName_, type_, path_)                

#define FARAABIN_VAR_STRUCT_DICT_CB_WP_(varName_, type_, fpAccessCb_, path_)        FARAABIN_VAR_STRUCT_CB_WP_(varName_, type_, fpAccessCb_, path_)      
#define FARAABIN_VAR_STRUCT_DICT_(varName_, type_)                                  FARAABIN_VAR_STRUCT_(varName_, type_)                                
#define FARAABIN_VAR_STRUCT_DICT_CB_(varName_, type_, fpAccessCb_)                  FARAABIN_VAR_STRUCT_CB_(varName_, type_, fpAccessCb_)                
#define FARAABIN_VAR_STRUCT_DICT_WP_(varName_, type_, path_)                        FARAABIN_VAR_STRUCT_WP_(varName_, type_, path_)                      

#define FARAABIN_VAR_STRUCT_ARRAY_DICT_CB_WP_(varName_, type_, fpAccessCb_, path_)  FARAABIN_VAR_STRUCT_ARRAY_CB_WP_(varName_, type_, fpAccessCb_, path_)
#define FARAABIN_VAR_STRUCT_ARRAY_DICT_(varName_, type_)                            FARAABIN_VAR_STRUCT_ARRAY_(varName_, type_)                          
#define FARAABIN_VAR_STRUCT_ARRAY_DICT_CB_(varName_, type_, fpAccessCb_)            FARAABIN_VAR_STRUCT_ARRAY_CB_(varName_, type_, fpAccessCb_)          
#define FARAABIN_VAR_STRUCT_ARRAY_DICT_WP_(varName_, type_, path_)                  FARAABIN_VAR_STRUCT_ARRAY_WP_(varName_, type_, path_)                

/* Exported types ------------------------------------------------------------*/	
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif
#endif

#endif /* FARAABIN_FOBJECT_VAR_WRAPPER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
