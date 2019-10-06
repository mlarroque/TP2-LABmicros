/*
 * accelerometer.c
 *
 *  Created on: Oct 3, 2019
 *      Author: Lu
 */

#include "FXOS8700CQ.h"
#include <math.h>

#define PI (float) 3.141592654

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

void coordConverter(int16_t x, int16_t y, int16_t z, int16_t * roll, int16_t * pitch){
	*roll = atan2(y,z);
	*pitch = atan2(-x,sqrt(y*y+z*z));
}

float atan2(int16_t y, int16_t x)
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
		return (float)(PI/2.0);
	}
	else if( (x==0) && (y>0) )
	{
		return (float)( (-PI)/2.0 );
	}
	else
	{
		//indefinido
	}
}

float atan(float arg)
{

}
