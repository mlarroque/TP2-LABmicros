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
 * @brief Reads certain register from the slave.
 * @param slave: slave address
 * @param reg: register address to read from
 * @param data: pointer to data where reading will be stored
 * @param data_size: bytes to read
 * @param callback: callback to be called when transfer finishes
 * @return none
*/
void i2cReadReg(uint8_t slave, uint8_t reg, uint8_t* data, uint8_t data_size, callbackPtr callback);


/**
 * @brief Writes certain register from the slave.
 * @param slave: slave address
 * @param reg: register address to write to
 * @param data: pointer to data with data to be written
 * @param data_size: bytes to write
 * @param callback: callback to be called when transfer finishes
 * @return none
*/
void i2cWriteReg(uint8_t slave, uint8_t reg, uint8_t* data, uint8_t data_size, callbackPtr callback);



/*******************************************************************************
 ******************************************************************************/

#endif // _I2C_H_
