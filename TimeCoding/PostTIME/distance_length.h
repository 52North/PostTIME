/*
 * duration_length.h
 *
 *  Created on: 06.05.2013
 *      Author: ptr
 */

#include "relative_position.h"
#include "calendars.h"
#include "textualizator.h"

#ifndef DURATION_LENGTH_H_
#define DURATION_LENGTH_H_

#define P0_8601_STRING "P0"

void distance_string(POSTTIME * ptime1, POSTTIME * ptime2, char * distance_out);
void distance_jul_day(POSTTIME * ptime1, POSTTIME * ptime2, float8 * jul_day_out);
void duration_string(POSTTIME * ptime, char * duration_out);
void duration_jul_day( POSTTIME * ptime , float8 * jul_day_out);


#endif /* DURATION_LENGTH_H_ */
