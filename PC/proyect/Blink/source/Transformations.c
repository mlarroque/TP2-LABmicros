/*
 * Transformations.c
 *
 *  Created on: Oct 5, 2019
 *      Author: G5
 */
#include "Transformations.h"
#include "arm_math.h"

Spherical_t CartesianToSpherical(int16_t x, int16_t y, int16_t z)
{
	radial = 0;
	azimuthal = 0;
	polar = 0;
	//Calculo la coordenada radial
	float32_t sum_of_squares = x*x+ y*y +z*z;
	arm_sqrt_f32(sum_of_squares, &radial);
	//Calculo el angulo azimutal

}

