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

int32 ptime_to_string( POSTTIME * ptime , char * str_out , int32 * int_count );
void dn_duration_to_string(DATE_NUMBERS * dn, char * str);

#endif /* TEXTUALIZATOR_H_ */
