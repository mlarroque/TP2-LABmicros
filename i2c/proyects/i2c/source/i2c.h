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


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/


/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum {MASTER, SLAVE}i2c_mode_type;
typedef enum {TX, RX, TX_RX}i2c_master_mode_type;


//RELLENAR ESTA ESTRUCTURAAAA!!!!1
typedef struct{
	i2c_master_mode_type master_mode;		//si transmito, recibo, o transmito seguido de recibo
	//con cantidad de bytes o un puntero al arreglo a transmitir me parece mejor
	uint8_t N;		//cantidad de bytes que transmito
	uint8_t M;		//cantidad de bytes que recibo
}i2c_data_type;

typedef struct{
	uint32_t baud_rate;
	i2c_mode_type mode;	//solo se configurará master mode
	uint8_t prim_slave_address;
}i2c_conf_type;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

void i2cInit(void)


datatypealgo readDataBurst(dfsdfsdf)

/*******************************************************************************
 ******************************************************************************/

#endif // _I2C_H_
