/*
 * accelerometer.c
 *
 *  Created on: Oct 3, 2019
 *      Author: Lu
 */

#include "FXOS8700CQ.h"
#include "accelerometer.h"



void accelandMagnetInit(void)
{
	FXOS8700CQInit();
}


void getAccelAndMagntData(void)
{
	ReadAccelMagnData();					//leemos int del acelerometro

}

bool IsDataReady(void)
{
	return  GetDataReady();
}

angles_t GetMeasuredAngles(void)
{
	angles_t data;
	data.roll = GetRollAngle();
	data.pitch=GetPitchAngle();
	data.orientation=GetOrientation();
	return data;
}



