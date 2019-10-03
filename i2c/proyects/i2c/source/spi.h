/*
 * spi.h
 *
 *  Created on: 19 sep. 2019
 *      Author: G5
 */

#ifndef SPI_H_
#define SPI_H_

#define SPI_CANT_IDS //?

#include <stdint.h>

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum {MASTER, SLAVE}SPI_MODE;
typedef struct {
    uint32_t baudRate;
    SPI_MODE mode; //master / slave
} uart_cfg_t;


/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/**
 * @brief Initialize SPI driver
 * @param id SPI's number
 * @param config SPI's configuration (master, slave, etc.)
*/
void SPIInit (uint8_t id, uart_cfg_t config);

#endif /* SPI_H_ */
