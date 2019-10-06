/*
 * accelerometer.c
 *
 *  Created on: Oct 3, 2019
 *      Author: Lu
 */

#include "FXOS8700CQ.h"


static raw_data_type *pAccelData;
static raw_data_type *pMagnData;
static bool newDataReady;


void accelandMagnetInit(void)
{
	FXOS8700CQInit();
}


void getAccelAndMagntData(void)	//capaz recibe una estructura con info mas didactica que x,y,z todo
{
	ReadAccelMagnData(pAccelData, pMagnData, newDataReady);
	//convierte de cartesianas a algo util para graficar
}


