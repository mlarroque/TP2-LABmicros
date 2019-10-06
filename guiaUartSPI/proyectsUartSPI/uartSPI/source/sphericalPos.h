/*
 * sphericalPos.h
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#ifndef SPHERICALPOS_H_
#define SPHERICALPOS_H_

#define NUMBER_OF_COORDS 3

#define ORIENTATION_ID 'O'
#define HEAD_ANGLE_ID 'C'
#define ROLL_ANGLE_ID 'R'

#define IDS_COORDS {ORIENTATION_ID, HEAD_ANGLE, ROLL_ANGLE}

#define PLUS_FLAG 1

#define SENSIBILITY 5

typedef struct{
	int orientation;
	int headAngle;
	int rollAngle;
}sphericalPos_t;

_Bool isValidCoord(int coordName, int coord);

//return true if there is at least one coord that has changed. return false if the structure newPos is equal to the oldPos
//if return true, the variable coordChanged is charged with the name of the coordinate in which the change was detected.
//The change verification has an order of priority: 1) ORIENTATION_ID - 2) HEAD_ANGLE_ID - 3) ROLL_ANGLE_ID
//It is important to know that the precision of the change is limited by the type of data of the coordinates.
_Bool anyCoordHasChanged(sphericalPos_t * p2oldPos, sphericalPos_t * p2newPos, char * p2coordChangedName);

//This function received a valid coordinate (if it is not valid the function doesnt't do anything) and translates it to
//a chain of chars data types (without a terminator '/0'). The maximum number of chars is determined
//by NUMBER_OF_COORDS + 1 (higher than NUMBER_OF_COORDS because of the sign), so the user of this function must be sure that
//the p2coordChared is not a NULL pointer and also it allows to write NUMBER_OF_COORDS + 1 memory positions
//since p2coordChared (this is really important, if not the function would generate an error in the program).
//If it is necessary the char '+' for positive coordinates, the input plusFlag must be equal to PLUS_FLAG.
//Translation examples: -169 -> '-169', 68 -> '+68' (plusFlag = PLUS_FLAG) or 68 -> '68' if plusFlag is not equal to PLUS_FLAG.

int int2charsCoord(int coord, char * p2coordChared, int plusFlag);

#endif /* SPHERICALPOS_H_ */
