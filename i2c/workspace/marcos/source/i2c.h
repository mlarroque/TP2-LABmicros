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
#define NULL 0

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

enum {I2C_ERROR=9, I2C_OK=6};

////RELLENAR ESTA ESTRUCTURAAAA!!!!1
//typedef struct{
//	 i2c_direction_type dir;		//si transmito, recibo
//	uint8_t slave_address;				//con qué slave quiero comunicarme
//	uint8_t* data;					//a qué registro quiero escribir
//	uint8_t	data_size;
//	uint8_t reg;
//    uint8_t reg_size;
//	bool repeat_start;
//}i2c_transfer_type;

//typedef struct{
//	uint32_t baud_rate;
//	bool  polling_mode;	//definir 0 si se quieren interrupciones
//}i2c_conf_type;

/*******************************************************************************
 * VARIABLE PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES WITH GLOBAL SCOPE
 ******************************************************************************/

bool i2cInit(uint8_t channel);

void i2cReadReg(uint8_t slave, uint8_t reg, uint8_t* data, uint8_t data_size);
void i2cWriteReg(uint8_t slave, uint8_t reg, uint8_t* data, uint8_t data_size);


//void i2cTransferNonBlocking(i2c_transfer_type t);
//bool i2cTransferBlocking(i2c_transfer_type t);


//datatypealgo readDataBurst(dfsdfsdf)

/*******************************************************************************
 ******************************************************************************/

#endif // _I2C_H_
