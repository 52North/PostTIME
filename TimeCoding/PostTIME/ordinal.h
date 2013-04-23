/*
 * ordinal.h
 *
 *  Created on: Apr 19, 2013
 *      Author: ptr
 */
#include "geotime.h"

#ifndef ORDINAL_H_
#define ORDINAL_H_

#define NUMBER_OF_ORDINAL 1

typedef struct ordinal_system {
	int32 count_of_eras;
	const char * const * ordinal_era_keys;
} ordinal_system;

int32 ordinal_to_jday(char * str_in, ordinal_system * sys);
char * jday_to_ordinal(int32 int_pos, ordinal_system * sys);

typedef struct ordinal_system_key {
	int32 id;
	ordinal_system * ordinal_system;
} ordinal_system_ptr;

const ordinal_system_ptr ordinal_systems[NUMBER_OF_ORDINAL];

ordinal_system * get_ordinal_system_from_key( int32 key );

#endif /* ORDINAL_H_ */
