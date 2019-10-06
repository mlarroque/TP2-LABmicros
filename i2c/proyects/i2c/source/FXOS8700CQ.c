/*
 * 	FXOS8700CQ.c
 *
 *  Created on: 20 sep. 2019
 *      Author: Lu
 *
 *
 *  El siguiente código está basado
 *  en el código de ejemplo que se
 *  encuentra en la datasheet del FXOS8700CQ.
 *
 *
 *
 */


#include "FXOS8700CQ.h"
//#include "i2c.h"
#include "marc.h"
#include <stdio.h>


#define I2C_ACCELEROMETER 0


// FXOS8700CQ I2C address
#define FXOS8700CQ_SLAVE_ADDR 0x1D


// FXOS8700CQ internal register addresses
#define FXOS8700CQ_STATUS 		0x00
#define FXOS8700CQ_WHOAMI 		0x0D
#define FXOS8700CQ_XYZ_DATA_CFG 0x0E
#define FXOS8700CQ_CTRL_REG1 	0x2A
#define FXOS8700CQ_M_CTRL_REG1 	0x5B
#define FXOS8700CQ_M_CTRL_REG2 	0x5C
#define FXOS8700CQ_WHOAMI_VAL 	0xC7

// number of bytes to be read from the FXOS8700CQ
#define FXOS8700CQ_READ_LEN 	13 // uno de status, y 6*2 de data accel/magn


//PROTOTIPOS
static bool FXOS8700CQConfiguration(void);




//todo borrar funcion callback
void callback()
{
	uint8_t a=0;
	a++;
}


void FXOS8700CQInit(void)
{
	init_i2c0();			//todo borrar esto y descomentar abajo
//	i2cInit(I2C_ACCELEROMETER);
	FXOS8700CQConfiguration();

}


//configura el FXOS8700CQ para modo híbrido: lee acelerómetro y magnetómetro
bool FXOS8700CQConfiguration(void)
{
    uint8_t databyte;

    // read and check the FXOS8700CQ WHOAMI register
    i2c_read_reg(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_WHOAMI, &databyte,(uint8_t) 1, callback);
    if (databyte != FXOS8700CQ_WHOAMI_VAL)		//no responde bien el acelerómetro
    {
    	//return false;
    }


    // write 0000 0000 = 0x00 to accelerometer control register 1 to place FXOS8700CQ into standby
    // [7-1] = 0000 000
    // [0]: active=0
    databyte = 0x00;
    i2c_write_reg(FXOS8700CQ_SLAVE_ADDR,FXOS8700CQ_CTRL_REG1, &databyte,(uint8_t) 1, callback);


    // write 0001 1111 = 0x1F to magnetometer control register 1
    // [7]: m_acal=0: auto calibration disabled
    // [6]: m_rst=0: no one-shot magnetic reset
    // [5]: m_ost=0: no one-shot magnetic measurement
    // [4-2]: m_os=111=7: 8x oversampling (for 200Hz) to reduce magnetometer noise
    // [1-0]: m_hms=11=3: select hybrid mode with accel and magnetometer active
    databyte = 0x1F;
    i2c_write_reg(FXOS8700CQ_SLAVE_ADDR,FXOS8700CQ_M_CTRL_REG1,&databyte, (uint8_t) 1, callback);


    // write 0010 0000 = 0x20 to magnetometer control register 2
    // [5]: hyb_autoinc_mode=1 to map the magnetometer registers to follow the
    // accelerometer registers
    // [4]: m_maxmin_dis=0 to retain default min/max latching even though not used
    // [3]: m_maxmin_dis_ths=0
    // [2]: m_maxmin_rst=0
    // [1-0]: m_rst_cnt=00 to enable magnetic reset each cycle
    databyte = 0x20;
    i2c_write_reg(FXOS8700CQ_SLAVE_ADDR,FXOS8700CQ_M_CTRL_REG2,&databyte, (uint8_t) 1, callback);


    // write 0000 0001= 0x01 to XYZ_DATA_CFG register
    // [4]: hpf_out=0
    // [1-0]: fs=01 for accelerometer range of +/-4g range with 0.488mg/LSB
    databyte = 0x01;
    i2c_write_reg(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_XYZ_DATA_CFG, &databyte, (uint8_t) 1, callback);

    // write 0000 1101 = 0x0D to accelerometer control register 1
    // [7-6]: aslp_rate=00
    // [5-3]: dr=001 for 200Hz data rate (when in hybrid mode)
    // [2]: lnoise=1 for low noise mode
    // [1]: f_read=0 for normal 16 bit reads
    // [0]: active=1 to take the part out of standby and enable sampling
    databyte = 0x0D;
    i2c_write_reg(FXOS8700CQ_SLAVE_ADDR,FXOS8700CQ_CTRL_REG1, &databyte,(uint8_t) 1, callback);

    return true;
}





void ReadAccelMagnData(raw_data_type *pAccelData, raw_data_type *pMagnData, bool newDataReady)
{
	newDataReady = false;
    uint8_t buffer[FXOS8700CQ_READ_LEN]; // read buffer

    //leo 13 bytes, uno de status y doce de info, el de status no es relevante
    i2c_read_reg(FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_STATUS, buffer, FXOS8700CQ_READ_LEN,callback);

    //paso de 16bits a 14bits que son los que importan
    //acelerómetro
	pAccelData->x = (int16_t)(((buffer[1] << 8) | buffer[2])) >> 2;
	pAccelData->y = (int16_t)(((buffer[3] << 8) | buffer[4])) >> 2;
	pAccelData->z = (int16_t)(((buffer[5] << 8) | buffer[6])) >> 2;

	//idem para el magnetómetro
	pMagnData->x = (buffer[7] << 8) | buffer[8];
	pMagnData->y = (buffer[9] << 8) | buffer[10];
	pMagnData->z = (buffer[11] << 8) | buffer[12];

	newDataReady = true;
}

