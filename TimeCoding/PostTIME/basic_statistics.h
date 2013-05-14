/*
 * basic_statistics.h
 *      Author: ptr
 */


#include "blackbox_converter.h"
#include "funcapi.h"
#include "calendars.h"
#include "parser.h"

#ifndef BASIC_STATISTICS_H_
#define BASIC_STATISTICS_H_

typedef struct normal_dn_histogram_fctx {
	calendar_era * cal_system;
	DATE_NUMBERS next_dn;
	POSTTIME next_ptime;
	DATE_NUMBERS period;
	JULIAN_DAY end_instant;
} normal_dn_histogram_fctx;

#endif /* BASIC_STATISTICS_H_ */
