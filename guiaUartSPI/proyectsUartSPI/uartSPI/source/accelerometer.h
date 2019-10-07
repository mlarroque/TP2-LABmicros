/*
 * accelerometer.h
 *
 *  Created on: Oct 3, 2019
 *      Author: Lu
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <stdint.h>


void accelandMagnetInit(void);
void getAccelAndMagntData(int16_t* roll, int16_t* head, int16_t* orientation);

#endif /* ACCELEROMETER_H_ */
