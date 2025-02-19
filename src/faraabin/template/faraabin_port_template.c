/**
******************************************************************************
* @file           : faraabin_port.c
* @brief          :
* @note           :
* @copyright      : COPYRIGHT© 2022 FaraabinCo
******************************************************************************
* @attention
*
* <h2><center>&copy; Copyright© 2022 FaraabinCo.
* All rights reserved.</center></h2>
*
* This software is licensed under terms that can be found in the LICENSE file
* in the root directory of this software component.
* If no LICENSE file comes with this software, it is provided AS-IS.
*
******************************************************************************
* @verbatim
* @endverbatim
*/

/* Includes ------------------------------------------------------------------*/
#include "faraabin_port.h"

/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static char fwName[] = "DEFAULT_FW_NAME";
static uint8_t fbTxBuffer[3000];
static uint8_t fbRxBuffer[1000];

/* Private function prototypes -----------------------------------------------*/
/* Variables -----------------------------------------------------------------*/

/*
===============================================================================
                ##### faraabin_port.c Exported Functions #####
===============================================================================*/
/**
 * @brief Returns Firmware name of your project to faraabin as string.
 * 
 * @return fwName Firmware name.
 */
char* fFaraabin_GetFirmwareName(void) {
  return fwName;
}

/**
 * @brief Returns additional info from your project to faraabin as a string.
 * 
 * @return fwInfo Firmware info. 
 */
char* fFaraabin_GetFirmwareInfo(void) {
	
  //User can send custom text as info.
  //User can also send any information data in JSON format.
  //Faraabin setect & parse JSON and show key-value pair in pc application.
	return "{\"Clock\":\"100MHz\"}";
}

/**
 * @brief Returns address of the faraabin TX Buffer.
 * 
 * @note This buffer has to be of type uint8_t with proper size.
 * 
 * @return pTxBuffer Pointer to Faraabin buffer created by user.
 */
uint8_t* fFaraabin_GetTxBufferPointer(void) {
  return fbTxBuffer;
}

/**
 * @brief Returns size of the faraabin TX Buffer.
 * 
 * @return size Size of Faraabin buffer created by user.
 */
uint32_t fFaraabin_GetTxBufferSize(void) {
  return sizeof(fbTxBuffer);
}

/**
 * @brief Returns address of the faraabin RX Buffer.
 * 
 * @note This buffer has to be of type uint8_t with proper size.
 * 
 * @return pRxBuffer Pointer to Faraabin buffer created by user.
 */
uint8_t* fFaraabin_GetRxBufferPointer(void) {
  return fbRxBuffer;
}

/**
 * @brief Returns size of the faraabin RX Buffer.
 * 
 * @return size Size of Faraabin buffer created by user.
 */
uint32_t fFaraabin_GetRxBufferSize(void) {
  return sizeof(fbRxBuffer);
}

/**
 * @brief This functions sends faraabin data via user's link to PC.
 * 
 * @return sendStatus '0' if sending data is failed, '1' otherwise.
 */
uint8_t fFaraabin_Send(uint8_t *data, uint16_t size) {

  //User should write code here to send <size> byte of data from buffer <data>
  return 1;
}

/**
 * @brief This function returns the sending status of the faraabin link.
 * 
 * @return isSending 'true' if the link is busy sending data, 'false' if the link is free to send new data.
 */
bool fFaraabin_IsSending(void) {

  //User should return that last sending data is finished or not.
  return false;
}

/**
 * @brief This functions is intended to generate a software reset via faraabin UI.
 * 
 */
void fFaraabin_ResetMcu(void) {
  
  //User should write code here to reset MCU.
}

/*
===============================================================================
                  ##### faraabin_port.c Private Functions #####
===============================================================================*/

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
