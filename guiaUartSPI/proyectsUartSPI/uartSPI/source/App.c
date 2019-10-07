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
#include "comController2pc.h"
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


/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/
void callbackRefreshPc(void);

void callbackRefreshCoord(void);

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/

/* Función que se llama 1 vez, al comienzo del programa */

void App_Init (void)
{
	InitializeTimers();
	initBoardsInterface();
	initResourcesController2pc();
	SetTimer(REFRESH_PC, REFRESH_RATE, callbackRefreshPc);
}
/* Función que se llama constantemente en un ciclo infinito */

void App_Run (void)
{
	updateLecture();
}

void callbackRefreshPc(void)
{
	SetTimer(REFRESH_COORD, REFRESH_PER_COORD, callbackRefreshCoord);
}

void callbackRefreshCoord(void)
{
	static int idBoard2refresh = 0;
	static int coordNumber2refresh = N_COORDS;
	char coordName;
	char idsCoords[N_COORDS] = IDS_COORDS;
	char coordChared[MAX_LEN_COORD];
	int cantCoordChared = 0;

	cantCoordChared = getBoardCoordChared(idBoard2refresh, idsCoords[coordNumber2refresh - 1], coordChared);
	coordName = idsCoords[coordNumber2refresh - 1];

	sendMessage2pc(idBoard2refresh + '0', coordChared, coordName, cantCoordChared);
	coordNumber2refresh--;

	if(coordNumber2refresh == 0)
	{
		idBoard2refresh++;
		coordNumber2refresh = COORDS_TO_REFRESH;
	}

	if(idBoard2refresh == N_BOARDS)
	{
		idBoard2refresh = 0;
		coordNumber2refresh = COORDS_TO_REFRESH;
		DisableTimer(REFRESH_COORD);
	}
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



/*******************************************************************************
 ******************************************************************************/
