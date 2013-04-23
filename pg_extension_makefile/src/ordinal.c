/*
 * ordinal.c
 *
 *      Author: ptr
 */
#include "ordinal.h"

int32 ordinal_to_jday(char * str_in, ordinal_system * sys){
	int32 i = 0;
	for( i = 0 ; i < sys->count_of_eras ; i++ ){
		if( strcmp( str_in, sys->ordinal_era_keys[i] ) == 0 ){
			return i + 1;
		}
	}
	return 0;
}

char * jday_to_ordinal(int32 int_pos, ordinal_system * sys){
	return sys->ordinal_era_keys[int_pos - 1];
}

const char * const ics_system_period_eras[] = { "Ediacaran","Neoproterozoic","Proterozoic","Cambrian","Ordovician","Silurian","Devonian","Carboniferous","Paleozoic","Triassic","Jurassic","Mesozoic","Paleogene","Neogene","Quaternary"};

ordinal_system ics_system_period = {
	15,
	ics_system_period_eras
};

const ordinal_system_ptr ordinal_systems[NUMBER_OF_ORDINAL] = {
		{ 1 , &ics_system_period },
};

ordinal_system * get_ordinal_system_from_key( int32 key ){
	int32 i = 0;
	for( i = 0 ; i < NUMBER_OF_ORDINAL ; i++ ){
		if( ordinal_systems[i].id == key ) {
			return ordinal_systems[i].ordinal_system;
		}
	}
	return NULL;
}
