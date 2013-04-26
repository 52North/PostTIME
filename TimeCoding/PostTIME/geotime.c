/*
 * geotime.c
 *
 *  Created on: Feb 1, 2013
 *      Author: ptr
 */

#include "geotime.h"
#include "calendars.h"
#include "blackbox_converter.h"

const char * refsys_types[NUMBER_REFSYS_TYPES] = { CALENDAR_FLAG, TEMPORAL_COORDINATE_FLAG, ORDINAL_FLAG };

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
