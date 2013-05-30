
#include "calendars.h"

/*!Test if a value is within the given bounds.
 * @param[in] min Lower boundary.
 * @param[in] max Upper boundary.
 * @param[in] value Test with this value.
 * \return 1 if the value is greater or equal than min and smaller or equal than max, otherwise 0. */
BYTE check_value(int32 min, int32 max, int32 value){
	BYTE byte_ret = 0;
	if ((value <= max)&&(value >= min)){
		byte_ret = 1;
	}
	return byte_ret;
}

/************************************************************************************
		D A T E		N U M B E R S
 ************************************************************************************/

/*!Compare two DATE_NUMBERS.
 * @param[in] dn_one First DATE_NUMBERS.
 * @param[in] dn_two Second DATE_NUMBERS.
 * \return -1= First is before Second, 0=Equal values, 1=First is after Second. */
BYTE dn_compare_dn(const DATE_NUMBERS * dn_one, const DATE_NUMBERS * dn_two) {
	BYTE byte_ret = 0 ;
	int32 * int_ptr_one = &dn_one->yea ;
	int32 * int_ptr_two = &dn_two->yea ;
	int32 one = 0, two = 0, i = 0;
	for( i = 0; i < 5; i++ ) {
		one = *(int_ptr_one + i);
		two = *(int_ptr_two + i);
		if( one < two ){
			byte_ret = -1;
			break;
		}
		else if( one > two ){
			byte_ret = 1;
			break;
		}
	}

	if( byte_ret == 0) {
		if ( dn_one->sec < dn_two->sec ){
			byte_ret = -1;
		}
		else if( dn_one->sec > dn_two->sec ){
			byte_ret = 1;
		}
	}
	return byte_ret;
}

/*!This function adds a period to a calendar date - the result is determined according to the rules of
 * the given calendar_era.
 * @param[in] dn_in Given calendar date.
 * @param[in] dn_add The period - also as DATE_NUMBERS.
 * @param[in] cal Process according to this system.
 * \return The result as DATE_NUMBERS. */
DATE_NUMBERS dn_plus_dn(DATE_NUMBERS * dn_in, DATE_NUMBERS * dn_add, calendar_era * cal){
	DATE_NUMBERS ret_dn, dn_add_intern;
	BYTE is_leap;
	memset(&ret_dn, '\0', sizeof(DATE_NUMBERS));
	memcpy(&dn_add_intern, dn_add, sizeof(DATE_NUMBERS));
	ret_dn.sec = dn_in->sec + dn_add_intern.sec;
	while(ret_dn.sec > 59){
		ret_dn.sec = ret_dn.sec - 60;
		dn_add_intern.min++;
	}
	ret_dn.min = dn_in->min + dn_add_intern.min;
	while(ret_dn.min > 59){
		ret_dn.min = ret_dn.min - 60;
		dn_add_intern.hou++;
	}
	ret_dn.hou = dn_in->hou + dn_add_intern.hou;
	while(ret_dn.hou > 23){
		ret_dn.hou = ret_dn.hou - 24;
		dn_add_intern.day++;
	}
	int32 yea_tmp, mon_tmp, max_day, max_month;
	ret_dn.day = dn_in->day + dn_add_intern.day;
	ret_dn.mon = dn_in->mon + dn_add_intern.mon;
	ret_dn.yea = dn_in->yea + dn_add_intern.yea;

	do {
		yea_tmp = ret_dn.yea;
		mon_tmp = ret_dn.mon;
		is_leap = cal->is_leap_fct(ret_dn.yea);
		if(is_leap){
			max_month = cal->month_per_year_leap;
		}
		else {
			max_month = cal->month_per_year;
		}
		while(ret_dn.mon > max_month){
			ret_dn.mon = ret_dn.mon - max_month;
			ret_dn.yea++;
		}
		if(ret_dn.mon == mon_tmp){
			if(is_leap){
				max_day = cal->days_per_month_leap[mon_tmp - 1];
			}
			else {
				max_day = cal->days_per_month[mon_tmp - 1];
			}
			if( ret_dn.day > max_day ){
				ret_dn.day = ret_dn.day - max_day;
				ret_dn.mon++;
			}
		}
	} while( ret_dn.mon != mon_tmp || ret_dn.yea != yea_tmp );
	return ret_dn;
}

/*!This function subtracts a period from a calendar date - the result is determined according to the rules of
 * the given calendar_era.
 * @param[in] dn_in Given calendar date.
 * @param[in] period_minus The period - also as DATE_NUMBERS.
 * @param[in] cal Process according to this system.
 * \return The result as DATE_NUMBERS. */
DATE_NUMBERS dn_minus_period(DATE_NUMBERS * dn_in, DATE_NUMBERS * period_minus, calendar_era * cal){
	DATE_NUMBERS ret_dn, dn_minus_intern;
	BYTE is_leap;
	memset(&ret_dn, '\0', sizeof(DATE_NUMBERS));
	memcpy(&dn_minus_intern, period_minus, sizeof(DATE_NUMBERS));
	ret_dn.sec = dn_in->sec - dn_minus_intern.sec;
	while(ret_dn.sec < 0){
		ret_dn.sec = ret_dn.sec + 60;
		dn_minus_intern.min++;
	}
	ret_dn.min = dn_in->min - dn_minus_intern.min;
	while(ret_dn.min < 0){
		ret_dn.min = ret_dn.min + 60;
		dn_minus_intern.hou++;
	}
	ret_dn.hou = dn_in->hou - dn_minus_intern.hou;
	while(ret_dn.hou < 0){
		ret_dn.hou = ret_dn.hou + 24;
		dn_minus_intern.day++;
	}
	int32 yea_tmp, mon_tmp, max_day, max_month;
	ret_dn.day = dn_in->day - dn_minus_intern.day;
	ret_dn.mon = dn_in->mon - dn_minus_intern.mon;
	ret_dn.yea = dn_in->yea - dn_minus_intern.yea;

	do {
		yea_tmp = ret_dn.yea;
		mon_tmp = ret_dn.mon;
		is_leap = cal->is_leap_fct(ret_dn.yea);
		while(ret_dn.mon < 1){
			ret_dn.yea--;
			is_leap = cal->is_leap_fct(ret_dn.yea); // TODO
			if(is_leap){
				max_month = cal->month_per_year_leap;
			}
			else {
				max_month = cal->month_per_year;
			}
			ret_dn.mon = ret_dn.mon + max_month;
		}
		if(ret_dn.day < 1){
			if(ret_dn.mon == mon_tmp) ret_dn.mon--;
			if(is_leap){
				max_day = cal->days_per_month_leap[ret_dn.mon - 1];
			}
			else {
				max_day = cal->days_per_month[ret_dn.mon - 1];
			}
			ret_dn.day = ret_dn.day + max_day;
		}
	} while( ret_dn.mon != mon_tmp || ret_dn.yea != yea_tmp );
	return ret_dn;
}

/*!This function subtracts a calendar date from anther calendar date - the result is determined according to the rules of
 * the given calendar_era.
 * @param[in] dn_later Has to be later than dn_earlier.
 * @param[in] dn_earlier Has to be earlier than dn_later.
 * @param[in] cal Process according to this system.
 * \return The result as DATE_NUMBERS. */
DATE_NUMBERS dn_minus_dn(DATE_NUMBERS * dn_later, DATE_NUMBERS * dn_earlier, calendar_era * cal){
	DATE_NUMBERS ret_dn, dn_minus_intern;
	BYTE is_leap;
	memset(&ret_dn, '\0', sizeof(DATE_NUMBERS));
	memcpy(&dn_minus_intern, dn_earlier, sizeof(DATE_NUMBERS));
	ret_dn.sec = dn_later->sec - dn_minus_intern.sec;
	while(ret_dn.sec < 0){
		ret_dn.sec = ret_dn.sec + 60;
		dn_minus_intern.min++;
	}
	ret_dn.min = dn_later->min - dn_minus_intern.min;
	while(ret_dn.min < 0){
		ret_dn.min = ret_dn.min + 60;
		dn_minus_intern.hou++;
	}
	ret_dn.hou = dn_later->hou - dn_minus_intern.hou;
	while(ret_dn.hou < 0){
		ret_dn.hou = ret_dn.hou + 24;
		dn_minus_intern.day++;
	}
	int32 max_day, max_month;
	ret_dn.day = dn_later->day - dn_minus_intern.day;
	ret_dn.mon = dn_later->mon - dn_minus_intern.mon;
	ret_dn.yea = dn_later->yea - dn_minus_intern.yea;

	is_leap = cal->is_leap_fct(dn_minus_intern.yea);
	if( ret_dn.day < 0 ){
		if(is_leap){
			max_day = cal->days_per_month_leap[dn_minus_intern.mon - 1];
		}
		else {
			max_day = cal->days_per_month[dn_minus_intern.mon - 1];
		}
		ret_dn.day = ret_dn.day + max_day;
		ret_dn.mon--;
	}
	if( ret_dn.mon < 0 ){
		if(is_leap){
			max_month = cal->month_per_year_leap;
		}
		else {
			max_month = cal->month_per_year;
		}
		ret_dn.mon = ret_dn.mon + max_month;
		ret_dn.yea--;
	}
	return ret_dn;
}

/************************************************************************************
		U T		C L O C K 		S Y S T E M
 ************************************************************************************/

/*!Dummmy function.
 * \return Returns 0. */
static BYTE is_leap_fct_ut(DATE_NUMBERS * dn_in){
	return 0;
}

const clock_system clock_ut = {
	&is_leap_fct_ut,
	24,
	60,
	60,
	60
};

/************************************************************************************
		S O M E 	G E N E R A L		F U N C T I O N S
 ************************************************************************************/

/*!Ensure that the values in the DATE_NUMBERS struct represent an valid date in the given
 * calendar_era.
 * @param[in] dn_in The DATE_NUMBERS.
 * @param[in] refsys Ensure semantical correctness relative to this system.
 * \return pt_error_type. */
pt_error_type check_validity(const DATE_NUMBERS * dn_in, const calendar_era * refsys){
	pt_error_type err_ret = CALENDAR_VALUES_NOT_VALID;
	BYTE is_leap = refsys->is_leap_fct(dn_in->yea);
	BYTE is_leap_sec = refsys->clock_system->is_leap_fct(dn_in);
	// Check years and general extent.
	if( dn_compare_dn(dn_in, refsys->lower_boundary) == -1 ) return err_ret;
	if( dn_compare_dn(dn_in, refsys->upper_boundary) == 1 ) return err_ret;

	// Check month and day.
	if( is_leap ) {
		if( !check_value( 1, refsys->month_per_year_leap, dn_in->mon ) ) return err_ret;
		if( !check_value( 1, *(&refsys->days_per_month_leap[0]+dn_in->mon-1), dn_in->day) ) return err_ret;
	}
	else {
		if( !check_value( 1, refsys->month_per_year, dn_in->mon ) ) return err_ret;
		if( !check_value( 1, *( (&refsys->days_per_month[0]) +dn_in->mon-1), dn_in->day) ) return err_ret;
	}
	// Check clock.

	if( !check_value( 0, refsys->clock_system->hours_per_day - 1, dn_in->hou ) ) return err_ret;
	if( !check_value( 0, refsys->clock_system->minutes_per_hour - 1, dn_in->min ) ) return err_ret;


	if( is_leap_sec ){
		if ((dn_in->sec  > ( refsys->clock_system->seconds_per_minute_leap - 1 ) )&&(dn_in->sec  < 0)) return err_ret;
	}
	else {
		if ((dn_in->sec  > ( refsys->clock_system->seconds_per_minute - 1) )&&(dn_in->sec  < 0)) return err_ret;
	}
	err_ret = NO_ERROR;
	return err_ret;

}


/*!Convert a clock time in decimal days.
 * @param[in] dn This DATE_NUMBERS contain the clock time.
 * @param[in] clock_sys Use this rules.
 * \return The decimal day value. */
int64 clock_time_to_dec_days(const DATE_NUMBERS * dn, const clock_system * clock_sys){
	const int64 ms_per_hour = 3600000;
	const int64 ms_per_min = 60000;
	const int64 ms_per_sec = 1000;
	float8 sec = (dn->sec * ( (float8) ms_per_sec) );
	int64 int_sec = (int64) sec;
	int64 time = 0;
	time = dn->hou * ms_per_hour + dn->min * ms_per_min + int_sec;
	return time;
}

/*!Convert decimal days in a clock time.
 * @param[in] time Convert this time.
 * @param[in] clock_sys Use this rules.
 * @param[out] dn This DATE_NUMBERS contain the clock time.
 * \return The values in dn. */
void dec_days_to_clock_time(DATE_NUMBERS * dn, int64 * time, const clock_system * clock_sys){
	int32 tmp = 0;
	const int64 ms_per_hour = 3600000;
	const int64 ms_per_min = 60000;
	const int64 ms_per_sec = 1000;
	dn->hou = *time / ms_per_hour;
	tmp = *time % ms_per_hour;
	dn->min = tmp / ms_per_min;
	tmp = *time % ms_per_min;
	dn->sec = ( (float8) tmp) / ((float8) ms_per_sec );
}

/************************************************************************************
		G R E G O R I A N		C A L E N D A R
 ************************************************************************************/

const calendar_era gregorian;

/*!Leap year test for Gregorian calendar.
 * @param[in] Given year.
 * \return 1=Yes 0=No. */
static BYTE leapyear_gre(int32 year){
	BYTE byte_ret = 0;
	if ((year % 4) == 0) byte_ret = 1;
	if ((year % 100) == 0) byte_ret = 0;
	if ((year % 400) == 0) byte_ret = 1;

	return byte_ret;
}

/*!Subroutine for Gregorian calendar date import.
 * @param[in] dn Calculate correction term for this DATE_NUMBERS.
 * \return The correction term. */
int32 correction_term_gregorian(DATE_NUMBERS * dn) {
	int32 month[12] = {-1, 0, -2, -1, -1, 0, 0, 1, 2, 2, 3, 3};
	int32 corr = 0;
	if((leapyear_gre(dn->yea)) && (dn->mon > 2)) corr = 1; /* for leap year */
	return corr = corr + month[dn->mon-1]; /* for month */
}

const int32 days_per_month_gregorian[12] = { 31 ,28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const int32 days_per_month_gregorian_leap[12] = { 31 ,29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
const DATE_NUMBERS dn_lower_boundary_gregorian = { 0, -4713, 1, 1, 0, 0, 0, 0, SECOND };
const DATE_NUMBERS dn_upper_boundary_gregorian = { 0, 100000, 1, 1, 0, 0, 0, 0, SECOND };

/*!Convert the given DATE_NUMBERS to JULIAN_DAY basing on the gregorian calendar.
 * @param[in] dn Input values.
 * \return The transformed JULIAN_DAY. */
static JULIAN_DAY dnumber_to_jday_greg( DATE_NUMBERS * dn ){
	JULIAN_DAY jd = 0;
	int32 y, d, n400, n100, n4, n1;
	d = dn->day + 30*(dn->mon-1) + correction_term_gregorian(dn);
	y = dn->yea-1;
	int32 jd0 = 1721426;
	n400 = y/400;
	n100 = (y% 400)/100;
	n4 = ((y% 400) % 100)/4;
	n1 = ((y% 400) % 100)%4;
	int32 jdday = jd0 + n400*146097 + n100*36524 + n4*1461 + n1*365 +d;
	int64 dec_day = clock_time_to_dec_days(dn , gregorian.clock_system);
	jd =  ((int64) jdday * (int64) MILLIS) - 43200000 + dec_day;
	return jd;
}

/*!Convert the given JULIAN_DAY in DATE_NUMBERS basing on the gregorian calendar.
 * @param[in] jd The JULIAN_DAY value.
 * \return The transformed DATE_NUMBERS. */
static DATE_NUMBERS jday_to_dnumbers_greg( JULIAN_DAY * jd ){
	DATE_NUMBERS dn;
	int32 J = (int32) ( (*jd + 43200000) / MILLIS);
	// int32 J = (int32)(*jd+0.5);  /* jd without daytime */
	int64 F = (*jd + 43200000) % MILLIS;		/* daytime */
	int32 jd0 = 1721426;
	int32 n400, n100, r100, n4, n1, lt;
	n400 = (J - jd0)/146097;
	n100 = ((J - jd0)%146097)/36524;
	r100 = ((J - jd0)%146097)%36524;
	if(n100 == 4 && r100 == 0){
		n100 = 3;
		r100 = 36524;
	}
	n4 = r100/1461;
	n1 = (r100%1461)/365;
	lt = (r100%1461)%365;
	if(n1 == 4 && lt == 0){
		n1 = 3;
		lt = 365;
	}
	float8 lj = 400*n400 + 100*n100 + 4*n4 + n1;
	dn.yea = lj+1;
	dn.mon = (lt+1)/30 + 1;
	dn.day = lt -30*(dn.mon-1) - correction_term_gregorian(&dn);
	if(dn.mon > 12 || dn.day < 1){
		dn.mon -= 1;
		dn.day = lt -30*(dn.mon-1) - correction_term_gregorian(&dn);
	}
	dec_days_to_clock_time(&dn, &F, gregorian.clock_system);
	return dn;
}

/*!The Gregorian calendar. */
const calendar_era gregorian = {
	&leapyear_gre,
	&dn_lower_boundary_gregorian,
	&dn_upper_boundary_gregorian,
	12,
	12,
	days_per_month_gregorian,
	days_per_month_gregorian_leap,
	&dnumber_to_jday_greg,
	&jday_to_dnumbers_greg,
	&clock_ut
};

/************************************************************************************
		J U L I A N  		C A L E N D A R
 ************************************************************************************/

const calendar_era julian;

/*!Leap year test for Julian calendar.
 * @param[in] Given year.
 * \return 1=Yes 0=No. */
static BYTE leapyear_jul(int32 year){
	BYTE byte_ret = 0;
	if ((year % 4) == 0) byte_ret = 1;
	return byte_ret;
}

/*!Convert the given DATE_NUMBERS to JULIAN_DAY basing on the Julian calendar.
 * @param[in] dn Input values.
 * \return The transformed JULIAN_DAY. */
static JULIAN_DAY dnumber_to_jday_jul( DATE_NUMBERS * dn ){
	JULIAN_DAY jd = 0;
	int64 Y = dn->yea + 4716 -(14-dn->mon)/12;
	int64 M = (dn->mon+9)%12;
	int64 D = dn->day-1;
	int64 c = (1461*Y)/4;
	int64 d = (153*M + 2)/5;
	/* ISO19108 P.49 */
	jd = ( ( c + d + D - 1401 ) * (int64) MILLIS ) - 43200000;
	int64 dec_day = clock_time_to_dec_days(dn , julian.clock_system);
	jd = jd + dec_day;
	return jd;
}

/*!Convert the given JULIAN_DAY in DATE_NUMBERS basing on the Julian calendar.
 * @param[in] jd The JULIAN_DAY value.
 * \return The transformed DATE_NUMBERS. */
static DATE_NUMBERS jday_to_dnumbers_jul( JULIAN_DAY * jd ){
	DATE_NUMBERS dn;
	int32 J = (int32) ( (*jd + 43200000) / MILLIS);
	// int32 J = (int32)(*jd+0.5);  /* jd without daytime */
	int64 F = (*jd + 43200000) % MILLIS;	/* daytime */
	J = J+1401;
	int32 Y = (4*J+3) / 1461;
	int32 T = ( (4*J+3) % 1461) / 4;
	int32 M = (5*T+2) / 153;
	int32 D = ( (5*T+2) % 153) / 5;
	dn.day = D+1;
	dn.mon = ((M+2)%12) + 1;
	dn.yea = Y-4716 + ((14-dn.mon)/12);
	dec_days_to_clock_time(&dn, &F, julian.clock_system);
	return dn;
}

const DATE_NUMBERS dn_lower_boundary_jul = { 0, -4713, 1, 1, 0, 0, 0, 0, SECOND };

/*!The Julian calendar. */
const calendar_era julian = {
	&leapyear_jul,
	&dn_lower_boundary_jul,
	&dn_upper_boundary_gregorian,
	12,
	12,
	days_per_month_gregorian,
	days_per_month_gregorian_leap,
	&dnumber_to_jday_jul,
	&jday_to_dnumbers_jul,
	&clock_ut
};

/************************************************************************************
		T H E	C A L E N D A R		S Y S T E M S
 ************************************************************************************/

const calendar_system calendar_systems[2] = {
		{ 1 , &gregorian },
		{ 2 , &julian },
};

calendar_era * get_system_from_key( int32 key ){
	int32 i = 0;
	for( i = 0 ; i < NUMBER_OF_CAL_SYS ; i++ ){
		if( calendar_systems[i].id == key ) {
			return calendar_systems[i].calendar_era;
		}
	}
	return NULL;
}
