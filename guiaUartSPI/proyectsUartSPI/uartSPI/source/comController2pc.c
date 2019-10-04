/*
 * comController2pc.c
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#include "boardIds.h"
#include "uart.h"
#include "timer.h"

#define TIME_OUT 2000UL //time out: 2 seconds



unsigned int timeCounter;

void initUartResources(void);

void timeOutCallback(void);

void initResourcesController2pc(void)
{
	initUartResources(); //the communication will be serial, using UART.
	InitializeTimers();

}

void sendMessage2pc(char id, char coord, char coordName)
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
