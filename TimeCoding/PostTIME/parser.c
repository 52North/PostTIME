/*
 * parser.c
 *
 */

#include "parser.h"

/*!Count the containing comma characters to determine count of
 * potential instants.
 * @param[in] str_in The given string.
 * \return Number of (potential) instants.*/
int32 count_of_primitives(char * str_in){
        int32 ret = 0;
        char * buf = str_in - 1;
        do {
        		buf = buf + 1; /* buf points after the last found separator */
                buf = strchr(buf, PRIMITIVE_SEPARATOR);
                ret = ret + 1;
        } while(buf != NULL);  /* try until no more separator is found */
        return ret;
}

/*!This figures out if a string represents an instant or period. An string without PERIOD_SEPARATOR
 * will be interpreted as an instant, one with only one separator as period, if more the function
 * raises an error.
 * @param[in] str_instant The string to analyze.
 * @param[out] str_instant_end If this primitve is a period, this will contain the part that is representing
 * the end instant.
 * \return 1=Instant 2=Period 0=Error. */
BYTE string_is_instant(char * str_instant[], char * str_instant_end[], int32 * i){
	char * str_tmp = strchr(str_instant[*i], PERIOD_SEPARATOR);
	if(str_tmp == NULL) return 1;
	if(NULL != strchr(str_tmp+1, PERIOD_SEPARATOR)) return 0;
	str_instant_end[*i] = str_tmp+1;
	str_tmp[0] = 0;
	return 2;
}

/*!Separate the primitives in the given string and check syntactical validity.
 * This means check if the strings could represent instants in the given refsys.
 * Additionally, check if they are type-homogeneous (INSTANT, PERIOD, etc.).
 * @param[in] str_in The given string.
 * @param[out] str_primitives the single primitive candidates are separated in single strings. */
void separate_primitives(char * str_in, char *str_primitives[]){
    char * buf = strchr(str_primitives[0], PRIMITIVE_SEPARATOR);
    int i = 1;
    while(buf != NULL){
    	str_primitives[i] = buf+1;
    	buf[0] = '\0';
    	buf = strchr(buf+1, PRIMITIVE_SEPARATOR);
    	i++;
    }
}

/*!Check if the given string begins with an valid refsys key.
 * Raise an error if it starts with a broken key and set refsys to
 * Gregorian Calendar with UT if there is no key.
 * @param[in] str_in The given string.
 * @param[out] strs_out Write the address of first primitive candidate in strs_out[0].
 * @param[out] ptime_tmp Write the refsys in ptime_tmp.refsys.*/
pt_error_type determine_refsys(char * str_in, char *strs_out[], POSTTIME * ptime_tmp){
	BYTE type = 0;
	int16 instance = 0;
	pt_error_type err_ret = 0;
	char * str_search;
	void * ptr;
	char key[4] = "000\0";
	int i, int_key = 0;
	for( i = 0; i < 3; i++ ){
		str_search = refsys_types[i];
		ptr = strstr(str_in, str_search);
		if(ptr != NULL && ptr == str_in) {
			type = i + 1;
			break;
		}
	}
	switch( type ){
	case 0:
		type++;
		break;
	case 1:
		strncpy(key, str_in + 3, 3);
		if( !parse_single_number(key, &int_key) ) return CAN_NOT_PARSE_REFERENCE_SYSTEM_KEY;
		for( i = 0; i < NUMBER_OF_CAL_SYS; i++ ){
			if( calendar_systems[i].id == int_key ) {
				instance = int_key;
				break;
			}
		}
		if( !instance ) err_ret = CALENDAR_SYSTEM_NOT_FOUND;
		break;
	case 2:
		strncpy(key, str_in + 3, 3);
		if( !parse_single_number(key, &int_key) ) return 0;
		for( i = 0; i < NUMBER_OF_TCS; i++ ){
			if( tcs_systems[i].id == int_key ) {
				instance = int_key;
				break;
			}
		}
		if( !instance ) err_ret = TCS_SYSTEM_NOT_FOUND;
		break;
	case 3:
		strncpy(key, str_in + 3, 3);
		if( !parse_single_number(key, &int_key) ) return 0;
		for( i = 0; i < NUMBER_OF_ORDINAL; i++ ){
			if( ordinal_systems[i].id == int_key ) {
				instance = int_key;
				break;
			}
		}
		if( !instance ) err_ret = ORD_SYSTEM_NOT_FOUND;
		break;
	}
	if( instance ) strs_out[0] = str_in + 6;
	else {
		strs_out[0] = str_in; // has to point after the refsys key tag.
		instance++;
	}

	ptime_tmp->refsys.type = type;
	ptime_tmp->refsys.instance = instance;

	return err_ret;
}

pt_error_type string_is_regular(POSTTIME * ptime_tmp, char * * str_primitives, char * * str_reg_parts){
	char * buf = 0;
	BYTE type = 0;
	// char * buf_2 = 0; R12/instant/p222/p222
	if( *str_primitives[0] != 'R' ) return EXPECTED_REGULAR_STR_IS_NOT_REGULAR;

	str_reg_parts[0] = str_primitives[0]; // 12/...

	str_primitives[0] = strchr( str_primitives[0] , REGULAR_SEPARATOR );
	if( str_primitives[0] == str_reg_parts[0] ) return EXPECTED_REGULAR_STR_IS_NOT_REGULAR;
	*(str_primitives[0]) = '\0';
	str_primitives[0]++;
	buf = strchr( str_primitives[0], REGULAR_SEPARATOR );
	if( buf == 0 ) return EXPECTED_REGULAR_STR_IS_NOT_REGULAR;
	*buf = '\0';
	str_reg_parts[1] = buf + 1;

	if( *str_reg_parts[1] != 'P' || str_reg_parts[1] ==  str_primitives[0]) return EXPECTED_REGULAR_STR_IS_NOT_REGULAR;
	void * buf2 = strchr( str_reg_parts[1], REGULAR_SEPARATOR );
	if( buf2 == (void*) 0 ){
		type = 5;
	}
	else{
		buf = strchr( str_reg_parts[1], REGULAR_SEPARATOR );
		if( *(buf+1) != 'P' ) return EXPECTED_REGULAR_STR_IS_NOT_REGULAR;
		str_reg_parts[2] = buf + 1;
		*buf = '\0';
		type = 6;
	}
	ptime_tmp->type = type;
	return NO_ERROR;
}

/*!Convert a given string to a POSTTIME instance.
 * @param[in] str_in The given string.
 * @param[in] int_count Former calculated count of primitive candidates.
 * @param[out] ptime_tmp Resulting POSTTIME instance.
 * \return 0 in case of error, otherwise 1 */
pt_error_type string_to_ptime(char * str_in, int32 * int_count, POSTTIME * ptime_tmp){
	int32 i = 0;
	pt_error_type ret_err = NO_ERROR;
	size_t size_needed = sizeof(char *)* *int_count;
	char * * str_primitives = palloc(size_needed);
	char * * str_primitives_end = palloc(size_needed);
	memset(str_primitives_end, 0, size_needed);
	memset(str_primitives, 0, size_needed);

	ret_err = determine_refsys(str_in, str_primitives, ptime_tmp);

	if ( ret_err == NO_ERROR) {
	// Separate the primitives in single strings.
		separate_primitives(str_in, str_primitives);

		// Check period or instant and separate strings (if period). This is kind of laborious because the
		// allocated char memory is needed later.
		BYTE byte_instant_test = string_is_instant(str_primitives, str_primitives_end, &i);

		if( !byte_instant_test ) {
			// At this Point the strings in str_primitives could
			// contain a regularMultiObject. Check for validity and parse.
			// could only be a tcs or calendarClock sys.
			if(*int_count > 1) ret_err = PERIOD_CAN_ONLY_CONTAIN_SINGLE_SLASH;
			else if(ptime_tmp->refsys.type == 3 ) ret_err = FUNCTION_UNDEFINED_FOR_ORDINAL;
			else {
				// str_reg_parts contains [0]=str_r_number, [1]str_p_valid, [2] str_p_invalid
				char * * str_reg_parts = palloc(sizeof(char *) * 3 );
				memset(str_reg_parts, 0 , sizeof(char *) * 3);
				ret_err = string_is_regular(ptime_tmp, str_primitives, str_reg_parts);

				if(ret_err == NO_ERROR) {
					ret_err = regular_strings_to_ptime_instance(ptime_tmp, str_primitives, str_reg_parts);
				}
				FREE_MEM(str_reg_parts);
			}
			// ret_err = PERIOD_CAN_ONLY_CONTAIN_SINGLE_SLASH;
		}
		else {

			ptime_tmp->type = byte_instant_test;
			for(i = 1; i< *int_count; i++){
				BYTE byte_test = string_is_instant(str_primitives, str_primitives_end, &i);
				if(byte_test == 0) {
					ret_err = PERIOD_CAN_ONLY_CONTAIN_SINGLE_SLASH;
					break;
				}
				if(byte_test != ptime_tmp->type) {
					ret_err = PRIMITIVES_MUST_BE_OF_HOMOGENOUS_TYPE;
					break;
				}
			}
			if( ret_err == NO_ERROR ){
				// ptime_tmp.type is 1 or 2.
				// ptime_tmp.refsys is finally set.
				ret_err = instant_strings_to_ptime_instants(int_count, ptime_tmp, str_primitives, str_primitives_end);
			}
		}
	}
	FREE_MEM(str_primitives_end);
	FREE_MEM(str_primitives);
	return ret_err;
}
