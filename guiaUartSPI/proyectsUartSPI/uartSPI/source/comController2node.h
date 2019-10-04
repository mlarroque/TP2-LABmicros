/*
 * comController2pc.h
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */


#ifndef COMCONTROLLER2NODE_H_
#define COMCONTROLLER2NODE_H_

//implementation options
#define NUMBER_OF_IMPLEMENTATION_OPTIONS 2
#define UART0_IMPLEMENTATION 0
#define UART1_IMPLEMENTATION 1
//#define CAN0_IMPLEMENTATION 2 //not enable on this file yet, we are working... sorry, we hope you will understand
//#define SPI0_IMPLEMENTATION 3 //not enable on this file yet, we are working... sorry, we hope you will understand
//

//specification of the protocol that will be implemented using this file
#define MAX_TIME_REACTION 100 //miliseconds
#define MAX_ERRORS_ALLOWED 2 //max number of tries to communicate

//general customizable configuration for the protocol that the user wants to be implemented
//(same as the configuration on the other side of communication bus)

#define PARITY_ODD 2
#define PARITY_EVEN 1
#define PARITY_NO 0

typedef struct
{
	int parity; //PARITY_ODD, PARITY_EVEN or PARITY_NO
	int baudRate; //In case of asynchronous implementations
	int nDataBits; //nDataBits per frame, if it is necessary
	int nStopBits; //nStopBits per frame, if it is necessary
}generalConfig_t;

//initResourcesController2node:
//args: implementationOption: it has to be one of the implementation options defined in this
//							file, and it must be equal to the implementation on the other side of communication bus.
//		dataMaxLen: maximum size of the data sent in a frame
//		p2config: p2config.
//brief: initialization of necessary resources for communication (initialize the implementation option
//		and establish a limit for dataLen).
void initResourcesController2node(int implementationOption, int dataMaxLen, generalConfig_t * p2config);

//sendMessage2node:
//args: id:
//		p2coord:
//		coordName:
//return: void
//brief: This function create a package a establishes a  new communication as master.
//		If the slave doesn't react with an "acknowledge" after a period of time (MAX_TIME_REACTION)
//		or it reacts by reporting an error, the package will be sent again
//		and the process can be repeated a limited number of times (MAX_ERRORS_ALLOWED)
void sendMessage2node(char header, char * p2data, char dataId, int implementation);

#endif /* COMCONTROLLER2NODE_H_ */
