/*
 * blackbox_converter.h
 *
 *      Author: ptr
 */

#include "calendars.h"
#include "coordinate_system.h"
#include "ordinal.h"

#ifndef BLACKBOX_CONVERTER_H_
#define BLACKBOX_CONVERTER_H_

BYTE parse_single_number(char * str_in, int32 * int_number);
pt_error_type instant_strings_to_ptime_instants(int32 *int_count, POSTTIME *ptime_tmp, char **str_primitives, char **str_primitives_end);
pt_error_type regular_strings_to_ptime_instance(POSTTIME * ptime_tmp, char ** str_primitives, char * * str_reg_parts);
void jday_pack_to_dn_period(DATE_NUMBERS * period_vals , JULIAN_DAY * jd_ptr , int32 * rvalue);

#endif /* BLACKBOX_CONVERTER_H_ */
