/*
 * can.h
 *
 *  Created on: Sep 27, 2019
 *      Author: Manuel Mollon
 *
 *  Bibliography: https://mcuxpresso.nxp.com/apidoc/group__flexcan__driver.html#ga8d7ebf086fb294130fa30a0f75a5f340
 */

#ifndef CAN_H_
#define CAN_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct{
	uint32_t baudRate; 	/* FlexCAN baud rate in bps. */
	uint8_t maxMbNum;  	/* The maximum number of Message Buffers used by user. */
	uint8_t rxMbNum;
	uint8_t txMbNum;
	uint32_t  * ids;
}can_config_t;

typedef enum{FLEXCAN_TxFail,FLEXCAN_TxSuccess}status_t;

/**
 * @brief Initialize CAN driver
 * @param config CAN's configuration
*/
void canInit(can_config_t * config);

/**
 * @brief Checks if a new frame was received in the FIFO buffer
 * @return True if new byte has being received
*/
bool canIsRxMsg(void);

/**
 * @brief Checks how many bytes were received in the message that is FO in the FIFO buffer
 * @return Quantity of received bytes
*/
uint8_t canGetRxMsgLength(void);

/**
 * @brief Read a received message. Non-Blocking
 * @param msg Buffer to paste the received bytes
 * @param cant Desired quantity of bytes to be pasted
 * @param id ID of the message read
 * @return Real quantity of pasted bytes
*/
uint8_t canReadMsg(char * msg, uint32_t * id, uint8_t cant);

/**
 * @brief Write a message to be transmitted. Non-Blocking
 * @param msg Buffer with the bytes to be transfered
 * @param cant Desired quantity of bytes to be transfered
*/
void canWriteMsg(char * msg, uint32_t id, uint8_t cant);

/**
 * @brief Check if all bytes were transfered
 * @return True if all bytes were transfered
*/
bool canIsTxMsgComplete(void);

#endif /* CAN_H_ */
