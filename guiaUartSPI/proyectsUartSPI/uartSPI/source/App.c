/***************************************************************************//**
  @file     App.c
  @brief    Application functions
  @author   Nicolás Magliola
 ******************************************************************************/

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
/*
#include "uart.h"

char messageReceived[MAX_MSG_LEN+1];
uint8_t aux;

*/
#include <comController2node.h>
#include "boardsInterface.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */

void App_Init (void)
{
	initBoardsInterface();
	initResourcesController2node();
}
/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{
	char id, coordName;
	char coordPack[N_COORDS_BOARDS + 1];
	if(updateLecture(&id, coordPack, &coordName))
	{
		sendMessage2node(id, coordPack, coordName, UART0_IMPLEMENTATION);
		if(id == OUR_BOARD)
		{
			sendMessage2othersBoards(OUR_BOARD, coordPack, coordName);
		}
	}
}












/* Función que se llama 1 vez, al comienzo del programa */
/*
void App_Init (void)
{
	uart_cfg_t configUART0;
	configUART0.baudRate = 9600;
	configUART0.parity = NO_PARITY;
	configUART0.mode = NON_BLOCKING_SIMPLE;
	configUART0.txWaterMark = 2;
	configUART0.rxWaterMark = 5;

	uartInit(U0, configUART0);
	uartWriteMsg(U0, "uart0", 5);

}

/* Función que se llama constantemente en un ciclo infinito */
/*
void App_Run (void)
{

	if(uartIsRxMsg(U0))
	{
		aux = uartReadMsg(U0, messageReceived, 1);
		uartWriteMsg(U0, messageReceived, aux);
	}
	//espero interrupciones y realizo ISRs.
}

*/
/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
