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
static void coordConverter(int16_t x, int16_t y, int16_t z, int16_t * roll, int16_t * pitch);

void accelandMagnetInit(void)
{
	FXOS8700CQInit();
}


void getAccelAndMagntData(int16_t* roll, int16_t* head, int16_t* orientation)
{
	ReadAccelMagnData(&pAccelData, &pMagnData);
	coordConverter(pAccelData.x, pAccelData.y, pAccelData.z, roll,  head);
}

void coordConverter(int16_t x, int16_t y, int16_t z, int16_t * roll, int16_t * pitch){
	*roll = atan2(y,z);
	*pitch = atan2(-x,sqrt(y*y+z*z));
}

