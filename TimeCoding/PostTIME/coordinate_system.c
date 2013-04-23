/*
 * coordinate_system.c
 */

#include "coordinate_system.h"

/*!Convert a floating point into JULIAN_DAY.
 * @param[in] tcs_value_in
 * @param[in] tcs Transform according to this system.
 * \return The calculated JULIAN_DAY-value. */
JULIAN_DAY tcs_to_jday(float8 * tcs_value_in, coordinate_system * tcs){
	JULIAN_DAY jd_ret = (JULIAN_DAY) (( *tcs_value_in * (float8) tcs->scaling_factor ) - (float8) tcs->origin );
	return jd_ret;
}

/*!Convert a JULIAN_DAY into a float.
 * @param[in] jd_in
 * @param[in] tcs Transform according to this system.
 * \return The calculated float. */
float8 jday_to_tcs(JULIAN_DAY * jd_in, coordinate_system * tcs){
	float8 float_ret = ( (float8) ( *jd_in + tcs->origin )) / (float8) tcs->scaling_factor;
	// float8 float_ret = tcs->scaling_factor;
	return float_ret;
}

/*! The REAL julian day -> different from internal JULIAN_DAY because of the MILLIS factorization.*/
coordinate_system julian_day_extern = {
	0,
	MILLIS
};

/*! UNIX TIME */
coordinate_system unix_time = {
	-210866760000000,
	1000
};

/*! This one presents the internal system to postgres. */
coordinate_system intern_to_extern = {
	0,
	1
};

/*! In this struct all available coordinate systems are listed. */
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
