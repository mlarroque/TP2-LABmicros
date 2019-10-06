/*
 * FXOS8700CQ.c
 *
 *  Created on: Oct 4, 2019
 *      Author: mlste
 */

/*******************************************************************************
 * INCLUDE HEADER FILES
 ******************************************************************************/
#include "FXOS8700CQ.h"
#include <stdbool.h>
#include "i2c.h"

/*******************************************************************************
 * CONSTANT AND MACRO DEFINITIONS USING #DEFINE
 ******************************************************************************/

#define FXOS8700CQ_STATUS 		0x00
#define FXOS8700CQ_WHOAMI 		0x0D
#define FXOS8700CQ_XYZ_DATA_CFG 0x0E
#define FXOS8700CQ_CTRL_REG1 	0x2A
#define FXOS8700CQ_M_CTRL_REG1 	0x5B
#define FXOS8700CQ_M_CTRL_REG2 	0x5C
#define FXOS8700CQ_WHOAMI_VAL 	0xC7
#define FXOS8700CQ_ADDRESS 		0X1D

#define FXOS8700CQ_READ_LEN		13

#define FXOS8700CQ_MAX_DATA_SENT_LENGTH 20
/*******************************************************************************
 * ENUMERATIONS AND STRUCTURES AND TYPEDEFS
 ******************************************************************************/

/*******************************************************************************
 * VARIABLES WITH GLOBAL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * FUNCTION PROTOTYPES FOR PRIVATE FUNCTIONS WITH FILE LEVEL SCOPE
 ******************************************************************************/

//register: registro dentro del integrado.
//rx_buf: buffer donde guardar lo recibido.
//rx_buf_len: longitud del buffer.
bool FXOS_read_from_register(uint8_t FXOS_register, uint8_t * rx_buf, uint8_t rx_buf_len);

//register: registro dentro del integrado.
//tx_buf: buffer donde se encuentran los bytes a enviar.
//tx_buf_len: longitud del buffer.
bool FXOS_write_to_register(uint8_t FXOS_register, uint8_t * tx_buf, uint8_t tx_buf_len);

/*******************************************************************************
 * ROM CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/

/*******************************************************************************
 * STATIC VARIABLES AND CONST VARIABLES WITH FILE LEVEL SCOPE
 ******************************************************************************/
//buffer usado para mandar los datos a escribir al integrado. Tiene que poder
//contener todos los valores a guardar mas el registro al que se lo quiere
//enviar
static uint8_t tx_buf_2_write[FXOS8700CQ_MAX_DATA_SENT_LENGTH + 1];

/*******************************************************************************
 *******************************************************************************
                        GLOBAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/


void FXOS_init()
{
	/************
	 * INIT I2C *
	 ************/

	SIM_Type * s = SIM;
	s->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    PORT_Type * i2c_port = PORTE;
    i2c_port->PCR[24] = PORT_PCR_ODE_MASK | PORT_PCR_MUX(5);
    i2c_port->PCR[25] = PORT_PCR_ODE_MASK | PORT_PCR_MUX(5);

	I2C_config_t config = {
		true,
		0,
		0,
		true
	};
	I2C_Init(I2C0, &config);

	/***************
	 * CONFIG FXOS *
	 ***************/
	//sacado de datasheet Rev. 7.0 - 22 March 2016

	uint8_t databyte = 0xFF;

	// read and check the FXOS8700CQ WHOAMI register
	FXOS_read_from_register(FXOS8700CQ_WHOAMI, &databyte, 1);
	if(databyte != FXOS8700CQ_WHOAMI_VAL) { return; }

	// write 0000 0000 = 0x00 to accelerometer control register 1 to place FXOS8700CQ into
	// standby
	// [7-1] = 0000 000
	// [0]: active=0
	databyte = 0x00;
	FXOS_write_to_register(FXOS8700CQ_CTRL_REG1, &databyte, 1);

	// write 0001 1111 = 0x1F to magnetometer control register 1
	// [7]: m_acal=0: auto calibration disabled
	// [6]: m_rst=0: no one-shot magnetic reset
	// [5]: m_ost=0: no one-shot magnetic measurement
	// [4-2]: m_os=111=7: 8x oversampling (for 200Hz) to reduce magnetometer noise
	// [1-0]: m_hms=11=3: select hybrid mode with accel and magnetometer active
	databyte = 0x1F;
	FXOS_write_to_register(FXOS8700CQ_M_CTRL_REG1, &databyte, 1);

	// write 0010 0000 = 0x20 to magnetometer control register 2
	// [7]: reserved
	// [6]: reserved
	// [5]: hyb_autoinc_mode=1 to map the magnetometer registers to follow the
	// accelerometer registers
	// [4]: m_maxmin_dis=0 to retain default min/max latching even though not used
	// [3]: m_maxmin_dis_ths=0
	// [2]: m_maxmin_rst=0
	// [1-0]: m_rst_cnt=00 to enable magnetic reset each cycle
	databyte = 0x20;
	FXOS_write_to_register(FXOS8700CQ_M_CTRL_REG2, &databyte, 1);

	// write 0000 0001= 0x01 to XYZ_DATA_CFG register
	// [7]: reserved
	// [6]: reserved
	// [5]: reserved
	// [4]: hpf_out=0
	// [3]: reserved
	// [2]: reserved
	// [1-0]: fs=01 for accelerometer range of +/-4g range with 0.488mg/LSB
	databyte = 0x01;
	FXOS_write_to_register(FXOS8700CQ_XYZ_DATA_CFG, &databyte, 1);

	// write 0000 1101 = 0x0D to accelerometer control register 1
	// [7-6]: aslp_rate=00
	// [5-3]: dr=001 for 200Hz data rate (when in hybrid mode)
	// [2]: lnoise=1 for low noise mode
	// [1]: f_read=0 for normal 16 bit reads
	// [0]: active=1 to take the part out of standby and enable
	databyte = 0x0D;
	FXOS_write_to_register(FXOS8700CQ_CTRL_REG1, &databyte, 1);

}

FXOS_data_t FXOS_get_data()
{
	uint8_t Buffer[FXOS8700CQ_READ_LEN];
	FXOS_read_from_register(FXOS8700CQ_STATUS, Buffer, FXOS8700CQ_READ_LEN);
	FXOS_data_t data;

	//La info del acelerometro viene serial y es de 14 bits
	data.accel.x = (int16_t)((Buffer[1] << 8) | Buffer[2])>> 2;
	data.accel.y = (int16_t)((Buffer[3] << 8) | Buffer[4])>> 2;
	data.accel.z = (int16_t)((Buffer[5] << 8) | Buffer[6])>> 2;

	//La info del magnetometro viene serial y es de 16 bits
	data.magnet.x = (Buffer[7] << 8) | Buffer[8];
	data.magnet.y = (Buffer[9] << 8) | Buffer[10];
	data.magnet.z = (Buffer[11] << 8) | Buffer[12];
	return data;
}

/*******************************************************************************
 *******************************************************************************
                        LOCAL FUNCTION DEFINITIONS
 *******************************************************************************
 ******************************************************************************/



//register: registro dentro del integrado.
//rx_buf: buffer donde guardar lo recibido.
//rx_buf_len: longitud del buffer.
bool FXOS_read_from_register(uint8_t FXOS_register, uint8_t * rx_buf, uint8_t rx_buf_len)
{
	I2C_TXRX_master_blocking(I2C0, FXOS8700CQ_ADDRESS, &FXOS_register, 1, rx_buf, rx_buf_len);
}

//register: registro dentro del integrado.
//tx_buf: buffer donde se encuentran los bytes a enviar.
//tx_buf_len: longitud del buffer.
bool FXOS_write_to_register(uint8_t FXOS_register, uint8_t * tx_buf, uint8_t tx_buf_len)
{
	//hago un nuevo buffer con todos los datos de tx_buf pero agregando
	//al registro en el primer lugar
	tx_buf_2_write[0] = FXOS_register;
	for (int i = 0; i < tx_buf_len; ++i) {
		tx_buf_2_write[i+1] = tx_buf[i];
	}

	I2C_TXRX_master_blocking(I2C0, FXOS8700CQ_ADDRESS, &tx_buf_2_write, tx_buf_len + 1, 0, 0);
}



/*******************************************************************************
 ******************************************************************************/


