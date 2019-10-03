/*
 * sphericalPos.h
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#ifndef SPHERICALPOS_H_
#define SPHERICALPOS_H_

enum {ORIENTATION, HEAD_ANGLE, ROLL_ANGLE}coordNames;

typedef struct{
	int orientation;
	int headAngle;
	int rollAngle;
}sphericalPos_t;

_Bool isValidCoord(int coordName, int coord);

#endif /* SPHERICALPOS_H_ */
