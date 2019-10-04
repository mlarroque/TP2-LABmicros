/*
 * i2c.c
 *
 *  Created on: 20 sep. 2019
 *      Author: Lu
 */


#include "i2c.h"
#include "pinsHandler.h"
#include "MK64F12.h"
#include "hardware.h"
#include <stdint.h>
#include <stdlib.h>


//MACROS

//#define I2C_BAUDRATE_DEFAULT 50000bps
#define buffer_SIZE 255 //tamaño del buffer


	 //ALT1//ALT1//ALT5
enum {I2C_0, I2C_1, I2C_2, I2C_CHANNEL_COUNT};

							//I2C0_SCL			//I2C1_SCL			//I2C2_SCL
uint8_t i2cSCLpins[] = {PORTNUM2PIN(PE, 24), PORTNUM2PIN(PC, 10), PORTNUM2PIN(PA, 14)};

							//I2C0_SDA			//I2C1_SDA			//I2C2_SDA
uint8_t i2cSDApins[] = {PORTNUM2PIN(PE, 25), PORTNUM2PIN(PC, 11), PORTNUM2PIN(PA, 13)};

//interrupt vector
uint8_t i2cIRQs[]= I2C_IRQS;


//ESTRUCTURAS

typedef struct{
	bool finished;
	uint8_t*	rx_buff;
	uint8_t*	tx_buff;

	uint8_t	tx_size;
	uint8_t	rx_size;

	uint8_t	slave;
	uint8_t	index;
	uint8_t	r_start;
	i2c_direction_type dir;
	uint8_t	reg;
	uint8_t	reg_size;
}buffer_type;

//GLOBAL VARIABLES

static I2C_Type* i2c;
static bool init=false;
static buffer_type buffer;


//PROTOTIPOS
static  void sendStart(){
     i2c->C1 |= I2C_C1_MST_MASK;
}
static void sendStop(){
    i2c->C1 &= ~I2C_C1_MST_MASK;
}
static  void sendRepeatStart(){
	i2c->FLT &= ~I2C_FLT_SSIE_MASK;
	i2c->C1 |= I2C_C1_TX_MASK;
	i2c->C1 |= I2C_C1_RSTA_MASK;
}
static  uint8_t	getInterruptFlag(){
    return i2c->S & I2C_S_IICIF_MASK;
}
static  void clearInterruptFlag(){
    i2c->S|=I2C_S_IICIF_MASK;
}
static  uint8_t	getMode(){
    return (i2c->C1 & I2C_C1_TX_MASK);
}
static  void setModeTX(){
    i2c->C1 |= I2C_C1_TX_MASK;
}
static  void setModeRX(){
    i2c->C1 &= ~I2C_C1_TX_MASK;
}
static  uint8_t	isBusBusy(){
    return i2c->S & I2C_S_BUSY_MASK;
}
static  void setNack(){
    i2c->C1 |= I2C_C1_TXAK_MASK;
}
static  void unsetNack(){
    i2c->C1 &= ~I2C_C1_TXAK_MASK;
}
static  uint8_t	receivedAck(){
    return ~(i2c->S & I2C_S_RXAK_MASK);
}
static  void writeByte(uint8_t byte){
    i2c->D=byte;
}
static  uint8_t	readByte(){
    return i2c->D;
}

static bool isr_routine(void);
void setBaudRate(I2C_Type * i2c);
void configurePins(uint8_t channel);
void clockGating(int8_t channel);
//void clearInterruptFlag(I2C_Type * i2c);
void masterRead(uint8_t* data, uint8_t data_size);
void i2cMasterWrite(uint8_t* data, uint8_t data_size/*, bool repeated_start*/);
void i2cResetModule(I2C_Type * i2c);
bool masterTransfer();


bool i2cInit(uint8_t channel, i2c_conf_type conf)
{
	if((channel < I2C_CHANNEL_COUNT) && (init==false))		//chequeo que sea valido el I2C elegido
	{
		I2C_Type * i2c_ptrs[] = I2C_BASE_PTRS;
		i2c = i2c_ptrs[channel];

		clockGating(channel);
		configurePins(channel);
//		resetModule(i2c);	//reseteo el modulo: deshabilito el flag de interrupciones, borro registros, etc
		setBaudRate(i2c);

		NVIC_EnableIRQ(i2cIRQs[channel]);
		i2c->C1 = 0;
		i2c->C1 = I2C_C1_IICIE_MASK | I2C_C1_IICEN_MASK;
//		i2c->FLT |= I2C_FLT_SSIE_MASK;
		i2c->S = I2C_S_TCF_MASK | I2C_S_IICIF_MASK;
		init = true;
	}
	return init;
}


void i2cTransferInit(i2c_transfer_type t)
{
	buffer.finished = false;
	buffer.rx_buff = t.data;
	buffer.tx_buff = t.data;

	buffer.tx_size = t.data_size;
	buffer.rx_size = t.data_size;

	buffer.slave = t.slave_address;
	buffer.index = 0;
	buffer.r_start = t.repeat_start;
	buffer.dir = t.dir;

	buffer.reg = t.reg;
	buffer.reg_size = t.reg_size;


	if(isBusBusy())
		return;
    setModeTX();
    sendStart();
    writeByte(t.slave_address << 1 | 0);
	if(!receivedAck())
		return false;
}

void i2cTransferNonBlocking(i2c_transfer_type t)
{
	i2cTransferInit(t);
}

bool i2cTransferBlocking(i2c_transfer_type t)
{
	sendStop();
	if(isBusBusy())
		return I2C_ERROR;
	else
	{
		i2cTransferInit(t);
		while(!buffer.finished)
			if(masterTransfer());
	}
	return I2C_OK;
}

void i2cMasterWrite(uint8_t* data, uint8_t data_size/*, bool repeated_start*/)
{
	setModeTX();
	while(data_size > 0)
	{
		uint8_t d=*data;
		writeByte(d);	//mando byte de datos
		data++;
		data_size--;
	}

	if(data_size == 0)					//si ya envié toda la data
	{
		setModeRX();
		setNack();
		sendStop();
		buffer.finished = true;
	}
}

void i2cMasterRead(uint8_t* data, uint8_t data_size)
{
	uint8_t dummy_read = 0;
//	unsetNack();
//	dummy_read=readByte();

	while(data_size > 0)
	{
		if(data_size == 1)
		{
			setNack();
//			sendStop();
//			setModeTX();
		}
		else
			unsetNack();

		*buffer.rx_buff = readByte();
		buffer.rx_buff++;
		data_size--;
	}

	if(data_size == 0)
	{
		setNack();
		setModeRX();
		sendStop();
		buffer.finished = true;
	}
}


bool masterTransfer()
{
	if(buffer.finished)
		return false;
	while(!receivedAck())
	{
	}
	clearInterruptFlag();

	if(buffer.reg_size > 0)
	{
		buffer.reg_size--;
		writeByte(buffer.reg);
		while(!receivedAck())
		{
		}
		clearInterruptFlag();

		if(buffer.dir == I2C_READ)
		{
//			setModeTX();
			sendRepeatStart();
			writeByte(buffer.slave << 1 | 1);
			while(!receivedAck())
			{
			}
			setModeRX();
//			if(buffer.rx_size == 1)
//				setNack();
			uint8_t dummy = readByte();

		}
	}
	else
	{

		//recibo data
		if(buffer.dir == I2C_READ)// && (buffer.rx_size> 0))
		{
			i2cMasterRead(buffer.rx_buff, buffer.rx_size);
		}

		//transmito data
		if(buffer.dir == I2C_WRITE)// && (buffer.tx_size > 0))
		{
			i2cMasterWrite(buffer.tx_buff, buffer.tx_size);
		}
		clearInterruptFlag();
	}
	return true;
}


bool isr_routine(void)
{
	if(getMode() == I2C_C1_TX_MASK)
	{
		if(buffer.finished)
			sendStop(i2c);
		else	//quedan cosas para mandar
		{
			if(!receivedAck())
				sendStop();
			else		//llegó ack
			{
				if(buffer.index == buffer.tx_size)
				{
					if(buffer.rx_size > 0)		//quiero recibir, entonces cambio de modo
					{
						setModeRX(i2c);
						if(buffer.index == (buffer.rx_size-1))
							setNack();
						else
							unsetNack();
						buffer.rx_buff[buffer.index] = readByte();		//dummy read
					}
					else
					{
						buffer.finished = true;
						sendStop();
					}
				}
				else
				{
					if(buffer.index == buffer.r_start) //buffer.repeat_start == 1 ??
					{
						sendRepeatStart();
						writeByte(buffer.slave << 1 | 1); //esto va?
					}
					else
					{
						writeByte(buffer.tx_buff[buffer.index]);
						buffer.index++;
					}
				}

			}
		}
	}
	else	//RX
	{
		if(buffer.index == buffer.rx_size)
		{
			sendStop();
			buffer.finished = true;
		}
		else if(buffer.index == (buffer.rx_size-1))
			setNack();
		else
			unsetNack();
		buffer.rx_buff[buffer.index++] = readByte();
	}
	clearInterruptFlag(i2c);
	return true;
}


static void resetModule(I2C_Type * i2c)
{
	i2c->C1 &= ~(uint8_t)(I2C_C1_IICEN_MASK); 	//deshabilito interrupciones
	//borro los registros que voy a usar
	i2c->A1 = 0x00; 		//address register
	i2c->F = 0x00;		//freq divider register
	i2c->C1 = I2C_C1_IICIE_MASK | I2C_C1_IICEN_MASK;	//control reg
	i2c->S = I2C_S_TCF_MASK | I2C_S_IICIF_MASK;		//status reg
}



void setBaudRate(I2C_Type * i2c)
{
	i2c->F = I2C_F_ICR(0x25) | I2C_F_MULT(0x0);
}

void configurePins(uint8_t channel)
{
	uint8_t i2c_mux=1;
	switch(channel)
	{
	case I2C_0:
		i2c_mux = 5;
		break;
	case I2C_1:
		i2c_mux = 1;
		break;
	case I2C_2:
		i2c_mux = 5;
		break;
	}
	PORT_Type * portPointers[] = PORT_BASE_PTRS;

	uint8_t port_SDA = PIN2PORT(i2cSDApins[channel]);
	uint8_t num_pin_SDA = PIN2NUM(i2cSDApins[channel]);

	uint8_t port_SCL = PIN2PORT(i2cSCLpins[channel]);
	uint8_t num_pin_SCL = PIN2NUM(i2cSCLpins[channel]);

	portPointers[PE]->PCR[24] |= PORT_PCR_MUX(5);
	portPointers[PE]->PCR[25] |= PORT_PCR_MUX(5);
//	setPCRmux(portPointers[port_SDA], num_pin_SDA, i2c_mux);
//	setPCRmux(portPointers[port_SCL], num_pin_SCL, i2c_mux);
	setPCRirqc(portPointers[port_SDA], num_pin_SDA, IRQ_MODE_DISABLE); //deshabilito interrupciones de puerto
	setPCRirqc(portPointers[port_SCL], num_pin_SCL, IRQ_MODE_DISABLE);

	//seteo open drain enable como dice la filmina
	portPointers[PE]->PCR[24] |= PORT_PCR_ODE_MASK;
	portPointers[PE]->PCR[25] |= PORT_PCR_ODE_MASK;
	portPointers[PE]->PCR[24] |= PORT_PCR_PE_MASK;
	portPointers[PE]->PCR[25] |= PORT_PCR_PE_MASK;
	portPointers[PE]->PCR[24] |= PORT_PCR_PS_MASK;
	portPointers[PE]->PCR[25] |= PORT_PCR_PS_MASK;

//	setPCRopenDrainEnable(portPointers[port_SDA], num_pin_SDA);
//	setPCRpullEnable(portPointers[port_SDA], num_pin_SDA);
//	setPCRpullUp(portPointers[port_SDA], num_pin_SDA);
//	setPCRopenDrainEnable(portPointers[port_SCL], num_pin_SCL);
//	setPCRpullEnable(portPointers[port_SCL], num_pin_SCL);
//	setPCRpullUp(portPointers[port_SCL], num_pin_SCL);

}

void clockGating(int8_t channel)
{
	SIM_Type * sim = SIM;
	switch(channel)
	{
		case I2C_0:
			sim->SCGC4 |= SIM_SCGC4_I2C0_MASK;
			sim->SCGC5 |= SIM_SCGC5_PORTE_MASK;
			break;
		case I2C_1:
			sim->SCGC4 |= SIM_SCGC4_I2C1_MASK;
			sim->SCGC5 |= SIM_SCGC5_PORTC_MASK;
			break;
		case I2C_2:
			sim->SCGC1 |= SIM_SCGC1_I2C2_MASK;
			sim->SCGC5 |= SIM_SCGC5_PORTA_MASK;
			break;
	}
}


void I2C0_IRQHandler(void)
{
	isr_routine();
}

void I2C1_IRQHandler(void)
{
	isr_routine();
}

void I2C2_IRQHandler(void)
{
	isr_routine();
}
