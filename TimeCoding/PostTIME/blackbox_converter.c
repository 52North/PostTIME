/*
 * blackbox_converter.c
 *
 *      Author: ptr
 */

#include "blackbox_converter.h"

/*!Parse in a single integer number and ensure syntactical validity, which means that the
 * string contains only numbers at this point.
 * @param[in] str_in The string.
 * @param[out] int_number Resulting integer.
 * \return 0 in case of error, otherwise 1 */
BYTE parse_single_number(char * str_in, int32 * int_number){
	int32 int_itr = 0;
	BYTE byte_ret = 1;
	while(str_in[int_itr] != '\0' && byte_ret == 1){
		switch(str_in[int_itr]){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			int_itr++;
			break;
		default:
			byte_ret = 0;
			break;
		}
	}
	if( byte_ret ){
		sscanf(str_in, "%d" , int_number);
	}
	return byte_ret;
}

/*!Parse in a single floating point number and ensure syntactical validity, which means that the
 * string contains only numbers at this point.
 * @param[in] str_in The string.
 * @param[out] float_number Resulting float.
 * \return 0 in case of error, otherwise 1 */
BYTE parse_single_float(char * str_in, float8 * float_number){
	int32 int_itr = 0;
	BYTE byte_ret = 1;
	BYTE point_flag = 0;
	while(str_in[int_itr] != '\0' && byte_ret == 1){
		switch(str_in[int_itr]){
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			int_itr++;
			break;
		case '.':
			if( point_flag ) byte_ret = 0;
			point_flag = 1;
			int_itr++;
			break;
		default:
			byte_ret = 0;
			break;
		}
	}
	if( byte_ret ){
		sscanf(str_in, "%lf" , float_number);
	}
	return byte_ret;
}

/*!Parse a string which is representing a single instant into a DATE_NUMBERS struct.
 * @param[in] str_in The string which is representing the instant.
 * @param[out] dn_ret The extracted values in a DATE_NUMBERS struct.
 * \return 0 in case of error, otherwise 1 */
pt_error_type parse_single_instant_string(char * str_in, DATE_NUMBERS * dn_ret){

	char * str_arr[6] = { 0x0, 0x0, 0x0, 0x0, 0x0, 0x0 };
	str_arr[0] = str_in;
	char * str_in_end = str_in + strlen(str_in); // Nothing goes behind this pointer.
	pt_error_type ret_err = NO_ERROR;
	char char_delimiter[5] = { '-', '-', 'T', ':', ':' }; // ISO8601-delimiters
	int32 j = 0, i = 0;
	int32 * p_int_dn_year = (int32 *) &(dn_ret->yea);
	char * buf = str_in;
	dn_ret->granularity = YEAR;

	// Scan in and check.
	while( i < 5 ){
    	buf = strchr( str_arr[i], char_delimiter[i] );
    	// No more delimiters - finish.
    	if( buf == NULL ){
    		break;
    	}
    	// Nothing behind the delimiter like e.g. 'YYYY-MM-' ..break and raise an error.
    	if( str_in_end == buf + 1 ) {
    		ret_err = AN_INSTANT_STRING_ENDS_WITH_DELIMITER;
    		break;
    	}
    	// 'YYYY-MM' -> 'YYYY\0MM' and go on.
    	else {
    		buf[0] = '\0';
    		i++;
    		str_arr[ i ] = buf + 1;
    		dn_ret->granularity++;
    	}
	}

	// Handle seconds as floating point if found.
	if( dn_ret->granularity == SECOND ){
		i--;
		if( !parse_single_float(str_arr[ 5 ], &dn_ret->sec) ){
			ret_err = CAN_NOT_PARSE_SECONDS;
		}
	}

	// Read in numbers. TODO has to throw error.
	i++;
	if( ret_err == NO_ERROR ){
		for(j = 0; j < i ; j++){
			if( !parse_single_number(str_arr[j], p_int_dn_year + j ) ){
				ret_err = CAN_NOT_PARSE_INTEGER_VALUE;
				break;
			}
		}
	}
	// TODO this should be a temporally solution.
	if( dn_ret->granularity < DAY ){
		dn_ret->day = 1;
		if( dn_ret->granularity < MONTH ) dn_ret->mon = 1;
	}

	return ret_err;
}

/*!Convert single-instant-strings in JULIAN_DAY-values.
 * @param[in] int_count Count of primitives.
 * @param[in] str_primitives The separated instant-strings.
 * @param[in] str_primitives_end In case of period this holds the end-instants.
 * @param[in/out] ptime_tmp Input are the REFSYS-values and type. If the function is successful finished the data[] is filled. */
pt_error_type instant_strings_to_ptime_instants(int32 *int_count, POSTTIME *ptime_tmp, char **str_primitives, char **str_primitives_end){
	int32 position_end = 0, position = 0, i = 0;
	pt_error_type ret_err = NO_ERROR;
	float8 float_value_tmp = 0, float_value_tmp_2 = 0;
	size_t size_needed_dn;
	DATE_NUMBERS * dn_tmp;
	calendar_era * cal_system;
	coordinate_system * tcs_system;
	ordinal_system * ord_system;
	switch(ptime_tmp->refsys.type){
	case 1:
		// Allocate memory for DATE_NUMBERS
		size_needed_dn = sizeof(DATE_NUMBERS) * (*int_count * ptime_tmp->type);
		dn_tmp = (DATE_NUMBERS *) palloc(size_needed_dn);
		memset(dn_tmp, 0, size_needed_dn);
		// Extract numbers into the DATE_NUMBERS
			if(ptime_tmp->type == 1){

				for( i = 0; i < *int_count; i++ ){
					ret_err = parse_single_instant_string( str_primitives[i], dn_tmp + i );
					if( ret_err != NO_ERROR ) {
						break;
					}
				}
			}
			else if(ptime_tmp->type == 2){
				pt_error_type tmp_err = NO_ERROR;
				for( i = 0; i < *int_count; i++ ){
					ret_err = parse_single_instant_string( str_primitives[i] , dn_tmp + (i * 2) );
					tmp_err = parse_single_instant_string( str_primitives_end[i] , dn_tmp + ((i * 2) + 1) );
					if( ret_err != NO_ERROR ) break;
					if( tmp_err != NO_ERROR) {
						ret_err = tmp_err;
						break;
					}
					if( dn_compare_dn( (dn_tmp +(i*2)) , (dn_tmp +(i*2))+1) != -1){
						ret_err = PERIOD_START_BEFORE_END;
						break;
					}
				}
			}
		if( ret_err == NO_ERROR ){
			cal_system = get_system_from_key( ptime_tmp->refsys.instance );

			if( cal_system == NULL ) {
				ret_err = CALENDAR_SYSTEM_NOT_FOUND;
			}
			else {
				int32 int_count_instants = *int_count * ptime_tmp->type;
				for( i = 0; i < int_count_instants; i++ ){
					ptime_tmp->data[i] = cal_system->dnumber_to_jday( dn_tmp + i );
				}
			}
		}
		FREE_MEM(dn_tmp);
		break;
	case 2:
		tcs_system = get_tcs_system_from_key( ptime_tmp->refsys.instance );
		if( tcs_system == NULL ){
			return TCS_SYSTEM_NOT_FOUND;
		}

		if(ptime_tmp->type == 1){
			for( i = 0 ; i < *int_count ; i++ ){
				if( !parse_single_float( str_primitives[i] , &float_value_tmp) ){
					return CAN_NOT_PARSE_TCS_VALUE;
				}
				else {
					ptime_tmp->data[i] = tcs_to_jday( &float_value_tmp , tcs_system );
				}
			}
		}
		else if(ptime_tmp->type == 2) {
			for( i = 0 ; i < *int_count ; i++ ){
				if( !parse_single_float( str_primitives[i] , &float_value_tmp) ||
						!parse_single_float( str_primitives_end[i] , &float_value_tmp_2)){
					return CAN_NOT_PARSE_TCS_VALUE;
				}
				else {
					if( float_value_tmp >= float_value_tmp_2 ) return PERIOD_START_BEFORE_END;
					ptime_tmp->data[ i*2 ] = tcs_to_jday( &float_value_tmp , tcs_system );
					ptime_tmp->data[ (i*2) + 1 ] = tcs_to_jday( &float_value_tmp_2 , tcs_system );
				}
			}
		}
		break;
	case 3:
		ord_system = get_ordinal_system_from_key( ptime_tmp->refsys.instance );
		if( ord_system == NULL ){
			return ORD_SYSTEM_NOT_FOUND;
		}
		if(ptime_tmp->type == 1){
			for( i = 0 ; i < *int_count ; i++ ){
				position = ordinal_to_jday( str_primitives[i], ord_system );
				if( position == 0) {
					return ORD_ERA_UNKNOWN;
				}
				else {
					ptime_tmp->data[i] = position;
				}
			}
		}
		else if(ptime_tmp->type == 2) {
			for( i = 0 ; i < *int_count ; i++ ){
				position = ordinal_to_jday( str_primitives[i], ord_system );
				position_end = ordinal_to_jday( str_primitives_end[i], ord_system );
				if( position == 0 || position_end == 0) {
					return ORD_ERA_UNKNOWN;
				}
				else {
					if( position >= position_end ) return PERIOD_START_BEFORE_END;
					ptime_tmp->data[i * 2] = position;
					ptime_tmp->data[ (i * 2) + 1] = position_end;
				}
			}
		}
		break;
	}
	if( *int_count > 1 ) ptime_tmp->type = ptime_tmp->type + 2;
	return ret_err;
}
