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

pt_error_type distance_string(POSTTIME * ptime1, POSTTIME * ptime2, char * distance_out);
pt_error_type distance_jul_day(POSTTIME * ptime1, POSTTIME * ptime2, float8 * jul_day_out);

#endif /* DURATION_LENGTH_H_ */