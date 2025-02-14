/**
 ******************************************************************************
 * @file           : faraabin_port.h
 * @brief          : Faraabin port header file.
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
     This file contains function prototypes that used needs to port faraabin in
     their application.
 
     User must create a port file in their software and include this header file.
     After implementing all required functions for their embedded software, 
     faraabin port will be completed.
 * @endverbatim
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef FARAABIN_PORT_H
#define FARAABIN_PORT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>

/* Exported defines ----------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief This function is used by faraabin appliaction to get the firmware name.
 * 
 * @note Firmware name is a name that appears in faraabin application on PC.
 *       Main purpose of this info is to easily identify which project user
 *       is working on.
 * 
 * @return fwName Pointer to the string that contains the firmware name of the software.
 */
char* fFaraabin_GetFirmwareName(void);

/**
 * @brief This function is used by faraabin application to get the firmware info.
 * 
 * @note Firmware info can be any string that user wants to view them in PC app.
 *       Faraabin supports JSON and can show the info with this format in PC app.
 * 
 * @return fwInfo Pointer to the string that contains the firmware information of the software.
 */
char* fFaraabin_GetFirmwareInfo(void);

/**
 * @brief Faraabin uses this function to get the pointer of the TX buffer, required for storing
 *        data before transmitting them over the link to PC application.
 * 
 * @note This buffer is provided by user. This way user can put the buffer at desired location
 *       in their embedded software.
 * 
 * @return pTxBuffer Pointer to faraabin TX buffer.
 */
uint8_t* fFaraabin_GetTxBufferPointer(void);

/**
 * @brief To get the size of the TX buffer (provided by user), faraabin calls this function.
 *        This function must return the size dedicated to the TX buffer of the faraabin.
 * 
 * @return size Size of faraabin TX buffer.
 */
uint32_t fFaraabin_GetTxBufferSize(void);

/**
 * @brief Faraabin uses this function to get the pointer of the RX buffer, required for storing
 *        received data from PC over the link.
 * 
 * @note This buffer is provided by user. This way user can put the buffer at desired location
 *       in their embedded software.
 * 
 * @return pRxBuffer Pointer to faraabin RX buffer.
 */
uint8_t* fFaraabin_GetRxBufferPointer(void);

/**
 * @brief To get the size of the RX buffer (provided by user), faraabin calls this function.
 *        This function must return the size dedicated to the RX buffer of the faraabin.
 * 
 * @return size Size of faraabin TX buffer.
 */
uint32_t fFaraabin_GetRxBufferSize(void);

/**
 * @brief Faraabin uses this function to send data over the link.
 * 
 * @note User can use serial, USB, ehternet, etc. for communicating with faraabin application in PC.
 *       Faraabin does not need to know anything about the implementation details of the link.
 *       It only needs to use this interface for transmitting data to the PC.
 *       This function will send "size" bytes of data starting from address indicated by "data".
 * 
 * @param data Pointer to data that is about to be transmitted (faraabin internal use).
 * @param size Size of transmitting data.
 * @return result If transmitting is successful, this interface must return '0'.
 *                 In case of any error, it must return '1'.
 */
uint8_t fFaraabin_Send(uint8_t *data, uint16_t size);

/**
 * @brief This function is used by faraabin to check if the user link is busy (sending data) or not.
 * 
 * @note Using this function, faraabin only sends data whenever the link is not busy.
 * 
 * @return IsSending Sending status of the link.
 */
bool fFaraabin_IsSending(void);

/**
 * @brief This function makes the MCU to execute a software reset.
 * 
 * @note User on the PC application of faraabin can send a reset command. When this command is
 *       received in embedded software, this function will be called. In order to reset the MCU,
 *       user must implement this function to execute a software reset.
 * 
 */
void fFaraabin_ResetMcu(void);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_PORT_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
