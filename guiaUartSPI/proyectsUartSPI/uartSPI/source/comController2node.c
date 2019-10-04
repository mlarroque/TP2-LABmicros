/*
 * comController2pc.c
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */
#include "comController2node.h"
#include "timer.h"
#include "uart.h"
#include <stdbool.h>

typedef void (*p2initFuncion_t)(generalConfig_t * p2config);
typedef _Bool (*p2comFunction_t)(char id, char * p2data, char dataName);

void initUART0(generalConfig_t * p2config);
void initUART1(generalConfig_t * p2config);

_Bool sendMsg2nodeUARTX(char id, char * p2data, char dataName, int idUART);
_Bool sendMsg2nodeUART0(char id, char * p2data, char dataName);
_Bool sendMsg2nodeUART1(char id, char * p2data, char dataName);

_Bool receiveMsgFromNodeUARTX(char id, char * p2data, char dataName, int idUART);
_Bool receiveMsgFromNodeUART0(char id, char * p2data, char dataName);
_Bool receiveMsgFromNodeUART1(char id, char * p2data, char dataName);

p2initFuncion_t initFunctionsArray[NUMBER_OF_IMPLEMENTATION_OPTIONS] = {initUART0, initUART1};
p2comFunction_t sendFunctionsArray[NUMBER_OF_IMPLEMENTATION_OPTIONS] = {sendMsg2nodeUART0, sendMsg2nodeUART1};
p2comFunction_t receiveFunctionsArray[NUMBER_OF_IMPLEMENTATION_OPTIONS] = {receiveMsgFromNodeUART0, receiveMsgFromNodeUART1};

unsigned int timeCounter;

void initUartResources(int uartId, generalConfig_t * p2config);

void timeOutCallback(void);

void initResourcesController2node(int implementationOption, int dataMaxLen)
{
	if((implementationOption >= 0) && (implementationOption < NUMBER_OF_IMPLEMENTATION_OPTIONS))
	{
		initFunctionsArray[implementationOption];
	}

}

void sendMessage2node(char id, char * p2data, char dataName, int implementation)	//In this function is implemented the communication protocol
{
	if((implementationOption >= 0) && (implementationOption < NUMBER_OF_IMPLEMENTATION_OPTIONS))
	{
		sendFunctionsArray[implementation];
	}
	void SetTimer(unsigned char index,unsigned long int timeout, callback_ptr func);
	void RestartTimer(unsigned char index);
}

void initUartResources(int uartId, uart_cfg_t * p2config)
{
	uart_cfg_t configUART;
	if (IS_VALID_BAUD_RATE(p2config->baudRate))
	{
		configUART.baudRate = p2config->baudRate;
	}
	else
	{
		configUART.baudRate = DEFAULT_BAUD_RATE_UART;
	}

	if(p2config->parity == PARITY_ODD)
	{
		configUART.parity = PARITY_ODD_UART;
	}
	else if(p2config->parity == PARITY_EVEN)
	{
		configUART.parity = PARITY_EVEN_UART;
	}
	else
	{
		configUART.parity = NO_PARITY_UART;
	}

	if(p2config->nDataBits == 9)
	{
		configUART.nBits = NBITS_9;
	}
	else
	{
		configUART.nBits = NBITS_8;
	}

	configUART.mode = NON_BLOCKING_SIMPLE;
	configUART.txWaterMark = 2;
	configUART.rxWaterMark = 5;

	uartInit(uartId, *p2config);
}
