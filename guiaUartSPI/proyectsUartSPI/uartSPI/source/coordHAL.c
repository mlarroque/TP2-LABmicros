/*
 * coordHAL.c
 *
 *  Created on: Oct 5, 2019
 *      Author: Manuel Mollon
 */

#include "coordHAL.h"
#include "can.h"


/****************************************************
 * DEFINES
 ****************************************************/

/* Own id for the Tx node. */
#define ID	((uint32_t)0x105)
/* Number of Rx Buffers. */
#define RX_MB_NUM	7
/* Number of Tx Buffers. */
#define TX_MB_NUM	1
/* Total number of Buffers. */
#define MB_NUM	(TX_MB_NUM+RX_MB_NUM)
/* Maximum number of bytes to send in one frame*/
#define MAX_BYTES	8
/* Index numbers to decode/encode message*/
#define NAME_COORD_INDEX	0
#define NAME_COORD_CANT	1
#define COORD_INDEX	1

/******************************************************
 * LOCAL VARIABLE DECLARATIONS
 ******************************************************/

/* Ids for the Rx nodes. */
static uint32_t	idsArray[RX_MB_NUM]={0x100,0x101,0x102,0x103,0x104,0x106,0x107};
/* String to create message. */
static char msg[MAX_BYTES];
static uint8_t cant = 0;

/******************************************************
 * LOCAL FUNCTION DECLARATIONS
 ******************************************************/

/**
 * @brief Encodes message to send to other MC.
 * @param nameCoord Coordinate type to send. (Look in coord_t)
 * @param coord String with the coordinate itself.
 * @param coordNum Number of chars in coord string.
*/
static void encodeMsg(char nameCoord, char * coord, uint8_t coordNum);

/**
 * @brief Decodes message to return to user coordinates and id.
 * @param nameCoord Pointer where to store kind of coord received.
 * @param coord String with the coordinate.
 * @param coordNum Number of chars in coord string.
*/
static void decodeMsg(char * nameCoord, char * coord, int * coordNum);

/******************************************************
 * LOCAL FUNCTION DEFINITIONS
 ******************************************************/

static void encodeMsg(char nameCoord, char * coord, uint8_t coordNum){
	/* Save name coord in message string.  */
	msg[NAME_COORD_INDEX] = nameCoord;
	/* Save coord value in message string. */
	for(int i = 0; i<coordNum; i++){
		msg[COORD_INDEX+i] = coord[i];
	}
	cant = coordNum + 1;
}

static void decodeMsg(char * nameCoord, char * coord, int * coordNum){
	/* Get the coord name and length. */
	*nameCoord = msg[NAME_COORD_INDEX];
	*coordNum = cant - NAME_COORD_CANT;
	/* Get the coord value. */
	for(int i = 0; i<(*coordNum); i++){
		coord[i] = msg[COORD_INDEX+i];
	}

}

/******************************************************
 * GLOBAL FUNCTION DEFINITIONS
 ******************************************************/

void coordHALinit(void){
	can_config_t canConfig;
	canConfig.baudRate = 125000U;
	canConfig.maxMbNum = MB_NUM; /* 1 Tx mb and 7 Rx mb*/
	canConfig.rxMbNum = RX_MB_NUM;
	canConfig.txMbNum = TX_MB_NUM;
	canConfig.ids = idsArray;
	canInit(&canConfig);
}

bool sendCoordToAll(char nameCoord, char * coord, uint8_t coordNum){
	bool successStatus = false;
	/* If no message sending, sends new message. */
	if(canIsTxMsgComplete()){
		/* Creates new message to send. */
		encodeMsg(nameCoord, coord, coordNum);
		/* Sends message. */
		canWriteMsg(msg, ID, cant);
		successStatus = true;
	}
	return successStatus;
}

bool readCoord(uint32_t * id, char * nameCoord, char * coord, int * coordNum){
	bool successStatus = false;
	/* If there is a new message in buffer. */
	if(canIsRxMsg()){
		cant = canGetRxMsgLength();
		/* Read message and save ID. */
		canReadMsg(msg, id, cant);
		/* Decode message to return. */
		decodeMsg(nameCoord, coord, coordNum);
		successStatus = true;
	}
	return successStatus;
}
