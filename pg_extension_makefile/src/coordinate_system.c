/*
 * coordinate_system.c
 */

#include "coordinate_system.h"

JULIAN_DAY tcs_to_jday(float8 * tcs_value_in, coordinate_system * tcs){
	JULIAN_DAY jd_ret = (JULIAN_DAY) (( *tcs_value_in * (float8) tcs->scaling_factor ) - (float8) tcs->origin );
	return jd_ret;
}

float8 jday_to_tcs(JULIAN_DAY * jd_in, coordinate_system * tcs){
	float8 float_ret = ( (float8) ( *jd_in + tcs->origin )) / (float8) tcs->scaling_factor;
	// float8 float_ret = tcs->scaling_factor;
	return float_ret;
}

coordinate_system julian_day_extern = {
	0,
	MILLIS
};

coordinate_system unix_time = {
	// -2440587.5,
	-210866760000000,
	1000
};

coordinate_system intern_to_extern = {
	0,
	1
};

const coordinate_system_ptr tcs_systems[NUMBER_OF_TCS] = {
		{ 1 , &julian_day_extern },
		{ 2 , &unix_time },
		{ 3 , &intern_to_extern }
};

coordinate_system * get_tcs_system_from_key( int32 key ){
	int32 i = 0;
	for( i = 0 ; i < NUMBER_OF_TCS ; i++ ){
		if( tcs_systems[i].id == key ) {
			return tcs_systems[i].coordinate_system;
		}
	}
	return NULL;
}
