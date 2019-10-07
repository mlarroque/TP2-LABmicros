/*
 * i2c.h
 *
 *  Created on: 20 sep. 2019
 *      Author: Lu
 */

#ifndef _I2C_H_
#define _I2C_H_

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

typedef void (*callbackPtr)(void);

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Initialize I2C module
 * @param channel: choose 0, 1 or 2 for said module to be initialized
 * @return true if init possible, false otherwise
*/
bool i2cInit(uint8_t channel);

/**
 * @brief Reads certain register from the I2C.
 * @param roll angle of device.
 * @param head/pitch angle of device.
 * @param orientation of device.
 * @return none
*/
void i2cReadReg(uint8_t slave, uint8_t reg, uint8_t* data, uint8_t data_size, callbackPtr callback);
void i2cWriteReg(uint8_t slave, uint8_t reg, uint8_t* data, uint8_t data_size, callbackPtr callback);



/*******************************************************************************
 ******************************************************************************/

#endif // _I2C_H_
