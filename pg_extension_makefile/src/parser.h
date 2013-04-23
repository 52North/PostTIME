/*
 * parser.h
 * Purpose: Parse from string to numeric representation and vice versa.
 */

#include "calendars.h"
#include "coordinate_system.h"
#include "ordinal.h"
#include "blackbox_converter.h"
#ifndef PARSER_H_
#define PARSER_H_

int32 count_of_primitives(char * str_in);
pt_error_type string_to_ptime(char * str_in, int32 * int_count, POSTTIME * ptime_out);
pt_error_type determine_refsys(char * str_in, char *strs_out[], POSTTIME * ptime_tmp);

#endif /* PARSER_H_ */
