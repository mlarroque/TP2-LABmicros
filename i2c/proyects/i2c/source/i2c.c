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
#include <stdlib.h>

//MACROS

//#define I2C_BAUDRATE_DEFAULT 50000bps
#define I2C_BUFFER_SIZE 256 //tamaño del buffer
#define I2C_READ 1
//#define I2C_WRITE !I2C_READ
#define I2C_BUS_CLOCK 60000000
	 //ALT2//ALT1//ALT5
enum {I2C0, I2C1, I2C2, I2C_CHANNEL_COUNT};


//los valores del SCL dividers posibles para calcular el ICR y luego el baud-rate, que es el índice del arreglo
static const uint16_t i2cSCLDividers[] = {
		20,  22,  24,  26,   28,   30,   34,   40,   28,   32,   36,   40,   44,   48,   56,   68,
		48,  56,  64,  72,   80,   88,   104,  128,  80,   96,   112,  128,  144,  160,  192,  240,
		160, 192, 224, 256,  288,  320,  384,  480,  320,  384,  448,  512,  576,  640,  768,  960,
		640, 768, 896, 1024, 1152, 1280, 1536, 1920, 1280, 1536, 1792, 2048, 2304, 2560, 3072, 3840};





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


//IDLE;WAITING_FOR_ACK;

enum _i2c_status
{
    kStatus_I2C_Busy            = MAKE_STATUS(kStatusGroup_I2C, 0), /*!< I2C is busy with current transfer. */
    kStatus_I2C_Idle            = MAKE_STATUS(kStatusGroup_I2C, 1), /*!< Bus is Idle. */
    kStatus_I2C_Nak             = MAKE_STATUS(kStatusGroup_I2C, 2), /*!< NAK received during transfer. */
    kStatus_I2C_ArbitrationLost = MAKE_STATUS(kStatusGroup_I2C, 3), /*!< Arbitration lost during transfer. */
    kStatus_I2C_Timeout         = MAKE_STATUS(kStatusGroup_I2C, 4), /*!< Timeout poling status flags. */
    kStatus_I2C_Addr_Nak        = MAKE_STATUS(kStatusGroup_I2C, 5), /*!< NAK received during the address probe. */
};
enum _i2c_flags
{
    kI2C_ReceiveNakFlag        = I2C_S_RXAK_MASK,  /*!< I2C receive NAK flag. */
    kI2C_IntPendingFlag        = I2C_S_IICIF_MASK, /*!< I2C interrupt pending flag. */
    kI2C_TransferDirectionFlag = I2C_S_SRW_MASK,   /*!< I2C transfer direction flag. */
    kI2C_RangeAddressMatchFlag = I2C_S_RAM_MASK,   /*!< I2C range address match flag. */
    kI2C_ArbitrationLostFlag   = I2C_S_ARBL_MASK,  /*!< I2C arbitration lost flag. */
    kI2C_BusBusyFlag           = I2C_S_BUSY_MASK,  /*!< I2C bus busy flag. */
    kI2C_AddressMatchFlag      = I2C_S_IAAS_MASK,  /*!< I2C address match flag. */
    kI2C_TransferCompleteFlag  = I2C_S_TCF_MASK,   /*!< I2C transfer complete flag. */
#ifdef I2C_HAS_STOP_DETECT
    kI2C_StopDetectFlag = I2C_FLT_STOPF_MASK << 8, /*!< I2C stop detect flag. */
#endif /* FSL_FEATURE_I2C_HAS_START_STOP_DETECT / FSL_FEATURE_I2C_HAS_STOP_DETECT */

#if defined(FSL_FEATURE_I2C_HAS_START_STOP_DETECT) && FSL_FEATURE_I2C_HAS_START_STOP_DETECT
    kI2C_StartDetectFlag = I2C_FLT_STARTF_MASK << 8, /*!< I2C start detect flag. */
#endif                                               /* FSL_FEATURE_I2C_HAS_START_STOP_DETECT */
};
typedef enum{
	uint8_t start_count;
	enum _i2c_transfer_states
	{
	    kIdleState             = 0x0U, /*!< I2C bus idle. */
	    kCheckAddressState     = 0x1U, /*!< 7-bit address check state. */
	    kSendCommandState      = 0x2U, /*!< Send command byte phase. */
	    kSendDataState         = 0x3U, /*!< Send data transfer phase. */
	    kReceiveDataBeginState = 0x4U, /*!< Receive data transfer phase begin. */
	    kReceiveDataState      = 0x5U, /*!< Receive data transfer phase. */
	};
}i2c_FSM_type;
typedef struct _i2c_master_transfer
{
    uint32_t flags;            /*!< A transfer flag which controls the transfer. */
    uint8_t slaveAddress;      /*!< 7-bit slave address. */
    i2c_direction_t direction; /*!< A transfer direction, read or write. */
    uint32_t subaddress;       /*!< A sub address. Transferred MSB first. */
    uint8_t subaddressSize;    /*!< A size of the command buffer. */
    uint8_t *volatile data;    /*!< A transfer buffer. */
    volatile size_t dataSize;  /*!< A transfer size. */
} i2c_master_transfer_t;


//GLOBAL VARIABLES

I2C_BUFFER i2c_tx_buffer;   //I2c TX buffer
I2C_BUFFER i2c_rx_buffer;   //i2c RX buffer


//PROTOTIPOS

//Initializes the specified I2C channel. Address mode is 7-bit
//Parameters:
//channel: I2C channel number specified by user
//addr: primary slave address
//bps: baud rate (Hz)
//Return value:
//none
void i2cInit(uint8_t channel, i2c_conf_type conf)
{
	init = false;
	if(channel < I2C_CHANNEL_COUNT)
	{
		I2C_Type * i2c_ptrs[] = I2C_BASE_PTRS;
		I2C_Type * i2c = i2c_ptrs[channel];

		i2cClockGating(channel);
		i2cConfigurePins(channel);
		i2cSetBaudRate(conf.baud_rate, i2c);

		//interrupciones
		//NVIC_EnableIRQ(i2cIRQs(channel));
		//callback de interrupcion??

		if(conf.)
		{



			i2c->C1 |= I2C_C1_IICEN_MASK;

			init = true;
		}
	}
	return init;
}

void i2cDeInit(uint8_t channel)
{
	I2C_Type * i2c = i2c_ptrs[channel];
    i2c->C1 &= ~(uint8_t)I2C_C1_IICEN_MASK;		//borro el bit que habilita interrupciones
}

//General function for performing I2C master transfers.
//Prototype:
//void i2c_master(uint8 channel, uint8 mode, uint16 slave_address)
//Parameters:
//channel: I2C channel number specified by user
//mode: Valid modes include I2C_TX, I2C_RX//, I2C_TXRX, I2C_10BIT_TX, I2C_10BIT_RX, I2C_10BIT_TXRX (all modes defined in i2c.h)
//slave_address: the slave address
//Return value:
//none
void i2cSetBaudRate(uint32_t baud_rate,I2C_Type * i2c)
{
	//i2c usa bus clock que es 60MHz
	uint32_t clock = I2C_BUS_CLOCK;
	uint32_t calculated_baud_rate;
	uint32_t mult = 1;
	uint32_t icr;
	uint32_t best_error = UINT32_MAX;
	uint32_t error = best_error;
	uint8_t = i,j;
	for(j = 0; j <= 2; j++)
	{
		mult = 1 << j; 	//1, 2, o 4, que son los mutiplicadores

		for(i=0; i < (sizeof(i2cSCLDividers)/sizeof(uint16_t)); i++)		//busco el menor error en todo el arreglo
		{
			calculated_baud_rate = clock / (mult * i2cSCLDividers[i]);
			error = abs(baud_rate-calculated_baud_rate);

			if (error < best_error)
			{
				best_error = error;
				mult  = j;
				icr   = i;

				if (error == 0)	//si el error es ideal, dejo de buscar
					break;
			}
		}
	}

	//seteo baud_rate

	i2c->F = I2C_F_MULT(mult) | I2C_F_ICR(icr);

}

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
	return;
}
