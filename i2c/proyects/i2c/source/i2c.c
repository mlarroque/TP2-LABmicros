/*
 * i2c.c
 *
 *  Created on: 20 sep. 2019
 *      Author: Lu
 */


#include "i2c.h"
#include "pinsHandler.h"




//MACROS
#define I2C_BUFFER_SIZE xxxx //tamaño del buffer

	 //ALT2//ALT1//ALT5
enum {I2C0, I2C1, I2C2, I2C_CHANNEL_COUNT};


							//I2C0_SCL			//I2C1_SCL			//I2C2_SCL
uint8_t i2cSCLpins[] = {PORTNUM2PIN(PB, 2), PORTNUM2PIN(PC, 10), PORTNUM2PIN(PA, 14)};

							//I2C0_SDA			//I2C1_SDA			//I2C2_SDA
uint8_t i2cSDApins[] = {PORTNUM2PIN(PB, 3), PORTNUM2PIN(PC, 11), PORTNUM2PIN(PA, 13)};

//interrupt vector
uint8_t i2cIRQs[]= I2C_IRQS;




//ESTRUCTURAS

/* Structure for storing I2C transfer data */
typedef struct {
    int tx_index; /* TX index */
    int rx_index; /* RX index */
    int data_present; /* Data present flag */
    uint16 length; /* Length of the buffer in bytes */
    uint8 buf[I2C_BUFFER_SIZE];/* Data buffer */
} I2C_BUFFER;   //llenarla antes de transferir



typedef enum{
	uint8_t start_count;

}i2c_FSM_type;



//GLOBAL VARIABLES

I2C_BUFFER i2c_tx_buffer;   //I2c TX buffer
I2C_BUFFER i2c_rx_buffer;   //i2c RX buffer


//PROTOTIPOS

//Initializes the specified I2C channel. Address mode is 7-bit
Parameters:
channel: I2C channel number specified by user
addr: primary slave address
bps: baud rate (Hz)
Return value:
none
void i2cInit(uint8_t channel, uint8_t slave_address, uint32_t baud_rate)
{
	I2C_Type * i2c_ptrs[] = I2C_BASE_PTRS;
	I2C_Type * i2c = i2c_ptrs[channel];

	i2cClockGating(channel);
	i2cConfigurePins(channel);
	//baud_rate

	//interrupciones
	//NVIC_EnableIRQ(i2cIRQs(channel));

}


//General function for performing I2C master transfers.
Prototype:
void i2c_master(uint8 channel, uint8 mode, uint16 slave_address)
Parameters:
channel: I2C channel number specified by user
mode: Valid modes include I2C_TX, I2C_RX//, I2C_TXRX, I2C_10BIT_TX, I2C_10BIT_RX, I2C_10BIT_TXRX (all modes defined in i2c.h)
slave_address: the slave address
Return value:
none


void i2cConfigurePins(uint8_t channel)
{
	uint8_t i2c_mux;
	switch(channel)
	{
	case I2C0:
		i2c_mux = 2;
		break;
	case I2C1:
		i2c_mux = 1;
		break;
	case I2C2:
		i2c_mux = 5;
		break;
	}
	PORT_Type * portPointers[] = PORT_BASE_PTRS;

	uint8_t port_SDA = PIN2PORT(i2cSDApins[channel]);
	uint8_t num_pin_SDA = PIN2NUM(i2cSDApins[channel]);

	uint8_t port_SCL = PIN2PORT(i2cSCLpins[channel]);
	uint8_t num_pin_SCL = PIN2NUM(i2cSCLpins[channel]);


	setPCRmux(portPointers[port_SDA], num_pin_SDA, i2c_mux);
	setPCRmux(portPointers[port_SCL], num_pin_SCL, i2c_mux);
	setPCRirqc(portPointers[port_SDA], num_pin_SDA, DISABLE_MODE); //deshabilito interrupciones de puerto
	setPCRirqc(portPointers[port_SCL], num_pin_SCL, DISABLE_MODE);

}

void i2cClockGating(uint8_t channel)
{
	SIM_Type * sim = SIM;
	switch(channel)
	{
		case I2C0:
			sim->SCGC4 |= SIM_SCGC4_I2C0_MASK;
			break;
		case I2C1:
			sim->SCGC4 |= SIM_SCGC4_I2C1_MASK;
			break;
		case I2C2:
			sim->SCGC1 |= SIM_SCGC1_I2C2_MASK;
			break;
		default:
			break;
	}
}


void initSlave()
{
//	Module Initialization (Slave)
//	1. Write: Control Register 2
//		• to enable or disable general call
//		• to select 10-bit or 7-bit addressing mode
//	2. Write: Address Register 1 to set the slave address
//	3. Write: Control Register 1 to enable the I2C module and interrupts
//	4. Initialize RAM variables (IICEN = 1 and IICIE = 1) for transmit data
//	5. Initialize RAM variables used to achieve the routine shown in the following figure
}
