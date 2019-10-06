/*
 * i2c.h
 *
 *  Created on: Sep 30, 2019
 *      Author: mlste
 */

#ifndef I2C_H_
#define I2C_H_


/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "MK64F12.h"


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef struct I2C_config_t {
	bool master;
	uint32_t baud_rate;
	int8_t address;
} I2C_config_t;

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/
/**
 * @brief Inicializa un periferico de I2C
 * @params base Puntero al arreglo de registros de un periferico I2C
 * @params config Puntero a parametros de configuracion
 */
void I2C_Init(I2C_Type * base, I2C_config_t * config);

bool I2C_TXRX_master_blocking(  I2C_Type * base,
								uint8_t address,
								uint8_t * tx_buf, uint8_t tx_buf_len,
								uint8_t * rx_buf, uint8_t rx_buf_len);
bool I2C_TXRX_master_non_blocking(  I2C_Type * base,
									uint8_t address,
									uint8_t * tx_buf, uint8_t tx_buf_len,
									uint8_t * rx_buf, uint8_t rx_buf_len);
/*******************************************************************************
 ******************************************************************************/


#endif /* I2C_H_ */
