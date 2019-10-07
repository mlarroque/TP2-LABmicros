/*
 * accelerometer.h
 *
 *  Created on: Oct 3, 2019
 *      Author: Lu
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <stdint.h>


/**
 * @brief Initialize Accelerometer and Magnetometer, with the corresponding default protocol (IC2_0)
 * @param none
 * @return none
*/
void accelandMagnetInit(void);

/**
 * @brief Returns Accelerometer and Magnetometer info in degrees: roll angle, head angle, orientation of device.
 * @param roll angle of device.
 * @param head/pitch angle of device.
 * @param orientation of device.
 * @return none
*/
void getAccelAndMagntData(int16_t* roll, int16_t* head, int16_t* orientation);

#endif /* ACCELEROMETER_H_ */
