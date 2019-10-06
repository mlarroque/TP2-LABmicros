/*
 * accelerometer.c
 *
 *  Created on: Oct 3, 2019
 *      Author: Lu
 */

#include "FXOS8700CQ.h"

#define PI (float32_t) 3.141592654

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


float32_t atan2(int16_t y, int16_t x)
{
	if( x > 0 )
	{
		//Esto en el primer o cuarto cuadrante
	}
	else if( (x<0) && (y>=0) )
	{
		//Estoy en el segundo cuadrante
	}
	else if( (x<0) && (y<0) )
	{
		//Estoy en el tercer cuadrante
	}
	else if( (x==0) && (y>0) )
	{
		return (float32_t)(PI/2.0);
	}
	else if( (x==0) && (y>0) )
	{
		return (float32_t)( (-PI)/2.0 );
	}
	else
	{
		//indefinido
	}
}

float32_t atan(float32_t arg)
{

}
