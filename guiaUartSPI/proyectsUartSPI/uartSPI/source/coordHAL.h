/*
 * coordHAL.h
 *
 *  Created on: Oct 5, 2019
 *      Author: Manuel Mollon
 */

#ifndef COORDHAL_H_
#define COORDHAL_H_

#include <stdbool.h>
#include <stdint.h>

/* Definition of the first message id.*/
#define OFFSET_ID 0x100

/**
 * @brief Initialize Hardware Abstraction Layer.
*/
void coordHALinit(void);

/**
 * @brief Sends Coordinates to other MC using CAN.
 * @param nameCoord Coordinate type to send. (Look in coord_t)
 * @param coord String with the coordinate itself.
 * @param coordNum Number of chars in coord string.
 * @return True if message was sent successfully. False if message couldn't be sent.
*/
bool sendCoordToAll(char nameCoord, char * coord, uint8_t coordNum);

/**
 * @brief Read a received coordinate from other MC. Non-Blocking.
 * @param id Pointer where to store id of received message.
 * @param nameCoord Pointer where to store kind of coord received.
 * @param coord String with the coordinate.
 * @param coordNum Number of chars in coord string.
 * @return True if message was read successfully. False if no message to read
*/
bool readCoord(uint32_t * id, char * nameCoord, char * coord, int * coordNum);

#endif /* COORDHAL_H_ */
