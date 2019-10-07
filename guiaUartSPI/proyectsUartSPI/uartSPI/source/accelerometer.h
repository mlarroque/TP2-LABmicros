/*
 * accelerometer.h
 *
 *  Created on: Oct 3, 2019
 *      Author: Lu
 */

#ifndef ACCELEROMETER_H_
#define ACCELEROMETER_H_

#include <stdint.h>
#include <stdbool.h>

typedef struct{
	int16_t roll;
	int16_t pitch;
	int16_t orientation;
}angles_t;


/**
 * @brief Initialize Accelerometer and Magnetometer,
 * with the corresponding default protocol (IC2_0)
 * @param none
 * @return none
*/
void accelandMagnetInit(void);

/**
 * @brief Triggers new reading from the sensor
 * @param none
 * @return none
*/
void getAccelAndMagntData(void);


/**
 * @brief Lets the user know if there is a new position
 * @param none
 * @return true if new position available, false otherwise
*/
bool IsDataReady(void);


/**
 * @brief Getter for the new position.
 * @param none
 * @return angles_t estructure with roll angle,
 * pitch/head angle, and orientation of device
*/
angles_t GetMeasuredAngles(void);

#endif /* ACCELEROMETER_H_ */
