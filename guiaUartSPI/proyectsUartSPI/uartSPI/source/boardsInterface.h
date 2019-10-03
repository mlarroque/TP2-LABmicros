/*
 * boardsInterface.h
 *
 *  Created on: 3 oct. 2019
 *      Author: G5
 */

#ifndef BOARDSINTERFACE_H_
#define BOARDSINTERFACE_H_

//brief: initialize accelerometer resources of our board, and communication with other boards.
void initBoardsInterface(void);

//Return 0 if there is no lecture. Return !0 if there is something updated.
//if there is something updated, the update is written in the "ints" passed as inputs.
int updateLetcure(int * id, int * coord, int * coordName);

#endif /* BOARDSINTERFACE_H_ */
