/*
 * coordinate_system.h
 */
#include "geotime.h"

#ifndef COORDINATE_SYSTEM_H_
#define COORDINATE_SYSTEM_H_

#define NUMBER_OF_TCS 3

typedef struct coordinate_system {
	JULIAN_DAY origin; //!< the origin relative to JULIAN_DAY origin.
	JULIAN_DAY scaling_factor; //!< ms per day - factor * x[time_unit] = x[julian_day]
} coordinate_system;

JULIAN_DAY tcs_to_jday(float8 * tcs_value_in, coordinate_system * tcs);
float8 jday_to_tcs(JULIAN_DAY * jd_in, coordinate_system * tcs);

typedef struct coordinate_system_key {
	int16 id;
	coordinate_system * coordinate_system;
} coordinate_system_ptr;

extern const coordinate_system_ptr tcs_systems[];

coordinate_system * get_tcs_system_from_key( int32 key );

#endif /* COORDINATE_SYSTEM_H_ */
