/*
 * boardsInterface.c
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#include "boardsInterface.h"
#include "coordHAL.h"
#include "accelerometer.h"
#include "timer.h"

#include "hardware.h"

#define SOMETHING_UPDATED 1
#define ANYTHING_UPDATED 0

#define MAX_FPS 20

#define ORIENTATION_NUM_COORD (N_COORDS) //the number of coordinate defines an order of priority
#define HEAD_ANGLE_NUM_COORD (N_COORDS - 1)
#define ROLL_ANGLE_NUM_COORD (N_COORDS - 2)
#define NO_COORD 0

#define TIME_OUT_INACTIVITY 3000
#define TIME_OUT_MAX_FPS 1000

void refreshCounterFps(void);
void timeOutCallback(void);
void initDataBase(void);

int name2numCoord(char coordNameChanged);

sphericalPos_t posDataBase[N_BOARDS];

_Bool flagsOurBoardChanged[N_COORDS] = {false, false, false};

int changesCounter = 0;

void initBoardsInterface(void)
{
	hw_EnableInterrupts();
	accelandMagnetInit();
	hw_DisableInterrupts();
	coordHALinit();
	initDataBase();
	changesCounter = 0;
	SetTimer(INACTIVITY, TIME_OUT_INACTIVITY, timeOutCallback);
	SetTimer(MAX_FPS_CLEAR, TIME_OUT_MAX_FPS, refreshCounterFps);
}

void updateLetcure(void)
{
	char coordNameChanged;
	char coordCharedChanged[MAX_LEN_COORDS];
	int cant = 0, coordNumber = 0, newCoord = 0;

	uint32_t idChanged;
	//sphericalPos_t newPos = updateOurBoardPos(); //ourBoardPos has the mayor priority, so in all cases it has to be updated
	if((changesCounter <= MAX_FPS) && anyCoordHasChanged(&(posDataBase[OUR_BOARD]), &(posDataBase[OUR_BOARD]), &coordNameChanged))
	{
		//posDataBase[OUR_BOARD] = newPos;
		coordNumber = name2numCoord(coordNameChanged);
		flagsOurBoardChanged[coordNumber] = true;
		cant = getBoardCoordChared(OUR_BOARD, coordNameChanged, coordCharedChanged);
		sendCoordToAll(coordNameChanged, coordCharedChanged, cant);
	}
	if(readCoord(&idChanged, &coordNameChanged, coordCharedChanged, &cant))
	{
		newCoord = chars2intCoord(coordCharedChanged, cant);
		if(isValidCoord(coordNameChanged, newCoord))
		{
			switch(coordNameChanged)
			{
				case ORIENTATION_ID:
					posDataBase[idChanged - OFFSET_ID - 1].orientation = newCoord;
					break;
				case ROLL_ANGLE_ID:
					posDataBase[idChanged - OFFSET_ID - 1].rollAngle = newCoord;
					break;
				case HEAD_ANGLE_ID:
					posDataBase[idChanged - OFFSET_ID - 1].headAngle = newCoord;
					break;
			}
		}

	}

}

void timeOutCallback(void)
{
	char idsCoord[N_COORDS] = IDS_COORDS;
	char coordChared[MAX_LEN_COORDS];
	int i = 0, cantCoordChared = 0;
	for(i = 0; i < N_COORDS; i++)
	{
		if(!flagsOurBoardChanged[i])
		{
			cantCoordChared = getBoardCoordChared(OUR_BOARD, idsCoord[i], coordChared);
			sendCoordToAll(idsCoord[i], coordChared, cantCoordChared);
			flagsOurBoardChanged[i] = false;
		}
	}
}

void refreshCounterFps(void)
{
	changesCounter = 0;
}

int getBoardCoordChared(int idBoard, char coordName, char * coordChared)
{
	int coord = 0, cant = 0;
	switch(coordName)
	{
		case ORIENTATION_ID:
			coord = posDataBase[idBoard].orientation;
			break;
		case HEAD_ANGLE_ID:
			coord = posDataBase[idBoard].headAngle;
			break;
		case ROLL_ANGLE_ID:
			coord = posDataBase[idBoard].rollAngle;
			break;
	}
	cant = int2charsCoord(coord, coordChared, true);
	return cant;
}
void initDataBase(void)
{
	int i;
	for(i = 0; i < N_BOARDS; i++)
	{
		posDataBase[i].orientation = 0;
		posDataBase[i].rollAngle = 0;
		posDataBase[i].headAngle = 0;
	}
}

int name2numCoord(char coordNameChanged)
{
	int ret = N_COORDS;
	switch(coordNameChanged)
	{
		case ORIENTATION_ID:
			ret = ORIENTATION_NUM_COORD;
			break;
		case HEAD_ANGLE_ID:
			ret = HEAD_ANGLE_NUM_COORD;
			break;
		case ROLL_ANGLE_ID:
			ret = ROLL_ANGLE_NUM_COORD;
			break;
	}
	return ret;
}
