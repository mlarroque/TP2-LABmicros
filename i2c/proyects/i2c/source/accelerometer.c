/*
 * accelerometer.c
 *
 *  Created on: 20 sep. 2019
 *      Author: Lu
 */


#include "accelerometer.h"
#include "i2c.h"
#include <stdio.h>


#define I2C_ACCELEROMETER 0
#define B_RATE 50000


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
#define FXOS8700CQ_READ_LEN 	13 // status plus 6 channels = 13 bytes



static bool I2C_WriteAccelReg(uint8_t i2c, uint8_t device_addr, uint8_t reg_addr, uint8_t* data, uint8_t size);
static bool I2C_ReadAccelRegs(uint8_t i2c, uint8_t device_addr, uint8_t reg_addr, uint8_t *rxBuff, uint32_t rxSize);

#define ACCEL_READ_TIMES 10U


void accelandMagnetInit(void)
{
	i2c_conf_type conf;
	conf.baud_rate = B_RATE;

	//conf.polling_mode = false;
//	conf.prim_slave_address = FXOS8700CQ_SLAVE_ADDR[0];
	i2cInit(I2C_ACCELEROMETER, conf);

	//inicializo registros del acelerometro y magnetometro

}

/*
raw_data_type* readAccelandMagnetValue(void)
{
	uint8_t who_am_i_reg = FXOS8700CQ_WHOAMI;

	i2c_transfer_type transfer;
	transfer.slave_address = FXOS8700CQ_SLAVE_ADDR[0];
	transfer.direction = I2C_READ;
	transfer.data = &who_am_i_reg;
	transfer.data_size = 1;
	transfer.repeated_start = true;

	trans_complete = i2cTransfer(I2C_ACCELEROMETER, transfer);

	if(trans_complete)

	i2c_direction_type direction;		//si transmito, recibo, o transmito seguido de recibo
		uint8_t slave_address;		//con qué slave quiero comunicarme
		uint8_t subaddress;			//a qué registro/ slave address me quiero comunicar
		//con cantidad de bytes o un puntero al arreglo a transmitir me parece mejor
		uint8_t* data;		//de N bytes
		uint8_t data_size;
	//	uint8_t N;		//cantidad de bytes que transmito
	//	uint8_t* data_rx;		//de M bytes
	//	uint8_t M;		//cantidad de bytes a leer
		bool repeated_start;
}*/

static bool I2C_WriteAccelReg(uint8_t i2c, uint8_t device_addr, uint8_t reg_addr, uint8_t* data, uint8_t size)
{
	i2c_transfer_type transfer;

	transfer.dir = I2C_WRITE;
    transfer.slave_address = device_addr;
    transfer.reg	= reg_addr;
    transfer.reg_size = 1;
	transfer.data = data;
	transfer.data_size = size;
	transfer.repeat_start = false;

	i2cTransferBlocking(transfer);

    return true;
}

static bool I2C_ReadAccelRegs(uint8_t i2c, uint8_t device_addr, uint8_t reg_addr, uint8_t *databyte, uint32_t dataSize)
{
	i2c_transfer_type transfer;

	transfer.dir = I2C_READ;
    transfer.slave_address   = device_addr;
    transfer.reg	= reg_addr;
    transfer.reg_size= 1;
    transfer.data = databyte;
    transfer.data_size = dataSize;
    transfer.repeat_start = true;


    /*  direction=write : start+device_write;cmdbuff;xBuff; */
    /*  direction=receive : start+device_write;cmdbuff;repeatStart+device_read;xBuff; */

    i2cTransferBlocking(transfer);

    return true;
}


#define I2C_OK	66
#define I2C_ERROR	99


// function configures FXOS8700CQ combination accelerometer and magnetometer sensor
uint8_t _mqx_ints_FXOS8700CQ_start()
{
    uint8_t databyte;
    // read and check the FXOS8700CQ WHOAMI register
    if (I2C_ReadAccelRegs(I2C_ACCELEROMETER, FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_WHOAMI, &databyte,(uint8_t) 1) != 1)
    {
        return (I2C_ERROR);
    }
    if (databyte != FXOS8700CQ_WHOAMI_VAL)
    {
        return (I2C_ERROR);
    }
    // write 0000 0000 = 0x00 to accelerometer control register 1 to place FXOS8700CQ into
    // standby
    // [7-1] = 0000 000
    // [0]: active=0
    databyte = 0x00;
    if (I2C_WriteAccelReg(I2C_ACCELEROMETER, FXOS8700CQ_SLAVE_ADDR,FXOS8700CQ_CTRL_REG1, &databyte,(uint8_t) 1) != 1)
    {
        return (I2C_ERROR);
    }
    // write 0001 1111 = 0x1F to magnetometer control register 1
    // [7]: m_acal=0: auto calibration disabled
    // [6]: m_rst=0: no one-shot magnetic reset
    // [5]: m_ost=0: no one-shot magnetic measurement
    // [4-2]: m_os=111=7: 8x oversampling (for 200Hz) to reduce magnetometer noise
    // [1-0]: m_hms=11=3: select hybrid mode with accel and magnetometer active
    databyte = 0x1F;

    if (I2C_WriteAccelReg(I2C_ACCELEROMETER, FXOS8700CQ_SLAVE_ADDR,FXOS8700CQ_M_CTRL_REG1,&databyte, (uint8_t) 1) != 1)
    {
        return (I2C_ERROR);
    }
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
    if (I2C_WriteAccelReg(I2C_ACCELEROMETER, FXOS8700CQ_SLAVE_ADDR,FXOS8700CQ_M_CTRL_REG2,&databyte, (uint8_t) 1) != 1)
    {
        return (I2C_ERROR);
    }
    // write 0000 0001= 0x01 to XYZ_DATA_CFG register
    // [7]: reserved
    // [6]: reserved
    // [5]: reserved
    // [4]: hpf_out=0
    // [3]: reserved
    // [2]: reserved
    // [1-0]: fs=01 for accelerometer range of +/-4g range with 0.488mg/LSB
    databyte = 0x01;
    if (I2C_WriteAccelReg(I2C_ACCELEROMETER, FXOS8700CQ_SLAVE_ADDR, FXOS8700CQ_XYZ_DATA_CFG, &databyte, (uint8_t) 1) != 1)
    {
        return (I2C_ERROR);
    }
    // write 0000 1101 = 0x0D to accelerometer control register 1
    // [7-6]: aslp_rate=00
    // [5-3]: dr=001 for 200Hz data rate (when in hybrid mode)
    // [2]: lnoise=1 for low noise mode
    // [1]: f_read=0 for normal 16 bit reads
    // [0]: active=1 to take the part out of standby and enable sampling
    databyte = 0x0D;

    if (I2C_WriteAccelReg(I2C_ACCELEROMETER, FXOS8700CQ_SLAVE_ADDR,FXOS8700CQ_CTRL_REG1, &databyte,(uint8_t) 1) != 1)
    {
        return (I2C_ERROR);
    }
    // normal return
        return (I2C_OK);
}




// read status and the three channels of accelerometer and magnetometer data from
// FXOS8700CQ (13 bytes)
int16_t ReadAccelMagnData(SRAWDATA *pAccelData, SRAWDATA *pMagnData)
{

    uint8_t Buffer[FXOS8700CQ_READ_LEN]; // read buffer
    // read FXOS8700CQ_READ_LEN=13 bytes (status byte and the six     channels of data)
    if (I2C_ReadAccelRegs(I2C_ACCELEROMETER, FXOS8700CQ_SLAVE_ADDR,    FXOS8700CQ_STATUS, Buffer,    FXOS8700CQ_READ_LEN) == 1)
    {
        // copy the 14 bit accelerometer byte data into 16 bit words
        pAccelData->x = (int16_t)(((Buffer[1] << 8) | Buffer[2]))>> 2;
        pAccelData->y = (int16_t)(((Buffer[3] << 8) | Buffer[4]))>> 2;
        pAccelData->z = (int16_t)(((Buffer[5] << 8) | Buffer[6]))>> 2;
        // copy the magnetometer byte data into 16 bit words
        pMagnData->x = (Buffer[7] << 8) | Buffer[8];
        pMagnData->y = (Buffer[9] << 8) | Buffer[10];
        pMagnData->z = (Buffer[11] << 8) | Buffer[12];
    }
    else
    {
        // return with error
        return (I2C_ERROR);
    }
    // normal return
    return (I2C_OK);
}

