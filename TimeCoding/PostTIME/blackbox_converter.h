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
BYTE parse_single_float(char * str_in, float8 * float_number);
pt_error_type instant_strings_to_ptime_instants(int32 *int_count, POSTTIME *ptime_tmp, char **str_primitives, char **str_primitives_end);
pt_error_type regular_strings_to_ptime_instance(POSTTIME * ptime_tmp, char ** str_primitives, char * * str_reg_parts);
pt_error_type parse_single_instant_string(char * str_in, DATE_NUMBERS * dn_ret);
void adjust_dn_to_granularity( DATE_NUMBERS * dn_in_out , calendar_era * cal );
void dn_period_to_jday_pack(DATE_NUMBERS * period_vals , JULIAN_DAY * jd_ptr , int32 rvalue);
void jday_pack_to_dn_period(DATE_NUMBERS * period_vals , JULIAN_DAY * jd_ptr , int32 * rvalue);
pt_error_type parse_single_duration_string(char * str_in, DATE_NUMBERS * dn);

#endif /* BLACKBOX_CONVERTER_H_ */
