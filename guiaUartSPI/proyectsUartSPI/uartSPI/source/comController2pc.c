/*
 * comController2pc.c
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#include "boardIds.h"
#include "uart.h"  /////////CAPAZ QUE HAY QUE ABSTRAERLO MAS TODAVIA DE UARTTTTTTT
#include "timer.h"



unsigned int timeCounter;

void initUartResources(void);

void timeOutCallback(void);

void initResourcesController2node(void)
{
	initUartResources(); //the communication will be serial, using UART.
	InitializeTimers();

}

void sendMessage2node(char id, char * p2coord, char coordName)
{
	//shapeinfo

}

void initUartResources(void)
{
	uart_cfg_t configUART0;
	configUART0.baudRate = 9600;
	configUART0.parity = NO_PARITY;
	configUART0.mode = NON_BLOCKING_SIMPLE;
	configUART0.txWaterMark = 2;
	configUART0.rxWaterMark = 5;

	uartInit(U0, configUART0);
}
