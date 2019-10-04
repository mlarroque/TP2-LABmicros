/*
 * boardsInterface.h
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#ifndef BOARDSINTERFACE_H_
#define BOARDSINTERFACE_H_

#define N_COORDS 3

//brief: initialize accelerometer resources of our board, and communication with other boards.
void initBoardsInterface(void);

//Return 0 if there is no lecture. Return !0 if there is something updated.
//if there is something updated, the update is written in the "chars" passed as inputs.
//It is important to know that p2coord is consider by the function as a pointer (distinct to a NULL pointer) that allows to access to
//N_COORDS + 1 chars data types since the memory direction pointed.
int updateLetcure(char * p2id, char * p2coord, char * p2coordName);

//
void sendMessage2otherBoards(char id, char * p2coord, char coordName);

#endif /* BOARDSINTERFACE_H_ */
