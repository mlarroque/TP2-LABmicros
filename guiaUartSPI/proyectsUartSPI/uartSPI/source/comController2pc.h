/*
 * comController2pc.h
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */


#ifndef COMCONTROLLER2PC_H_
#define COMCONTROLLER2PC_H_

void initResourcesController2pc(void);

//sendMessage2pc:
//args: id:
//		coord:
//		coordName:
//return: void
//brief: This function create a package a establishes a  new communication as master.
//		If the slave doesn't react with an "acknowledge" after a period of time (MAX_TIME_REACTION)
//		or it reacts by reporting an error, the package will be sent again
//		and the process can be repeated a limited number of times (MAX_ERRORS_ALLOWED)
void sendMessage2pc(int id, int coord, int coordName);

#endif /* COMCONTROLLER2PC_H_ */
