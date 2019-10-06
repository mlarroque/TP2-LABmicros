/*
 * sphericalPos.c
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#include "sphericalPos.h"
#include <stdbool.h>

#define MAX_HEAD_ANGLE_SPHERICAL 179
#define MIN_HEAD_ANGLE_SPHERICAL -179

#define MAX_ROLL_ANGLE_SPHERICAL 179
#define MIN_ROLL_ANGLE_SPHERICAL -179

#define MAX_ORIENTATION_SPHERICAL 179
#define MIN_ORIENTATION_SPHERICAL -179

#define IS_VALID_HEAD_ANGLE_SPHERICAL(x)  ( ( (x >=MIN_HEAD_ANGLE_SPHERICAL) && (x <= MAX_HEAD_ANGLE_SPHERICAL) ) )
#define IS_VALID_ROLL_ANGLE_SPHERICAL(x)  ( ( (x >=MIN_ROLL_ANGLE_SPHERICAL) && (x <= MAX_ROLL_ANGLE_SPHERICAL) ))
#define IS_VALID_ORIENTATION_SPHERICAL(x) ( ( (x >=MIN_ORIENTATION_SPHERICAL) && (x <= MAX_ORIENTATION_SPHERICAL) ) )

_Bool areCoordsEquals(int coordA, int coordB);

_Bool isValidCoord(int coordName, int coord)
{
	bool ret = false;
	switch(coordName)
	{
		case ORIENTATION:
			ret = IS_VALID_ORIENTATION_SPHERICAL(coord);
			break;
		case HEAD_ANGLE:
			ret = IS_VALID_HEAD_ANGLE_SPHERICAL(coord);
			break;
		case ROLL_ANGLE:
			ret = IS_VALID_ROLL_ANGLE_SPHERICAL(coord);
			break;
		default:
			ret = false;
	}
	return ret;
}

_Bool anyCoordHasChanged(sphericalPos_t * p2oldPos, sphericalPos_t * p2newPos, char * p2coordChanged)
{
	_Bool ret = false;
	if(!areCoordsEquals(p2newPos->orientation, p2oldPos->orientation))
	{
		ret = true;
		*p2coordChanged = ORIENTATION_ID;
	}
	else if(!areCoordsEquals(p2newPos->headAngle, p2oldPos->headAngle))
	{
		ret = true;
		*p2coordChanged = HEAD_ANGLE_ID;
	}
	else if(!areCoordsEquals(p2newPos->rollAngle, p2oldPos->rollAngle))
	{
		ret = true;
		*p2coordChanged = ROLL_ANGLE_ID;
	}

	return ret;
}

int int2charsCoord(int coord, char * p2coordChared, int plusFlag)
{

}

_Bool areCoordsEquals(int coordA, int coordB)
{
	_Bool ret = false;
	if((coordA >= (coordB + SENSIBILITY)) || (coordA <= (coordB - SENSIBILITY)))
	{
		ret = true;
	}
	return ret;
}
