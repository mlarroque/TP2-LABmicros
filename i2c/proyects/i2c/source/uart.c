/*
 * uart.c
 *
 *  Created on: 18 sep. 2019
 *      Author: G5
 */

#include "uart.h"
#include "pinsHandler.h"
#include "hardware.h"

#define MAX_BAUD_RATE 0x1FFF
#define MIN_BAUD_RATE 0x0000
#define BAUD_RATE_DEFAULT 9600

#define DISABLE_MODE 0 //disable port interrupts for uart's pins

#define UART_MUX 3

enum {U0, U1, U2, U3, U4};
							//RX_0				//RX_1				//RX_2					//RX_3				//RX_4
uint8_t uartRXpins[] = {PORTNUM2PIN(PB, 16), PORTNUM2PIN(PC, 3), PORTNUM2PIN(PD, 2), PORTNUM2PIN(PC, 16), PORTNUM2PIN(PE, 24)};

							//TX_0				//TX_1				//TX_2				//TX_3				//TX_4
uint8_t uartTXpins[] = {PORTNUM2PIN(PB, 17), PORTNUM2PIN(PC, 4), PORTNUM2PIN(PD, 3), PORTNUM2PIN(PC, 17), PORTNUM2PIN(PE, 25)};


uint8_t uartIRQs_TX_RX[] = UART_RX_TX_IRQS;
uint8_t uartIRQs_ERR[] = UART_ERR_IRQS;
uint8_t uartIRQS_LON[] = UART_LON_IRQS;

void UART_clockGating(uint8_t id);
void UART_setBaudRate(UART_Type * p2uart, uint32_t baudRate);

void UARTX_LON_IRQHandler(uint8_t id);
void UARTX_RX_TX_IRQHandler(uint8_t id);
void UARTX_ERR_IRQHandler(uint8_t id);

void UART0_LON_IRQHandler(void);
void UART0_RX_TX_IRQHandler(void);
void UART0_ERR_IRQHandler(void);
void UART1_RX_TX_IRQHandler(void);
void UART1_ERR_IRQHandler(void);
void UART2_RX_TX_IRQHandler(void);
void UART2_ERR_IRQHandler(void);
void UART3_RX_TX_IRQHandler(void);
void UART3_ERR_IRQHandler(void);
void UART4_RX_TX_IRQHandler(void);
void UART4_ERR_IRQHandler(void);

void uartInit (uint8_t id, uart_cfg_t config)
{
	UART_Type * p2uartsArray[] = UART_BASE_PTRS;
	UART_Type * p2uart;
	PORT_Type * portPointers[] = PORT_BASE_PTRS;
	uint8_t portRX, portTX, numPinRX, numPinTX;

	if ((id >= 0) && (id < UART_CANT_IDS)) //solo se hace algo si el id es válido
	{
		p2uart = p2uartsArray[id];
		UART_clockGating(id);
		UART_setBaudRate(p2uart, config.baudRate);

		portRX = PIN2PORT(uartRXpins[id]);
		numPinRX = PIN2NUM(uartRXpins[id]);
		portTX = PIN2PORT(uartTXpins[id]);
		numPinTX = PIN2PORT(uartTXpins[id]);

		setPCRmux(portPointers[portRX], numPinRX, UART_MUX);
		setPCRmux(portPointers[portTX], numPinTX, UART_MUX);
		setPCRirqc(portPointers[portRX], numPinRX, DISABLE_MODE); //deshabilito interrupciones de puerto, para que transmitir o
		setPCRirqc(portPointers[portTX], numPinTX, DISABLE_MODE); //recibir no me ocasione interrupción de puerto

		p2uart->C2 |= (UART_C2_TE_MASK | UART_C2_RE_MASK);

//		if(config.mode == NON_BLOCKING_SIMPLE)
		{
			p2uart->C2 |= UART_C2_RIE_MASK;
			NVIC_EnableIRQ(uartIRQs_TX_RX[id]);
			NVIC_EnableIRQ(uartIRQs_ERR[id]);
			if(id == U0)
			{
				NVIC_EnableIRQ(uartIRQS_LON[id]);
			}
		}
//		else if(config.mode == NON_BLOCKING_FIFO)
		{
			//ACA HAY QUE SETEAR LOS WATERMARK, EL TAMAÑO DE LA FIFO, Y VER COMO SE MANEJAN LAS INTERRUPCIONES
		}





	}
}

uint8_t uartIsRxMsg(uint8_t id)
{
return 0;
}

uint8_t uartGetRxMsgLength(uint8_t id)
{
return 0;
}

uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant)
{
return 0;
}

uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant)
{
return 0;
}

uint8_t uartIsTxMsgComplete(uint8_t id)
{
return 0;
}




void UART_setBaudRate(UART_Type * p2uart, uint32_t baudRate)
{
	uint8_t brfa;
	uint16_t sbr;
	uint32_t clock;

	clock = ((p2uart == UART0) || (p2uart == UART1))? (__CORE_CLOCK__) : (__CORE_CLOCK__ >> 1); //CORE_CLOCK o BUS_CLOCK

	if((baudRate <= MIN_BAUD_RATE) || (baudRate >= MAX_BAUD_RATE))
	{
		baudRate = BAUD_RATE_DEFAULT;
	}

	sbr = clock / (baudRate << 4); //clock /(baudRate * 16)
	brfa = ((clock << 1) / baudRate) - (sbr << 5); //(2*clock/baudRate) - 32*sbr

	p2uart->BDH = UART_BDH_SBR((sbr >> 8)); //primero se escribe parte alta
	p2uart->BDL = UART_BDL_SBR(sbr);  //luego se escribe parte baja
	p2uart->C4 = (p2uart->C4 & (~UART_C4_BRFA_MASK)) | UART_C4_BRFA(brfa);

}

void UART_clockGating(uint8_t id)
{
	SIM_Type * sim = SIM;
	switch(id)
	{
		case U0:
			sim->SCGC4 |= SIM_SCGC4_UART0_MASK;
			break;
		case U1:
			sim->SCGC4 |= SIM_SCGC4_UART1_MASK;
			break;
		case U2:
			sim->SCGC4 |= SIM_SCGC4_UART2_MASK;
			break;
		case U3:
			sim->SCGC4 |= SIM_SCGC4_UART3_MASK;
			break;
		case U4:
			sim->SCGC1 |= SIM_SCGC1_UART4_MASK;
			break;
//		case U5:
//			sim->SCGC1 |= SIM_SCGC1_UART5_MASK;
//			break;
	}
}


void UARTX_LON_IRQHandler(uint8_t id)
{

}
void UARTX_RX_TX_IRQHandler(uint8_t id)
{

}
void UARTX_ERR_IRQHandler(uint8_t id)
{

}

void UART0_LON_IRQHandler(void)
{
	UARTX_LON_IRQHandler(U0);
}
void UART0_RX_TX_IRQHandler(void)
{
	UARTX_RX_TX_IRQHandler(U0);
}
void UART0_ERR_IRQHandler(void)
{
	UARTX_ERR_IRQHandler(U0);
}
void UART1_RX_TX_IRQHandler(void)
{
	UARTX_RX_TX_IRQHandler(U1);
}
void UART1_ERR_IRQHandler(void)
{
	UARTX_ERR_IRQHandler(U1);
}
void UART2_RX_TX_IRQHandler(void)
{
	UARTX_RX_TX_IRQHandler(U2);
}
void UART2_ERR_IRQHandler(void)
{
	UARTX_ERR_IRQHandler(U2);
}
void UART3_RX_TX_IRQHandler(void)
{
	UARTX_RX_TX_IRQHandler(U3);
}
void UART3_ERR_IRQHandler(void)
{
	UARTX_ERR_IRQHandler(U3);
}
void UART4_RX_TX_IRQHandler(void)
{
	UARTX_RX_TX_IRQHandler(U4);
}
void UART4_ERR_IRQHandler(void)
{
	UARTX_ERR_IRQHandler(U4);
}
