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
#include <comController2pc.h>
#include "boardsInterface.h"
#include "timer.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define REFRESH_RATE 32 //mseconds (> 30FPS)
#define COORDS_TO_REFRESH 3
#define REFRESH_PER_COORD 4 //mseconds
#define ROLL_COORD_NUMBER 3
#define HEAD_COORD_NUMBER 2
#define ORIENTATION_COORD_NUMBER 1
#define COORDS_NUMBERS 3

char coordPack[N_COORDS_BOARDS + 1];


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
	initResourcesController2pc();
	initializeTimers();
	SetTimer(REFRESH_PC, REFRESH_RATE, callbackRefreshPc);
}
/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{

	char id, cant, coordName;
	if(updateLecture(&id, coordPack, &coordName, &cant))
	{
		if(id == OUR_BOARD)
		{
			sendMessage2othersBoards(coordPack, coordName, cant);
		}
	}
}

void callbackRefreshPc(void)
{
	SetTimer(REFRESH_COORD, REFRESH_PER_COORD, callbackRefreshCoord);
}

void callbackRefreshCoord(void)
{
	static int idBoard2refresh = 0;
	static int coordNumber2refresh = COORD_NUMBERS;
	char coordName;
	char idsCoords[N_COORDS] = IDS_COORDS;
	char * coordChared[MAX_LEN_COORD];
	int cantCoordChared = 0;

	cantCoordChared = getBoardCoordChared(idBoard2refresh, coordChared, idsCoords[coordNumber2refresh - 1]);
	coordName = idsCoords[coordNumber2refresh - 1];

	sendMessage2pc(idBoard2refresh + '0', coordChared, coordName, cantCoordChared);
	coordNumber2refresh--;

	if(coordNumber2refresh == 0)
	{
		idBoard2refresh++;
		coordNumber2refresh = COORD_NUMBERS;
	}

	if(idBoard2refresh == N_BOARDS)
	{
		idBoard2refresh = 0;
		coordNumber2refresh = COORD_NUMBERS;
		DisableTimer(REFRESH_COORD);
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
