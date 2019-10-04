/*
 * boardsInterface.c
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#include "boardsInterface.h"
//#include "interBoardsCommunication.h"
//#include "acelerometerHal.h"
#include "timer.h"

#define SOMETHING_UPDATED 1
#define ANYTHING_UPDATED 0

#define ORIENTATION_NUM_COORD (NUMBER_OF_COORDS) //the number of coordinate defines an order of priority
#define HEAD_ANGLE_NUM_COORD (NUMBER_OF_COORDS - 1)
#define ROLL_ANGLE_NUM_COORD (NUMBER_OF_COORDS - 2)
#define NO_COORD 0

sphericalPos_t ourBoardPos;
sphericalPos_t ourOldBoardPos;
//sphericalPos_t auxOtherBoardPos;

int timeOutNcoords2notice;

void initBoardsInterface(void)
{
	//initAccelerometer();
	//initBoardsCommunication();

	timeOutNcoords2notice = 0;
	SetTimer(INACTIVITY, TIME_OUT, timeOutCallback);
}

int updateLetcure(char * p2id, char * p2coord, char * p2coordName)
{
	int somethingUpdated = ANYTHING_UPDATED;
	char coordNameChanged;
	//updateOurBoardPos(); //ourBoardPos has the mayor priority, so in all cases it has to be updated
	if(timeOutNcoords2notice != NO_COORD) //at the beginning of the function it is necessary check if there is any pending time out event to communicate
	{
		*id = OUR_BOARD;
		if(timeOutNcoords2notice == ORIENTATION_NUM_COORD)
		{
			*p2coord = int2charCoord(ourBoardPos.orientation);
			*p2coordName = ORIENTATION_ID;
		}
		else if(timeOutNcoords2notice == HEAD_ANGLE_NUM_COORD)
		{
			*p2coord = int2charCoord(ourBoardPos.headAngle);
			*p2coordName = HEAD_ANGLE_ID;
		}
		else if(timeOutNcoords2notice == ROLL_ANGLE_NUM_COORD)
		{
			*p2coord = int2charCoord(ourBoardPos.rollAngle);
			*p2coordName = ROLL_ANGLE_ID;
		}
		timeOutNcoord2notice--; //decrement number of time out events pending to notice
		somethingUpdated = SOMETHING_UPDATED;
		RestartTimer(INACTIVITY);
	}
	else if(anyCoordHasChanged(ourOldBoardPos, ourBoardPos, &coordNameChanged))
	{
		ourOldBoardPos = ourBoardPos; //remember the change for future updates
		*p2id = OUR_BOARD;
		switch(coordNameChanged)
		{
			case ORIENTATION_ID:
				*p2coord = int2charCoord(ourBoardPos.orientation);
				*p2coordName = ORIENTATION_ID;
				break;
			case HEAD_ANGLE_ID:
				*p2coord = int2charCoord(ourBoardPos.headAngle);
				*p2coordName = HEAD_ANGLE_ID;
				break;
			case ROLL_ANGLE_ID:
				*p2coord = int2charCoord(ourBoardPos.rollAngle);
				*p2coordName = ROLL_ANGLE_ID;
				break;
		}
		somethingUpdated = SOMETHING_UPDATED;
		RestartTimer(INACTIVITY);
	}
	else if(0/*isSomeChangeInOtherBoards)*/)
	{
		//readInfoExternalBoard(p2id, p2coord, p2coordName);
		somethingUpdated = SOMETHING_UPDATED;
	}
	return somethingUpdated;
}

void sendMessage2otherBoards(char id, char * p2coord, char coordName)
{
	//
}

void timeOutCallback(void)
{
	timeOutNcoord2notice = NUMBER_OF_COORDS; //set flag that reports about the time out to external users
}


