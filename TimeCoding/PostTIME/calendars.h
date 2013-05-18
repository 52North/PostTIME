
#ifndef CALENDARS_H_
#define CALENDARS_H_

#include "geotime.h"

#define NUMBER_OF_CAL_SYS 1

/*!Struct to handle date values. It is quite similar to the tm struct,
 * but with some differences. Do not change the order or size of the fields!
 * Various functions accessing them via pointer-arithmetic. */
typedef struct {
        float8 sec;
        int32 yea;
        int32 mon;
        int32 day;
        int32 hou;
        int32 min;
        int32 utc_offset_h;
        int32 utc_offset_m;
        calendar_gran granularity;
} DATE_NUMBERS;

typedef struct clock_system {
	BYTE (*is_leap_fct) (DATE_NUMBERS *); //!< Determines if a given date contains leap seconds.
	const int32 hours_per_day;
	const int32 minutes_per_hour;
	const int32 seconds_per_minute;
	const int32 seconds_per_minute_leap;
} clock_system;

/*!This is  an experimental struct which should handle various calendar systems in a generic way.
 * It prevents not only some information about the system, but also points to transformation functions.*/
typedef struct calendar_era {
	BYTE (*is_leap_fct) (int32); //!< Pointer to a function that determines if a given year is a leap year (return 1) or not (return 0).
	const DATE_NUMBERS * lower_boundary; //!< Lower boundary of extent of use.
	const DATE_NUMBERS * upper_boundary; //!< Upper boundary of extent of use.
	const int32 month_per_year; //!< The month per year.
	const int32 month_per_year_leap; //!< Month per leap year.
	const int32 * days_per_month; //! Days per month in a regular year.
	const int32 * days_per_month_leap; //!< Days per month per leap year.
	JULIAN_DAY (*dnumber_to_jday) (DATE_NUMBERS *); //!< Function that transforms DATE_NUMBERS to a JULIAN_DAY according to this systems rules.
	DATE_NUMBERS (*jday_to_dnumbers) (JULIAN_DAY *); //!< ...and the vice versa transformation.
	const clock_system * clock_system;
} calendar_era;

typedef struct calendar_system {
	int16 id;
	calendar_era * calendar_era;
} calendar_system;

extern const calendar_system calendar_systems[];

BYTE check_value(int32 min, int32 max, int32 value);
int64 clock_time_to_dec_days(const DATE_NUMBERS * dn, const clock_system * clock_sys);
int32 correction_term_gregorian(DATE_NUMBERS * dn);
void dec_days_to_clock_time(DATE_NUMBERS * dn, int64 * time, const clock_system * clock_sys);
DATE_NUMBERS dn_plus_dn(DATE_NUMBERS * dn_in, DATE_NUMBERS * dn_add, calendar_era * cal);
DATE_NUMBERS dn_minus_period(DATE_NUMBERS * dn_in, DATE_NUMBERS * dn_minus, calendar_era * cal);
DATE_NUMBERS dn_minus_dn(DATE_NUMBERS * dn_later, DATE_NUMBERS * dn_earlier, calendar_era * cal);
BYTE dn_compare_dn(const DATE_NUMBERS * dn_one, const DATE_NUMBERS * dn_two);
pt_error_type check_validity(const DATE_NUMBERS * dn_in, const calendar_era * refsys);
calendar_era * get_system_from_key( int32 key );

#endif /* DATE_NUMBERS_TO_JUL_DAY_H_ */
