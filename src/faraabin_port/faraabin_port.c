/**
 ******************************************************************************
 * @file           : faraabin_port.c
 * @brief          :
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2025 FaraabinCo.
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

/* Includes ------------------------------------------------------------------*/
#include "faraabin_port.h"

#if defined ( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 )
#include <arm_compat.h>
#endif

#if defined ( __ICCARM__ )
#include "intrinsics.h"
#endif
#include "bsp.h"
#include "faraabin.h"
#include "stm32f1xx.h"

/* Private define ------------------------------------------------------------*/
/**
 * @brief Definition of the inline function for the corresponding compiler.
 * 
 * @note Depending on your target and compiler, define the 'INLINE_' macro to ensure that 
 *       the compiler forces 'fFaraabin_CriticalSectionExit()' to be an inline function.
 */
#ifndef INLINE__

#if defined ( __CC_ARM )
#define INLINE__  __forceinline

#elif defined ( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 )
#define INLINE__  __attribute__((always_inline))

#elif defined ( __GNUC__ )
#define INLINE__  __attribute__((always_inline))

#elif defined ( __ICCARM__ )
#define INLINE__  __inline

#elif defined ( __TI_ARM__ )
#define INLINE__

#elif defined ( __CSMC__ )
#define INLINE__

#elif defined ( __TASKING__ )
#define INLINE__

#else
  #error Unknown compiler

#endif
#endif  //INLINE__

/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static char FwName[] = "EXPERIENCE_BOARD";
static uint8_t FbTxBuffer[1500];
static uint8_t FbRxBuffer[1000];

/* Private function prototypes -----------------------------------------------*/
static void FaraabinReceiveFrameHandler(uint8_t *data, uint16_t size);

/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
                ##### faraabin_port.c Exported Functions #####
===============================================================================*/

/**
 * @brief 
 * 
 * @return uint8_t 
 */
uint8_t fFaraabin_PortInit(void) {
  /* Register USB receive callback to a function that handles Faraabin frames. */
  fBsp_VCP_RegisterFrameReceivedCallback(FaraabinReceiveFrameHandler);
  
  return 0;
}

/**
 * @brief 
 * 
 */
void fFaraabin_PortRun(void) {
  /*  ______                 _          */
  /* |  ____|               | |         */
  /* | |__   _ __ ___  _ __ | |_ _   _  */
  /* |  __| | '_ ` _ \| '_ \| __| | | | */
  /* | |____| | | | | | |_) | |_| |_| | */
  /* |______|_| |_| |_| .__/ \__|\__, | */
  /*                  | |         __/ | */
  /*                  |_|        |___/  */
}

/**
 * @brief Returns Firmware name of your project to faraabin as string.
 * 
 * @return FwName A pointer to a string containig the firmware name string.
 */
char* fFaraabin_GetFirmwareName(void) {
  return FwName;
}

/**
 * @brief Retrieves additional information from your project to be sent to Faraabin as a string.
 * 
 * @note You can use JSON format in your info string. In Faraabin, this format will
 *       be parsed, allowing each key and value to be presented to the user in a user-friendly manner.
 * 
 * @return FwInfo A pointer to the  string containing the firmware information.
 */
char* fFaraabin_GetFirmwareInfo(void){
  return "{\"Clock\":\"16MHz\"}";
}

/**
 * @brief Returns address of the faraabin TX Buffer.
 * 
 * @note This buffer has to be of type uint8_t with proper size.
 * 
 * @return pTxBuffer Pointer to Faraabin buffer created by user.
 */
uint8_t* fFaraabin_GetTxBufferPointer(void) {
  return FbTxBuffer;
}

/**
 * @brief Returns size of the faraabin TX Buffer.
 * 
 * @return size Size of Faraabin TX buffer created by user.
 */
uint32_t fFaraabin_GetTxBufferSize(void) {
  return sizeof(FbTxBuffer);
}

/**
 * @brief Returns address of the faraabin RX Buffer.
 * 
 * @note This buffer has to be of type uint8_t with proper size.
 * 
 * @return pRxBuffer Pointer to Faraabin buffer created by user.
 */
uint8_t* fFaraabin_GetRxBufferPointer(void) {
  return FbRxBuffer;
}

/**
 * @brief Returns size of the faraabin RX Buffer.
 * 
 * @return size Size of Faraabin RX buffer created by user.
 */
uint32_t fFaraabin_GetRxBufferSize(void) {
  return sizeof(FbRxBuffer);
}

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
uint8_t fFaraabin_Send(uint8_t *data, uint16_t size) {
  return fBsp_VCP_Send(data, size);
}

/**
 * @brief This function is used by Faraabin to check if the user link is busy (i.e., currently sending data).
 * 
 * @note Faraabin will only send data when the link is not busy.
 * 
 * @return isSending Returns the sending status of the link. If the link is busy, it must return 'true'.
 *                   If the link is not busy and Faraabin can send its data, it must return 'false'.
 */
bool fFaraabin_IsSending(void) {
  return fBsp_VCP_IsSending();
}

/**
 * @brief This function is designed to initiate a software reset via the Faraabin UI.
 * 
 * @note If your microcontroller supports a software reset, implement the function body
 *       to trigger the reset. In Faraabin, you can right-click on your MCU fobject and 
 *       select "Reset MCU" to invoke this function whenever necessary.
 * 
 */
void fFaraabin_ResetMcu(void) {
  HAL_NVIC_SystemReset();
}

/**
 * @brief This function is used when Faraabin needs to enter a critical section.
 * 
 * @note To overcome mixing of the faraabin data that is being generated in different processes(IRQ or Task), faraabin often needs 
 *       to disable all interrupts and reenable them after generating the frame.
 * 
 */
INLINE__ void fFaraabin_CriticalSectionEnter(void) {

#if defined ( __CC_ARM )

  __disable_irq();

#elif defined ( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 )

  __disable_irq();

#elif defined ( __GNUC__ )

  __disable_irq();

#elif defined ( __ICCARM__ )

  __disable_interrupt();

#elif defined ( __TI_ARM__ )

  // Implement the critical section enter code for TI ARM compiler;

#elif defined ( __CSMC__ )

  // Implement the critical section enter code for CSMC compiler;

#elif defined ( __TASKING__ )

  // Implement the critical section enter code for TASKING compiler;

#else
  #error Unknown compiler
#endif

}

/**
 * @brief This function is used when Faraabin needs to exit from a critical section.
 * 
 * @note To overcome mixing of the faraabin data that is being generated in different processes(IRQ or Task), faraabin often needs 
 *       to disable all interrupts and reenable them after generating the frame.
 * 
 */
INLINE__ void fFaraabin_CriticalSectionExit(void) {

#if defined ( __CC_ARM )

  __enable_irq();

#elif defined ( __ARMCC_VERSION ) && ( __ARMCC_VERSION >= 6010050 )

  __enable_irq();

#elif defined ( __GNUC__ )

  __enable_irq();

#elif defined ( __ICCARM__ )

  __enable_interrupt();

#elif defined ( __TI_ARM__ )

  // Implement the critical section enter code for TI ARM compiler;

#elif defined ( __CSMC__ )

  // Implement the critical section enter code for CSMC compiler;

#elif defined ( __TASKING__ )

  // Implement the critical section enter code for TASKING compiler;

#else
  #error Unknown compiler
#endif

}

/**
  * @brief Passes received frame from the PC to fFaraabin_HandleReceivedBytes() function for parsing the frame.
  * @param data Pointer to the received frame buffer
  * @param size Size of the received frame
  * @retval None
  */
static void FaraabinReceiveFrameHandler(uint8_t *data, uint16_t size) {
  fFaraabin_HandleReceivedBytes(data, size);
}

/*
===============================================================================
                  ##### faraabin_port.c Private Functions #####
===============================================================================*/

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
