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
		if( !parse_single_number(key, &int_key) ) return 0;
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

		if( !byte_instant_test ) ret_err = PERIOD_CAN_ONLY_CONTAIN_SINGLE_SLASH;
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
		}
		if( ret_err == NO_ERROR ){
			// ptime_tmp.type is 1 or 2.
			// ptime_tmp.refsys is finally set.
			ret_err = instant_strings_to_ptime_instants(int_count, ptime_tmp, str_primitives, str_primitives_end);

		}
	}
	FREE_MEM(str_primitives_end);
	FREE_MEM(str_primitives);

	return ret_err;
}
