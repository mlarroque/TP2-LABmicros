/*
 * accelerometer.h
 *
 *  Created on: Oct 3, 2019
 *      Author: Lu
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_


void accelandMagnetInit(void);
void getAccelAndMagntData(void);
void coordConverter(int16_t x, int16_t y, int16_t z, int16_t * roll, int16_t * pitch);

#endif /* ACCELEROMETER_H_ */
