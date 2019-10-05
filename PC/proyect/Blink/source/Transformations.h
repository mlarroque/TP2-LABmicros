/*
 * Transformations.h
 *
 *  Created on: Oct 5, 2019
 *      Author: G5
 */

#ifndef TRANSFORMATIONS_H_
#define TRANSFORMATIONS_H_
/****************************************************************
 * 						MACROS Y PARAMETROS
 *****************************************************************/

/*****************************************************************
 * 					ESTRUCTURAS Y TIPOS DE DATOS
 *****************************************************************/
typedef struct{
	int16_t radial;
	float32_t azimuthal_angle;
	float32_t polar_angle;
}Spherical_t;


/******************************************************************
 *							FUNCIONES
 ******************************************************************/
Spherical_t CartesianToSpherical(int16_t x, int16_t y, int16_t z);

#endif /* TRANSFORMATIONS_H_ */
