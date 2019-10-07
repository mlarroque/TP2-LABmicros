/*
 * i2c.c
 *
 *  Created on: 20 sep. 2019
 *      Author: Lu
 */


#include "i2c.h"
#include "pinsHandler.h"
#include "MK64F12.h"
#include <stdint.h>

	 //ALT5 //ALT1 //ALT5
enum {I2C_0, I2C_1, I2C_2, I2C_CHANNEL_COUNT};

							//I2C0_SCL			//I2C1_SCL			//I2C2_SCL
uint8_t i2cSCLpins[] = {PORTNUM2PIN(PE, 24), PORTNUM2PIN(PC, 10), PORTNUM2PIN(PA, 14)};

							//I2C0_SDA			//I2C1_SDA			//I2C2_SDA
uint8_t i2cSDApins[] = {PORTNUM2PIN(PE, 25), PORTNUM2PIN(PC, 11), PORTNUM2PIN(PA, 13)};

//interrupt vector
uint8_t i2cIRQs[]= I2C_IRQS;


//ESTRUCTURAS y ENUMS

typedef enum {I2C_WRITE, I2C_READ}i2c_direction_type;

typedef enum{SEND_REPEAT_START, SEND_REGISTER, DATA}fsm_states;




typedef struct{

	bool finished;
	uint8_t	start_count;
	i2c_direction_type dir;

	uint8_t	slave;
	uint8_t	reg;
	uint8_t* data;
	uint8_t	data_size;

	fsm_states fsm_state;

}buffer_type;


//GLOBAL VARIABLES

static I2C_Type* i2c;
static bool init = false;
static buffer_type buffer;

callbackPtr callback2Fun;

//PROTOTIPOS
static  void sendStart(){
     i2c->C1 |= I2C_C1_MST_MASK;
}
static void sendStop(){
    i2c->C1 &= ~I2C_C1_MST_MASK;
}
static  void sendRepeatStart(){
	i2c->C1 |= I2C_C1_TX_MASK;
	i2c->C1 |= I2C_C1_RSTA_MASK;
}
//static  uint8_t	getInterruptFlag(){    return i2c->S & I2C_S_IICIF_MASK;}

static  uint8_t	getStopFlag(){    return i2c->FLT&I2C_FLT_STOPF_MASK;}

static  uint8_t	getStartFlag(){    return i2c->FLT&I2C_FLT_STARTF_MASK;}

static  void clearInterruptFlag(){    i2c->S|=I2C_S_IICIF_MASK;}

static  void clearStopFlag(){    i2c->FLT|=I2C_FLT_STOPF_MASK;}

static  void clearStartFlag(){    i2c->FLT|=I2C_FLT_STARTF_MASK;}

static  uint8_t	getMode(){    return (i2c->C1 & I2C_C1_TX_MASK);}

static  void setModeTX(){    i2c->C1 |= I2C_C1_TX_MASK;}

static  void setModeRX(){    i2c->C1 &= ~I2C_C1_TX_MASK;}

static  uint8_t isBusBusy(){    return i2c->S & I2C_S_BUSY_MASK;}

static  void setNack(){    i2c->C1 |= I2C_C1_TXAK_MASK;}

static  void unsetNack(){    i2c->C1 &= ~I2C_C1_TXAK_MASK;}

static  uint8_t	receivedAck(){    return ~(i2c->S & I2C_S_RXAK_MASK);}

static  void writeByte(uint8_t byte){	i2c->D=byte;}

static  uint8_t	readByte(){	return i2c->D;}



static void isr_routine(void);
static void setBaudRate();
static void configurePins(uint8_t channel);
static void clockGating(int8_t channel);


bool i2cInit(uint8_t channel)
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
		i2c->C1 = I2C_C1_IICIE_MASK | I2C_C1_IICEN_MASK;		//habilito modulo
		i2c->FLT |= I2C_FLT_SSIE_MASK;					//habilito interrupciones startf/stopf
		i2c->S = I2C_S_TCF_MASK | I2C_S_IICIF_MASK;		//habilito interrupciones

		init = true;
	}
	return init;
}



void i2cWriteReg(uint8_t slave, uint8_t reg, uint8_t* data, uint8_t data_size, callbackPtr callback)
{
	if(isBusBusy())
		return;
	buffer.finished = false;		//lleno la estructura para la transferencia
	buffer.dir = I2C_WRITE;
	buffer.fsm_state = SEND_REGISTER;
	buffer.start_count = 0;
	buffer.slave = slave;
	buffer.reg = reg;
	buffer.data = data;
	buffer.data_size = data_size;
	callback2Fun = callback;

	setModeTX();
	sendStart();		//mando start e inicia la transferencia
}

void i2cReadReg(uint8_t slave, uint8_t reg, uint8_t* data, uint8_t data_size, callbackPtr callback)
{
	if(isBusBusy())
		return;
	buffer.finished = false;
	buffer.dir = I2C_READ;
	buffer.fsm_state = SEND_REGISTER;
	buffer.start_count = 0;
	buffer.slave = slave;
	buffer.reg = reg;
	buffer.data = data;
	buffer.data_size = data_size;
	callback2Fun = callback;
	setModeTX();
	sendStart();		//mando start e inicia la transferencia
}


void isr_routine(void)	//este codigo sigue el diagrama 51.6 del reference manual
{
	if(getStopFlag())	//STOP
	{
		clearStopFlag();
		clearInterruptFlag();
		unsetNack();
		buffer.start_count=0;
		buffer.finished=true;
		callback2Fun();
	}
	else
	{
		if(getStartFlag())		//START
		{
			clearStartFlag();
			clearInterruptFlag();
			buffer.start_count++;
			if(buffer.start_count == 1)	//fue un START
			{
				writeByte(buffer.slave << 1 | 0);//mando address con 0 en LSB para write
				return;
			}
			else		//start count > 1 entonces fue REPEATED START
			{
				writeByte(buffer.slave << 1 | 1);	//mando address con 1 en LSB para read
				return;
			}
		}
		else
			clearInterruptFlag();

		if(getMode()== I2C_C1_TX_MASK)	//TX
		{
			if(!receivedAck())
			{
				sendStop();
				buffer.finished = true;		//??
				return;
			}

			if(buffer.dir == I2C_READ)
			{
				switch (buffer.fsm_state)	//mini event handler de maquina de estado
				{
					//si entro acá, es porque ya se mandó el start seguido del slave address en modo write

				case SEND_REGISTER:			//si tengo que enviar register
					writeByte(buffer.reg);
					buffer.fsm_state = SEND_REPEAT_START;	//actualizo el estado
					break;
				case SEND_REPEAT_START:				//mando repeat start seguido del slave address 	en modo read
					sendRepeatStart();
					buffer.fsm_state = DATA;
					return;
					break;
				case DATA:			//si tengo que enviar register
					setModeRX();					//paso a recibir
					if(buffer.data_size == 1)		//quedan dos bytes: el dummy, y la data, entonces mando nack
						setNack();
					uint8_t dummy = readByte();dummy++;
					buffer.fsm_state = DATA;
					break;
				}
			}
			else		//buffer.dir == WRITE
			{
				switch(buffer.fsm_state)
				{
				case SEND_REGISTER:
					writeByte(buffer.reg);
					buffer.fsm_state = DATA;
					break;
				case DATA:
					if(buffer.data_size == 0)		//si queda un byte por recibir
					{
						sendStop();					//mando stop
//						writeByte(buffer.data);
						return;
					}
					else
					{
						uint8_t data = *buffer.data;
						writeByte(data);
						buffer.data++;
						buffer.data_size--;
					}
					buffer.fsm_state = DATA;		//proximo estado será DATA
					break;
				default:
					break;
				}
			}
		}
		else	//modo RX
		{
			if(buffer.fsm_state == DATA)		//recibo bytes
			{
				if(buffer.data_size == 1)	//si queda uno por recibir
					sendStop();
				else if(buffer.data_size == 2)	//quedan dos por recibir, mando nak
					setNack();
				*buffer.data = readByte();		//guardo data
				buffer.data++;				//aumento puntero donde guardo data
				buffer.data_size--;			//decremento contador
			}
		}
	}
}



void setBaudRate()
{
	i2c->F = I2C_F_ICR(0x35) | I2C_F_MULT(0b10);
}

void configurePins(uint8_t channel)
{
	uint8_t i2c_mux = 5;
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

	setPCRmux(portPointers[port_SDA], num_pin_SDA, i2c_mux);
	setPCRmux(portPointers[port_SCL], num_pin_SCL, i2c_mux);
	setPCRirqc(portPointers[port_SDA], num_pin_SDA, IRQ_MODE_DISABLE); //deshabilito interrupciones de puerto
	setPCRirqc(portPointers[port_SCL], num_pin_SCL, IRQ_MODE_DISABLE);

	//seteo open drain enable como dice la filmina
	setPCRopenDrainEnable(portPointers[port_SDA], num_pin_SDA);
	setPCRpullEnable(portPointers[port_SDA], num_pin_SDA);
	setPCRpullUp(portPointers[port_SDA], num_pin_SDA);
	setPCRopenDrainEnable(portPointers[port_SCL], num_pin_SCL);
	setPCRpullEnable(portPointers[port_SCL], num_pin_SCL);
	setPCRpullUp(portPointers[port_SCL], num_pin_SCL);

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
