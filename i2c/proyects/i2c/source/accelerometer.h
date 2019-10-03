/*
 * accelerometer.h
 *
 *  Created on: 20 sep. 2019
 *      Author: Lu
 */

#ifndef _ACCELEROMETER_H_
#define _ACCELEROMETER_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct{
	int16_t x;
	int16_t y;
	int16_t z;
}raw_data_type;


typedef struct
{
    int16_t x;      //son de 16 bits porque leo parte baja y alta
    int16_t y;
    int16_t z;
} SRAWDATA;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void accelandMagnetInit(void);
raw_data_type* readAccelandMagnetValue(void);
uint8_t _mqx_ints_FXOS8700CQ_start();
int16_t ReadAccelMagnData(SRAWDATA *pAccelData, SRAWDATA *pMagnData);
/*******************************************************************************
 ******************************************************************************/

#endif // _ACCELEROMETER_H_
