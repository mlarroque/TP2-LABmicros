/*
 * FXOS8700CQ.h
 *
 *  Created on: 20 sep. 2019
 *      Author: Lu
 */

#ifndef _FXOS8700CQ_H_
#define _FXOS8700CQ_H_

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/**
 * struct where data from the accelerometer/magnetometer will  be stored
*/
typedef struct
{
    float x;
    float y;
    float z;
} raw_data_type;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initializes FXOS8700CQ with the I2C0 as default
 * @param none
 * @return none
*/
void FXOS8700CQInit(void);

/**
 * @brief Reads accelerometer and magnetometer data from FXOS8700CQ
 * @param pAccelData where to store accelerometer data
 * @param pMagnData where to store magnetometer data
 * @return noneh
*/
void ReadAccelMagnData(void);

//Getters
int16_t GetRollAngle(void);
int16_t GetPitchAngle(void);
int16_t GetOrientation(void);
bool GetDataReady(void);

/*******************************************************************************
 ******************************************************************************/

#endif // _FXOS8700CQ_H_
