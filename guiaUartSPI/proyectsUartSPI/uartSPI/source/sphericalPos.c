/*
 * sphericalPos.c
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#include "sphericalPos.h"
#include <stdbool.h>
#include <stdlib.h>

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

int exp10(int i);

_Bool isValidCoord(char coordName, int coord)
{
	bool ret = false;
	switch(coordName)
	{
		case ORIENTATION_ID:
			ret = IS_VALID_ORIENTATION_SPHERICAL(coord);
			break;
		case HEAD_ANGLE_ID:
			ret = IS_VALID_HEAD_ANGLE_SPHERICAL(coord);
			break;
		case ROLL_ANGLE_ID:
			ret = IS_VALID_ROLL_ANGLE_SPHERICAL(coord);
			break;
		default:
			ret = false;
	}
	return ret;
}
_Bool isValidPos(sphericalPos_t * p2pos)
{
	_Bool ret = false;
	if(isValidCoord(ORIENTATION_ID, p2pos->orientation) && isValidCoord(HEAD_ANGLE_ID, p2pos->headAngle) && isValidCoord(ROLL_ANGLE_ID, p2pos->rollAngle))
	{
		ret = true;
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
	else;

	return ret;
}

int int2charsCoord(int coord, char * p2coordChared, int plusFlag)
{
	int cant = 0, i = 0, aux = 0, signCounter = 1;

	if(plusFlag && (coord >= 0))
	{
		p2coordChared[0] = '+';
		cant++;
		signCounter--;
	}
	else if(coord < 0)
	{
		p2coordChared[0] = '-';
		cant++;
		signCounter--;
		coord = coord*(-1);
	}
	for(i = 0; (i < N_COORDS); i++)
	{
		aux = coord % 10;
		p2coordChared[N_COORDS - i - signCounter] = aux + '0';
		aux = coord - aux;
		coord = aux / 10;

		cant++;
	}
	return cant;
}

int chars2intCoord(char * p2coordChared, int cant)
{
	int i = 0, ret = 0;
	if((p2coordChared[i] == '+') || (p2coordChared[i] == '-'))
	{
		cant--;
	}
	for(i = 0; i < cant; i++)
	{
		ret += (p2coordChared[cant - 1] - '0') * exp10(i);
	}
	if(p2coordChared[i] == '-')
	{
		ret = (-1) * ret;
	}
	return ret;
}

_Bool areCoordsEquals(int coordA, int coordB)
{
	_Bool ret = true;
	if(abs(coordB-coordA) >= SENSIBILITY)
	{
		ret = false;
	}
	return ret;
}

int exp10(int i)
{
	int j = 0, ret = 10;
	if(i <= 0)
	{
		ret = 1;
	}
	else
	{
		for(j = 0; j < i; j++)
		{
			ret = 10*ret;
		}
	}
	return ret;
}
