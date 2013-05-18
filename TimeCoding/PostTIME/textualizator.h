/*
 * textualizator.h
 *
 *  Created on: 06.04.2013
 *      Author: ptr
 */

#include "calendars.h"
#include "coordinate_system.h"
#include "ordinal.h"
#include "blackbox_converter.h"

#ifndef TEXTUALIZATOR_H_
#define TEXTUALIZATOR_H_

void dn_duration_to_string(DATE_NUMBERS * dn, char * str);
void date_numbers_to_instant_string(DATE_NUMBERS * dn, char * str_out);
void julianday_cat_instantstring(float8 * value, char * str);
int32 ptime_to_string( POSTTIME * ptime , char * str_out , int32 * int_count );
void ptime_to_string_calendar( POSTTIME * ptime , char * str_out , int32 * int_count );
void ptime_to_string_ordinal( POSTTIME * ptime , char * str_out , int32 * int_count );
void ptime_to_string_tcs( POSTTIME * ptime , char * str_out , int32 * int_count );
void refsys_tag( POSTTIME * ptime , char * str_out );

#endif /* TEXTUALIZATOR_H_ */
