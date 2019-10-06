/*
 * i2c.c
 *
 *  Created on: Sep 30, 2019
 *      Author: mlste
 */

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>


/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/
typedef enum {I2C_INT_TCF, I2C_INT_IAAS, I2C_INT_ARBL, I2C_INT_STOPF, I2C_INT_STARTF, I2C_INT_SLTF, I2C_INT_SHTF2, I2C_INT_IAAS_WU, I2C_INT_N} i2c_interrupt_t;

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

static i2c_interrupt_t I2C_get_interrupt_type_blocking(I2C_Type * base);
static i2c_interrupt_t I2C_get_interrupt_type_non_blocking(I2C_Type * base);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/


/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


//aca pongo las funciones que me pedistes
//fin

void I2C_Init(I2C_Type * base, I2C_config_t * config)
{
	if(base == NULL){return;}	//todo: assert???

	//prender clock //todo: hacer con la otra funcion, debug
    //CLOCK_EnableClock(s_i2cClocks[I2C_GetInstance(base)]);
	//podria hacer algo mas elegante tipo un for pero creo que esto es lo mas directo
	SIM_Type * s = SIM_BASE_PTRS;
	switch((uint32_t)base)
	{
		case I2C0_BASE:
			s->SCGC4 |= SIM_SCGC4_I2C0_MASK;
			break;
		case I2C1_BASE:
			s->SCGC4 |= SIM_SCGC4_I2C1_MASK;
			break;
		case I2C2_BASE:
			s->SCGC1 |= SIM_SCGC1_I2C2_MASK;
			break;
		default:
			break;
	}

    //address register 1
	base->A1 = config->master ? 0 : (config->address << 1);

	//baud rate
    base->F = I2C_F_MULT(2) | I2C_F_ICR(5); //todo: magic number

	//control register 1 (disable during inicializacion, al final enable)
	base->C1 = 0;

	//NOTE on page 1542: first clear STOPF or STARTF, then IICIF
	// => seteo primero FLT y despues S
	base->FLT = 1 << 6 | 1 << 5 | 1 << 4;
	base->S = 1 << 4 | 1 << 1;

	//control register 2 (default)
	base->C2 = 0;
	//Range address register (default)
	base->RA = 0;
	//SMBus Control and Status Register (default)
	base->SMB = 1 << 3 | 1 << 1;
	//Address register 2 (default)
	base->A2 = 1 << 7 | 1 << 6 | 1 << 1;

#warning I2C no tiene filtro digital para glitches.

	//por default, C1 esta t-odo en 0. Prendo el enable y habilito o no las interrupciones dependiendo de si es modo bloqueante o no
	base->C1 = I2C_C1_IICEN(1) | I2C_C1_IICIE(1);

	//podria hacer algo mas elegante tipo un for pero creo que esto es lo mas directo
/*	IRQn_Type * p = I2C_IRQS;
	switch((uint32_t)base)
	{
		case I2C0_BASE:
			NVIC_EnableIRQ(p[0]);
			break;
		case I2C1_BASE:
			NVIC_EnableIRQ(p[1]);
			break;
		case I2C2_BASE:
			NVIC_EnableIRQ(p[2]);
			break;
		default:
			break;
	}*/

}

bool I2C_TXRX_master_blocking(  I2C_Type * base,
								uint8_t address,
								uint8_t * tx_buf, uint8_t tx_buf_len,
								uint8_t * rx_buf, uint8_t rx_buf_len)
{
	//esperar a que el bus este libre
	while(base->C1 & I2C_S_BUSY_MASK){}
	bool should_i_write = tx_buf && tx_buf_len;
	bool should_i_read = rx_buf && rx_buf_len;

	//si no hay nada que hacer, devuelvo false;
	if(!should_i_write && !should_i_read){
		return false;
	}

	/********************
	 * ADDRESS TRANSFER *
	 ********************/
	//Genero start con MST y seteo TX para transmitir la address
	base->C1 |= (I2C_C1_MST_MASK | I2C_C1_TX_MASK);
	if(I2C_get_interrupt_type_blocking(base) != I2C_INT_STARTF){ return false; }

	//transmito address y R/_W
	base->D = address << 1 | (uint8_t)!should_i_write;

	if(I2C_get_interrupt_type_blocking(base) != I2C_INT_TCF){ return false; }

	/*****************
	 * DATA TRANSFER *
	 *****************/
	if(should_i_write){
		//ya estoy en modo TX, no hace falta ponerlo
		for (int i = 0; i < tx_buf_len; ++i) {

			//chequeo que el byte anterior recibio un ACK. Si no, indico que hubo error.
			if(base->S & I2C_S_RXAK_MASK){ return false; }
			base->D = tx_buf[i];
			//espero hasta que se reciba bien. Si no se recibe bien, indico que hubo error
			if(I2C_get_interrupt_type_blocking(base) != I2C_INT_TCF){ return false; }
		}
	}

	/********************************
	 * ADDRESS TRANSFER FOR RESTART *
	 ********************************/
	if(should_i_write){
		if(should_i_read) {
			//genero un repeated start
			base->C1 |= I2C_C1_RSTA_MASK;
			if(!(I2C_get_interrupt_type_blocking(base) == I2C_INT_STARTF)){ return false; }

			//transmito address y R/_W
			base->D = address << 1 | (uint8_t)should_i_read;

			if(I2C_get_interrupt_type_blocking(base) != I2C_INT_TCF){ return false; }
		} else {	//todo: esta mal
			//genero un stop
			base->C1 &= ~(I2C_C1_MST_MASK);
			if(!(I2C_get_interrupt_type_blocking(base) == I2C_INT_STOPF)){ return false; }
		}
	}

	/*****************
	 * DATA RECEPTION *
	 *****************/
	if(should_i_read){
		//pongo modo RX
		base->C1 &= ~I2C_C1_TX_MASK;

		//dummy read
		if(rx_buf_len == 1)	{ base->C1 |= I2C_C1_TXAK_MASK; }
		else 				{ base->C1 &= ~I2C_C1_TXAK_MASK; }

		//dummy read para generar la recibida del nuevo dato
		static volatile _ = 0;
		_ = base->D;
		if(I2C_get_interrupt_type_blocking(base) != I2C_INT_TCF){ return false; }

		for(int i = 0; i < rx_buf_len; i++){
			//NACK para el ultimo byte recibido, ACK para los demas
			if(i + 2 == rx_buf_len)	{ base->C1 |= I2C_C1_TXAK_MASK; }
			else 					{ base->C1 &= ~I2C_C1_TXAK_MASK; }

			//STOP cuando ya recibi todos los bytes
			if(i + 1 == rx_buf_len) {
				base->C1 &= ~I2C_C1_MST_MASK;
				if(I2C_get_interrupt_type_blocking(base) != I2C_INT_STOPF){ return false; }
			}

			rx_buf[i] = base->D;

			//solo busco la interrupcion antes de haber mandado el stop.
			//despues del stop no recibo bytes nuevos => no interrumpe.
			if(i + 1 < rx_buf_len){
				if(I2C_get_interrupt_type_blocking(base) != I2C_INT_TCF){ return false; }
			}

		}
	}
	return true;
}

bool I2C_TXRX_master_non_blocking(I2C_Type * base, uint8_t address, uint8_t * tx_buf, uint8_t tx_buf_len, uint8_t * rx_buf, uint8_t rx_buf_len)
{
	return false;
}


/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


static i2c_interrupt_t I2C_get_interrupt_type_blocking(I2C_Type * base)
{
	while(!(base->S & I2C_S_IICIF_MASK)){}
	return I2C_get_interrupt_type_non_blocking(base);
}

static i2c_interrupt_t I2C_get_interrupt_type_non_blocking(I2C_Type * base){
	//para guardar el tipo de interrupcion. por default, indica que no se reconocio el tipo de interrupcion.
	i2c_interrupt_t interrupt_type = I2C_INT_N;

	//si no habia interrupcion
	if(!(base->S & I2C_S_IICIF_MASK)){ return interrupt_type;}


	if(base->FLT & I2C_FLT_STARTF_MASK){
			base->FLT |= I2C_FLT_STARTF_MASK;
			interrupt_type = I2C_INT_STARTF;
	} else if(base->FLT & I2C_FLT_STOPF_MASK){
		base->FLT |= I2C_FLT_STOPF_MASK;
		interrupt_type = I2C_INT_STOPF;
	} else if(base->S & I2C_S_IAAS_MASK){
		base->S |= I2C_S_IAAS_MASK;
		interrupt_type = I2C_INT_IAAS;
	} else if(base->S & I2C_S_ARBL_MASK){
		base->S |= I2C_S_ARBL_MASK;
		interrupt_type = I2C_INT_ARBL;
	} else if(base->S & I2C_S_TCF_MASK){
		//no hace falta hacer clear del flag
		interrupt_type = I2C_INT_TCF;
	}

	//si habia interrupcion
	//importante: borrar despues de que borro STARTF o STOPF
	base->S |= I2C_S_IICIF_MASK;

	return interrupt_type;
}



/*
bool I2C_TXRX_master(I2C_Type * base, uint8_t address, uint8_t * tx_buf, uint8_t tx_buf_len, uint8_t * rx_buf, uint8_t rx_buf_len)
{
	//esperar a que el bus este libre
	while(base->C1 & I2C_S_BUSY_MASK){}

	bool repeat_start = false;

	//Seteo si voy a usar modo escritura o lectura.
	//Seteo si uso repeated start o stop.
	//Devuelvo false si no hay nada que transmitir o recibir
	bool read_not_write;
	if((tx_buf && tx_buf_len)){
		read_not_write = false;
		if(rx_buf && rx_buf_len){
			repeat_start = true;
		}
	} else if (rx_buf && rx_buf_len){
		read_not_write = true;
	} else {
		return false;
	}


	////START:////

	//Genero start con MST y seteo TX para transmitir la address
	base->C1 |= (I2C_C1_MST_MASK | I2C_C1_TX_MASK);
	base->D = address << 1 | (uint8_t)read_not_write;

	//ya esta enviado el address
	if(!read_not_write){
		for(int i = 0; i < tx_buf_len; i++){
			while(!(base->S & I2C_S_RXAK_MASK)){} //todo: mal ahi, nunca sale si no me mandan un ack
			base->D = *tx_buf++;
		}
	}
	while(1);
	if(repeat_start){
		//genero repeated start, mando el address e indico modo lectura
		base->C1 |= I2C_C1_RSTA_MASK;
		base->D = address << 1 | 1;
	}
}
*/
int i = 0;
void I2C0_IRQHandler(void){
	i++;
}

void I2C1_IRQHandler(void){
	if(i){ i--; }
}

void I2C2_IRQHandler(void){
	volatile static bool f = false;
	f = true;
}
/*******************************************************************************
 ******************************************************************************/
