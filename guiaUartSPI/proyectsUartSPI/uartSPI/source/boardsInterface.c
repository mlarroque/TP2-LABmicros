/*
 * boardsInterface.c
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#include "boardsInterface.h"
//#include "canHAL.h"
//#include "acelerometerHal.h"
#include "timer.h"

#include "boardIds.h"

#define CHANGED_TRUE 1
#define CHANGED_FALSE 0

#define ORIENTATION_NUM_COORD (NUMBER_OF_COORDS) //the number of coordinate defines an order of priority
#define HEAD_ANGLE_NUM_COORD (NUMBER_OF_COORDS - 1)
#define ROLL_ANGLE_NUM_COORD (NUMBER_OF_COORDS - 2)
#define NO_COORD 0

sphericalPos_t ourBoardPos;
sphericalPos_t ourOldBoardPos;


int changedBeforeTimeOutFlag;
int timeOutNcoords2notice;

void initBoardsInterface(void)
{
	//initAccelerometer();
	//initBoardsCommunication();

	timeOutNcoords2notice = 0;
	changedBeforeTimeOutFlag = CHANGED_FALSE;
	SetTimer(INACTIVITY, TIME_OUT, timeOutCallback);
}

int updateLetcure(char * id, char * coord, char * coordName)
{
	char coordNameChanged;
	//updateOurBoardPos(); //ourBoardPos has the mayor priority, so in all cases it has to be updated
	if(timeOutNcoords2notice != NO_COORD) //at the beginning of the function it is necessary check if there is any pending time out event to communicate
	{
		if(timeOutNcoords2notice == ORIENTATION_NUM_COORD)
		{
			*id = ORIENTATION_ID;
			*coord = int2charCoord(ourBoardPos.orientation);
			*coordName = ORIENTATION_ID;
		}
		else if(timeOutNcoords2notice == HEAD_ANGLE_NUM_COORD)
		{
			*id = ORIENTATION_ID;
			*coord = int2charCoord(ourBoardPos.headAngle);
			*coordName = HEAD_ANGLE_ID;
		}
		else if(timeOutNcoords2notice == ROLL_ANGLE_NUM_COORD)
		{
			*id = ORIENTATION_ID;
			*coord = int2charCoord(ourBoardPos.rollAngle);
			*coordName = ROLL_ANGLE_ID;
		}
		timeOutNcoord2notice--; //decrement number of time out events pending to notice
	}
	else if(anyCoordHasChanged(ourOldBoardPos, ourBoardPos, &coordNameChanged))
	{
		ourOldBoardPos = ourBoardPos; //remember the change for future updates

	}

	//send headAngle to other boards
	//send rollAngle to other boards
	//me fijo si le tengo que avisar algo al usuario externo por el tema del time out
	//else if: me fijo si cambio alguna coordenada mia (si cambio la actualizo, aviso al usuario ex
}

void timeOutCallback(void)
{
	if(changedBeforeTimeOut == CHANGED_FALSE)
	{
		timeOutNcoord2notice = NUMBER_OF_COORDS; //set flag that reports about the time out to external users
	}
	changedBeforeTimeOut = FALSE;  //flag clear
}


