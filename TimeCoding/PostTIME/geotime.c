/*
 * geotime.c
 *
 *  Created on: Feb 1, 2013
 *      Author: ptr
 */

#include "geotime.h"
#include "calendars.h"
#include "blackbox_converter.h"

char postgis_sharedlib[50];

const char * refsys_types[NUMBER_REFSYS_TYPES] = { CALENDAR_FLAG, TEMPORAL_COORDINATE_FLAG, ORDINAL_FLAG };

/*! Convert an RegularMulti into a 'norma' Multi object.
 * @param[in] ptime_reg The regular POSTTIME.
 * @param[out] ptime_multi The resulting MultiInstant or MultiPeriod. */
void regular_multi_to_multi( POSTTIME * ptime_reg , POSTTIME * ptime_multi ){
	DATE_NUMBERS dn_0 , dn_valid, dn_invalid, dn_result;
	int32 rvalue = 0 , i = 0, dummy = 0;
	calendar_era * cal_system;
	ptime_multi->refsys = ptime_reg->refsys;
	ptime_multi->type = ptime_reg->type - 2;
	switch(ptime_reg->refsys.type){
	case 1:
		memset(&dn_valid , 0 , sizeof(DATE_NUMBERS));
		memset(&dn_invalid , 0 , sizeof(DATE_NUMBERS));
		memset(&dn_0 , 0 , sizeof(DATE_NUMBERS) );
		memset(&dn_result , 0 , sizeof(DATE_NUMBERS) );

		cal_system = get_system_from_key( ptime_reg->refsys.instance );
		dn_0 = cal_system->jday_to_dnumbers( &(ptime_reg->data[0]) );
		ptime_multi->data[0] = ptime_reg->data[0];

		if( ptime_reg->type == 6 ) {
			jday_pack_to_dn_period( &dn_valid, (JULIAN_DAY *) &ptime_reg->data[1], &rvalue);
			jday_pack_to_dn_period( &dn_invalid, (JULIAN_DAY *) &ptime_reg->data[5], &dummy);
			dn_result = dn_plus_dn( &dn_0 , &dn_valid, cal_system );
			ptime_multi->data[1] = cal_system->dnumber_to_jday(&dn_result);
		}
		else if( ptime_reg->type == 5) {
			jday_pack_to_dn_period( &dn_invalid, (JULIAN_DAY *) &ptime_reg[1], &rvalue);
			dn_result = dn_plus_dn( &dn_0 , &dn_invalid, cal_system );
			ptime_multi->data[1] = cal_system->dnumber_to_jday(&dn_result);
		}

		if( ptime_reg->type == 5 ) {
			for( i = 2 ; i < rvalue + 1; i++ ){
				dn_result = dn_plus_dn( &dn_result , &dn_invalid , cal_system );
				ptime_multi->data[i] = cal_system->dnumber_to_jday(&dn_result);
			}
		}

		else if( ptime_reg->type == 6 ) {
			for( i = 2 ; i < (rvalue * 2) + 1; i = i + 2 ){
				dn_result = dn_plus_dn( &dn_result , &dn_invalid, cal_system );
				ptime_multi->data[i] = cal_system->dnumber_to_jday(&dn_result);
				dn_result = dn_plus_dn( &dn_result , &dn_valid , cal_system );
				ptime_multi->data[i + 1] = cal_system->dnumber_to_jday(&dn_result);
			}
		}
		break;
	case 2:
		rvalue = (int32) ptime_reg->data[0];
		ptime_multi->data[0] = ptime_reg->data[1];
		if(ptime_reg->type == 5){
			for( i = 1 ; i < rvalue + 1 ; i++ ){
				ptime_multi->data[i] = ptime_multi->data[i - 1] + ptime_reg->data[2];
			}
		}
		else if(ptime_reg->type == 6){
			ptime_multi->data[1] = ptime_multi->data[0] + ptime_reg->data[2];
			for( i = 2 ; i < (rvalue * 2) + 1 ; i = i + 2 ){
				ptime_multi->data[i] = ptime_multi->data[i - 1] + ptime_reg->data[3];
				ptime_multi->data[i + 1] = ptime_multi->data[i] + ptime_reg->data[2];
			}
		}
		break;
	}
}

/*!Get the number of recurrences from a RegularMultiObject.
 * @param[in] ptime_reg
 * @param[out] rvalue
 * \return Makes use of the pt_error_type. */
pt_error_type get_rvalue(POSTTIME * ptime_reg, int32 * rvalue){
	pt_error_type ret_err = NO_ERROR;
	if(ptime_reg->type < 5){
		ret_err = REGULAR_MULTI_FUNCTION;
	}
	else {
		switch( ptime_reg->refsys.type ){
		case 1:
			*rvalue = (int32) ptime_reg->data[1];
			break;
		case 2:
			*rvalue = (int32) ptime_reg->data[0];
			break;
		case 3:
			ret_err = FUNCTION_UNDEFINED_FOR_ORDINAL;
			break;
		}
	}
	return ret_err;
}


pt_error_type transform_regular_system( POSTTIME * ptime , REFSYS * refsys_new , POSTTIME * ptime_new ){
	DATE_NUMBERS dn_0 , dn_valid, dn_invalid, dn_result, dn_result_2;
	JULIAN_DAY tmp = 0;
	memset(&dn_valid , 0 , sizeof(DATE_NUMBERS));
	memset(&dn_invalid , 0 , sizeof(DATE_NUMBERS));
	memset(&dn_0 , 0 , sizeof(DATE_NUMBERS) );
	memset(&dn_result , 0 , sizeof(DATE_NUMBERS) );
	memset(&dn_result_2 , 0 , sizeof(DATE_NUMBERS) );
	int32 rvalue = 0;
	calendar_era * cal_system;
	ptime_new->type = ptime->type;
	ptime_new->refsys = *refsys_new;
	switch( ptime->refsys.type ){
	case 1:
		cal_system = get_system_from_key( ptime->refsys.instance );
		dn_0 = cal_system->jday_to_dnumbers( &(ptime->data[0]) );
		jday_pack_to_dn_period( &dn_valid, (JULIAN_DAY *) &ptime->data[1], &rvalue);
		dn_result = dn_plus_dn( &dn_0 , &dn_valid, cal_system );
		ptime_new->data[0] = rvalue;
		ptime_new->data[1] = ptime->data[0];
		ptime_new->data[2] = cal_system->dnumber_to_jday( &dn_result ) - ptime->data[0];
		if( ptime->type == 6 ){
			jday_pack_to_dn_period( &dn_invalid, (JULIAN_DAY *) &ptime->data[5], &rvalue);
			dn_result_2 = dn_plus_dn( &dn_result , &dn_valid, cal_system );
			ptime_new->data[3] = cal_system->dnumber_to_jday( &dn_result_2 ) - ptime_new->data[2];
		}
		break;
	case 2:
		rvalue = ptime->data[0];
		ptime_new->data[0] = ptime->data[1];
		cal_system = get_system_from_key( refsys_new->instance );
		dn_0 = cal_system->jday_to_dnumbers( &ptime->data[1] );
		tmp = ptime->data[1] + ptime->data[2];
		dn_valid = cal_system->jday_to_dnumbers( &tmp );
		dn_result = dn_minus_dn( &dn_valid , &dn_0 , cal_system );
		dn_period_to_jday_pack( &dn_result , &ptime_new->data[1] , rvalue );
		if( ptime->type == 6 ){
			tmp = tmp + ptime->data[3];
			dn_invalid = cal_system->jday_to_dnumbers( &tmp );
			dn_result_2 = dn_minus_dn( &dn_invalid , &dn_valid , cal_system );
			dn_period_to_jday_pack( &dn_result_2 , &ptime_new->data[5] , 0 );
		}
		break;
	}

	return NO_ERROR;
}
