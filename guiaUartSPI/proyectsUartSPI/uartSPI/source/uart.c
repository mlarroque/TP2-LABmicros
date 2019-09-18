/*
 * uart.c
 *
 *  Created on: 18 sep. 2019
 *      Author: G5
 */

#include "uart.h"

#define MAX_BAUD_RATE 0x1FFF
#define MIN_BAUD_RATE 0x0000
#define BAUD_RATE_DEFAULT 9600
#define MAX_UARTS_AVAILABLES 5

void UART_setBaudRate(UART_Type * p2uart, uint32_t baudRate);

void uartInit (uint8_t id, uart_cfg_t config)
{
	UART_Type * p2uartsArray[] = UART_BASE_PTRS;
	UART_Type * p2uart;
	if ((id >= 0) && (id < MAX_UARTS_AVAILABLES)) //solo se hace algo si el id es válido
	{
		p2uart = p2uartsArray[id];
		UART_setBaudRate(p2uart, config.baudRate);
		//para que se active el baudRate aun hay que setear C2[RE] o C2[TE] (receiver o transmitter enabled)
	}
}

uint8_t uartIsRxMsg(uint8_t id)
{

}

uint8_t uartGetRxMsgLength(uint8_t id)
{

}

uint8_t uartReadMsg(uint8_t id, char* msg, uint8_t cant)
{

}

uint8_t uartWriteMsg(uint8_t id, const char* msg, uint8_t cant)
{

}

uint8_t uartIsTxMsgComplete(uint8_t id)
{

}




void UART_setBaudRate(UART_Type * p2uart, uint32_t baudRate)
{
	uint8_t brfa;
	uint16_t sbr;
	uint32_t clock;

	clock = ((p2uart == UART0) || (p2uart == UART1))? (_CORE_CLOCK_) : (_CORE_CLOCK_ >> 1); //CORE_CLOCK o BUS_CLOCK

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
