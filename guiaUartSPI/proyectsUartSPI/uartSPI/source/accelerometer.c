/*
 * accelerometer.c
 *
 *  Created on: Oct 3, 2019
 *      Author: Lu
 */

#include "FXOS8700CQ.h"
#include <math.h>


static raw_data_type pAccelData;
static raw_data_type pMagnData;
static void coordConverter(float x, float y, float z, int16_t * roll, int16_t * pitch);

void accelandMagnetInit(void)
{
	FXOS8700CQInit();
}


void getAccelAndMagntData(int16_t* roll, int16_t* head, int16_t* orientation)
{
	ReadAccelMagnData(&pAccelData, &pMagnData);
	coordConverter(pAccelData.x, pAccelData.y, pAccelData.z, roll,  head);
	*orientation = (int16_t)(atan2(pMagnData.y, pMagnData.x)*180/M_PI);
}

void coordConverter(float x, float y, float z, int16_t * roll, int16_t * pitch)
{
	*roll = (int16_t)(atan2(y,z)*180/M_PI);
	*pitch = (int16_t)(atan2(-x,sqrt(y*y+z*z))*180/M_PI);
}

