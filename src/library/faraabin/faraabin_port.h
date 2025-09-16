/**
 ******************************************************************************
 * @file           : faraabin_port.h
 * @brief          : Faraabin port header file.
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
 * @brief Run before faraabin init. User can write any initialize that should run
 * before faraabin init.
 * 
 * @return Result of user initialize.
 */
uint8_t fFaraabin_PortInit(void);

/**
 * @brief Run when fFaraabin_Run function is executed.
 * 
 */
void fFaraabin_PortRun(void);

/**
 * @brief Returns Firmware name of your project to faraabin as string.
 * 
 * @note The primary purpose of this name is to help the user easily identify 
 *       the firmware they are working on in Faraabin.
 * 
 * @return FwName A pointer to a string containig the firmware name string.
 */
char* fFaraabin_GetFirmwareName(void);

/**
 * @brief Retrieves additional information from your project to be sent to Faraabin as a string.
 * 
 * @note You can use JSON format in your info string. In Faraabin, this format will
 *       be parsed, allowing each key and value to be presented to the user in a user-friendly manner.
 * 
 * @return FwInfo A pointer to the  string containing the firmware information.
 */
char* fFaraabin_GetFirmwareInfo(void);

/**
 * @brief Returns address of the faraabin TX Buffer.
 * 
 * @note This buffer has to be of type uint8_t with proper size.
 * 
 * @return pTxBuffer Pointer to Faraabin buffer created by user.
 */
uint8_t* fFaraabin_GetTxBufferPointer(void);

/**
 * @brief Returns size of the faraabin TX Buffer.
 * 
 * @return size Size of Faraabin TX buffer created by user.
 */
uint32_t fFaraabin_GetTxBufferSize(void);

/**
 * @brief Returns address of the faraabin RX Buffer.
 * 
 * @note This buffer has to be of type uint8_t with proper size.
 * 
 * @return pRxBuffer Pointer to Faraabin buffer created by user.
 */
uint8_t* fFaraabin_GetRxBufferPointer(void);

/**
 * @brief Returns size of the faraabin RX Buffer.
 * 
 * @return size Size of Faraabin RX buffer created by user.
 */
uint32_t fFaraabin_GetRxBufferSize(void);

/**
 * @brief Faraabin uses this function to send data over the communication link.
 * 
 * @note The user can utilize serial, USB, Ethernet, etc., to communicate with the Faraabin application on the PC.
 *       Faraabin does not need to know the implementation details of the link; it only requires this interface 
 *       for transmitting data to the PC. This function will send "size" bytes of data starting from the address 
 *       indicated by "data".
 * 
 * @param data Pointer to the data that is about to be transmitted to Faraabin application on the PC.
 * @param size Size of the data to be transmitted.
 * @return sendStatus If the transmission is successful, this function must return '0'.
 *                    In case of an error, it must return '1'.
 */
uint8_t fFaraabin_Send(uint8_t *data, uint16_t size);

/**
 * @brief This function is used by Faraabin to check if the user link is busy (i.e., currently sending data).
 * 
 * @note Faraabin will only send data when the link is not busy.
 * 
 * @return isSending Returns the sending status of the link. If the link is busy, it must return 'true'.
 *                   If the link is not busy and Faraabin can send its data, it must return 'false'.
 */
bool fFaraabin_IsSending(void);

/**
 * @brief This function is designed to initiate a software reset via the Faraabin UI.
 * 
 * @note If your microcontroller supports a software reset, implement the function body
 *       to trigger the reset. In Faraabin, you can right-click on your MCU fobject and 
 *       select "Reset MCU" to invoke this function whenever necessary.
 * 
 */
void fFaraabin_ResetMcu(void);

/**
 * @brief This function is used when Faraabin needs to enter a critical section.
 * 
 * @note To overcome mixing of the faraabin data that is being generated in different processes(IRQ or Task), faraabin often needs 
 *       to disable all interrupts and reenable them after generating the frame.
 * 
 */
void fFaraabin_CriticalSectionEnter(void);

/**
 * @brief This function is used when Faraabin needs to exit from a critical section.
 * 
 * @note To overcome mixing of the faraabin data that is being generated in different processes(IRQ or Task), faraabin often needs 
 *       to disable all interrupts and reenable them after generating the frame.
 * 
 */
void fFaraabin_CriticalSectionExit(void);

/* Exported variables --------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_PORT_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
