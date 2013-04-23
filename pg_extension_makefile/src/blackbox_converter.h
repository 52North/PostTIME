/*
 * blackbox_converter.h
 *
 *  Created on: 20.04.2013
 *      Author: ptr
 */

#include "geotime.h"
#include "calendars.h"
#include "coordinate_system.h"
#include "ordinal.h"

#ifndef BLACKBOX_CONVERTER_H_
#define BLACKBOX_CONVERTER_H_

BYTE parse_single_number(char * str_in, int32 * int_number);
pt_error_type instant_strings_to_ptime_instants(int32 *int_count, POSTTIME *ptime_tmp, char **str_primitives, char **str_primitives_end);

#endif /* BLACKBOX_CONVERTER_H_ */
