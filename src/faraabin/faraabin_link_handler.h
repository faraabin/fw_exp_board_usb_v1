/**
 ******************************************************************************
 * @file           : faraabin_link_handler.h
 * @brief          : Link handler module header file.
 * @copyright      : COPYRIGHT© 2024 FaraabinCo
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
#ifndef FARAABIN_LINK_HANDLER_H
#define FARAABIN_LINK_HANDLER_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "faraabin_type.h"

#include "faraabin_fobject.h"

#include "faraabin_link_deserializer.h"
#include "chrono.h"

/* Exported defines ----------------------------------------------------------*/
/** @defgroup LINK_HANDLER_RESULT Return results of link handler functions.
 *  @{
 */

#define FB_LINK_HANDLER_RESULT_OK                                       (0U)  /*!< No error. */
#define FB_LINK_HANDLER_RESULT_ERROR_ALLOCATED_RX_BUFFER_SIZE_IS_SMALL  (1U)  /*!< Faraabin link handler has RX buffer size smaller than the minimum required. */
#define FB_LINK_HANDLER_RESULT_ERROR_RX_BUFFER_SIZE_IS_NULL             (2U)  /*!< Faraabin link handler RX buffer is null. */

/** @} */ //End of LINK_HANDLER_RESULT

/* Exported macro ------------------------------------------------------------*/
/* Exported types ------------------------------------------------------------*/
/**
 * @brief Dictionary sending mode object.
 * 
 */
typedef struct {
  
  bool SendFlag;    /*!< Flag for indicating send status. */
  
  uint8_t ReqSeq;   /*!< Request sequence. */
  
  bool IsBlocking;  /*!< Flag for indicating that sending is blocking. */
  
}sDictSendingMode;

typedef struct {
	
	bool Init;                        /*!< Initialization flag of faraabin link handler. */

	uint8_t *RxCharBuffer;            /*!< Pointer to the value of received character via faraabin link. */

	uint16_t RxCharBufferSize;        /*!< Size of received characters buffer. */

	bool IsNewFrameDetected;          /*!< Flag for indicating that new frame is detected. */

	sClientFrame ClientFrame;         /*!< Client frame. */

	bool IsFlushingBuffer;            /*!< Flag for indicating that Faraabin buffer is being flushed. */
	
	sChrono ChronoPortSending;        /*!< Chrono for measuring sending time. */

	sDictSendingMode DictSendingMode; /*!< Dictionary sending mode. */

	const char *Password;             /*!< String of the password that is set by the user for authenticating the loading procedure. */

}sLinkHandlerInternal;

/* Exported constants --------------------------------------------------------*/
/* Exported functions prototypes ---------------------------------------------*/
/**
 * @brief Initializes link handler of faraabin.
 * 
 * @param rxBuffer Pointer to the RX buffer that serializer needs to keep transmitting data.
 * @param rxBufferSize Size of the RX buffer.
 * @return result Could be any value from LINK_HANDLER_RESULT group.
 */
uint8_t fFaraabinLinkHandler_Init(uint8_t *rxBuffer, uint16_t rxBufferSize);

/**
 * @brief Runs the link handler priodically and handles frame if new one is detected.
 * 
 * @note This function is called in faraabin.c internally.
 * 
 */
void fFaraabinLinkHandler_Run(void);

/**
 * @brief Get newly received byte from the link and interprets it, if it is a valid frame.
 * 
 * @param c Received character.
 */
void fFaraabinLinkHandler_CharReceived(uint8_t c);

/**
 * @brief Flushes the link TX buffer
 * 
 */
void fFaraabinLinkHandler_FlushBuffer(void);

/**
 * @brief Sets the password for authenticating faraabin connection.
 * 
 * @param password 
 */
void fFaraabinLinkHandler_SetPassword(char *password);

/**
 * @brief Gets the amount of RAM usage in bytes for link handler.
 * 
 * @return usage Amount of RAM usage in bytes. 
 */
uint32_t fFaraabinLinkHandler_GetRamUsage(void);

/* Exported variables --------------------------------------------------------*/
extern sLinkHandlerInternal LinkHandler;

#ifdef __cplusplus
}
#endif

#endif /* FARAABIN_LINK_HANDLER_H */

/************************ © COPYRIGHT FaraabinCo *****END OF FILE****/
