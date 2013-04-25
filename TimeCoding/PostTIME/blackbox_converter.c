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

	i++;
	if( ret_err == NO_ERROR ){
		for(j = 0; j < i ; j++){
			if( !parse_single_number(str_arr[j], p_int_dn_year + j ) ){
				ret_err = CAN_NOT_PARSE_INTEGER_VALUE;
				break;
			}
		}
	}
	if( dn_ret->granularity < DAY ){
		dn_ret->day = 1;
		if( dn_ret->granularity < MONTH ) dn_ret->mon = 1;
	}

	return ret_err;
}

pt_error_type parse_single_duration_string(char * str_in, DATE_NUMBERS * dn){
	pt_error_type err_ret = NO_ERROR;
	const char calendar_value_identifier[] = {'Y','M','D'};
	const char clock_value_identifier[] = {'H','M','S'};
	char * clock_part = strchr(str_in,'T');
	char * dur_str = str_in+1;
	char * day_part_boundary;
	if(clock_part == NULL) day_part_boundary = str_in + strlen(str_in);
	else day_part_boundary = clock_part;
	int32 err_byte = 1, i = 0;
	/* search for calendar tags Y or_and M or_and D */
	while((clock_part != dur_str) && (i<3) && err_byte == 1){
		char * tmp = strchr(dur_str ,calendar_value_identifier[i]);
		if(tmp != NULL && day_part_boundary > tmp){
			*tmp = '\0';
			switch(i){
			case 0:
				err_byte = parse_single_number(dur_str, &dn->yea);
				break;
			case 1:
				err_byte = parse_single_number(dur_str, &dn->mon);
				break;
			case 2:
				err_byte = parse_single_number(dur_str, &dn->day);
				break;
			}
			dur_str = tmp+1;
		}
		i = i+1;
	}
	i = 0;
	/* search for clock tags H, M and S */
	if(clock_part != NULL && err_byte == 1){
		dur_str = clock_part +1;
		for(i = 0;i<3;i=i+1){
			char * tmp = strchr(dur_str,clock_value_identifier[i]);
			if(tmp != NULL){
				*tmp = '\0';
				switch(i){
				case 0:
					err_byte = parse_single_number(dur_str, &dn->hou);
					break;
				case 1:
					err_byte = parse_single_number(dur_str, &dn->min);
					break;
				case 2:
					err_byte = parse_single_float(dur_str, &dn->sec);
					break;
				}
				dur_str = tmp+1;
				if(err_byte != 1) break;
			}
		}
		i = 0;
	}
	if(err_byte != 1){
		err_ret = INVALID_SYNTAX_AT_OR_NEAR_REGULAR_VALUE;
	}
	clock_part = NULL;
	dur_str = NULL;
	return err_ret;
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

void dn_period_to_jday_pack(DATE_NUMBERS * period_vals , JULIAN_DAY * jd_ptr , int32 rvalue){
	int32 int32_array[6] = { rvalue,
			period_vals->yea,
			period_vals->mon,
			period_vals->day,
			period_vals->hou,
			period_vals->min };
	memcpy( jd_ptr , int32_array , sizeof(JULIAN_DAY) * 3 );
	*(jd_ptr + 3) = (int64) (period_vals->sec * MILLIS);
}

void jday_pack_to_dn_period(DATE_NUMBERS * period_vals , JULIAN_DAY * jd_ptr , int32 * rvalue){
	int32 int32_array[6] = { 0,0,0,0,0,0 };
	memcpy( int32_array , jd_ptr , sizeof(JULIAN_DAY) * 3 );
	int32 * debug_ptr = (int32 *) jd_ptr;
	period_vals->sec = ((float8) *(jd_ptr + 3)) / ((float8) MILLIS);
	*rvalue = int32_array[0];
	period_vals->yea = int32_array[1];
	period_vals->mon = int32_array[2];
	period_vals->day = int32_array[3];
	period_vals->hou = int32_array[4];
	period_vals->min = int32_array[5];
}

// TODO implement for TCS! ..use switch - cases..
pt_error_type regular_strings_to_ptime_instance(POSTTIME * ptime_tmp, char ** str_primitives, char * * str_reg_parts){
	pt_error_type err_ret = NO_ERROR;
	int32 int_r_value;
	calendar_era * cal_system;
	coordinate_system * tcs_system;
	DATE_NUMBERS * dn_start;
	DATE_NUMBERS * dn_period_valid;
	DATE_NUMBERS * dn_period_invalid;
	float8 float_instant = 0, float_period_valid = 0, float_period_invalid = 0;
	if( parse_single_number( str_reg_parts[0] + 1, &int_r_value ) != 1){
		err_ret = INVALID_SYNTAX_AT_OR_NEAR_REGULAR_VALUE;
	}
	else {
		switch(ptime_tmp->refsys.type){
		case 1:
			dn_start = palloc(sizeof(DATE_NUMBERS));
			memset(dn_start , 0 , sizeof(DATE_NUMBERS));
			dn_period_valid = palloc(sizeof(DATE_NUMBERS));
			memset(dn_period_valid , 0 , sizeof(DATE_NUMBERS));
			dn_period_invalid = palloc(sizeof(DATE_NUMBERS));
			memset(dn_period_invalid , 0 , sizeof(DATE_NUMBERS));

			// str_reg_parts contains [0]=str_r_number, [1]str_p_valid, [2] str_p_invalid
			if(err_ret == NO_ERROR) err_ret = parse_single_instant_string( str_primitives[0] , dn_start);
			if( err_ret == NO_ERROR ){
				cal_system = get_system_from_key( ptime_tmp->refsys.instance );
				if( cal_system == NULL ) {
					err_ret = CALENDAR_SYSTEM_NOT_FOUND;
				}
			}
			if(err_ret == NO_ERROR) {
				ptime_tmp->data[0] = cal_system->dnumber_to_jday( dn_start );
				switch(ptime_tmp->type){
				case 5:
					err_ret = parse_single_duration_string( str_reg_parts[1] , dn_period_invalid );
					dn_period_to_jday_pack( dn_period_invalid , &ptime_tmp->data[1] , int_r_value);
					break;
				case 6:
					err_ret = parse_single_duration_string( str_reg_parts[1] , dn_period_valid );
					dn_period_to_jday_pack( dn_period_valid , &ptime_tmp->data[1] , int_r_value);
					if(err_ret == NO_ERROR) err_ret = parse_single_duration_string( str_reg_parts[2] , dn_period_invalid );
					dn_period_to_jday_pack( dn_period_invalid , &ptime_tmp->data[5] , 0);
					break;
				}
			}
			FREE_MEM(dn_period_valid);
			FREE_MEM(dn_period_invalid);
			FREE_MEM(dn_start);
			break;
		case 2:
			tcs_system = get_tcs_system_from_key( ptime_tmp->refsys.instance );
			if( tcs_system == NULL ){
				return TCS_SYSTEM_NOT_FOUND;
			}
			if( !parse_single_float( str_primitives[0] , &float_instant) ||
				!parse_single_float( str_reg_parts[1] + 1 , &float_period_valid) ){
				return CAN_NOT_PARSE_TCS_VALUE;
			}
			if( ptime_tmp->type == 6  ){
				if(!parse_single_float( str_reg_parts[2] + 1, &float_period_invalid))
				return CAN_NOT_PARSE_TCS_VALUE;
			}
			ptime_tmp->data[0] = (int64) int_r_value;
			ptime_tmp->data[1] = tcs_to_jday( &float_instant , tcs_system);
			ptime_tmp->data[2] = tcs_to_jday( &float_period_valid , tcs_system);
			if(ptime_tmp->type == 6) {
				ptime_tmp->data[3] = tcs_to_jday( &float_period_invalid , tcs_system);
			}
			break;
		}
	}
	return NO_ERROR;
}

