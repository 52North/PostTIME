/*
 * textualizator.c
 */

#include "textualizator.h"

void refsys_tag( POSTTIME * ptime , char * str_out ){
	char str_tmp[4];
	strncpy( str_out , refsys_types[ptime->refsys.type - 1] , 3 );
	sprintf( str_tmp, "%03d" , ptime->refsys.instance );
	strncpy( str_out + 3 , str_tmp , 3 );
}

void julianday_cat_instantstring(float8 * value, char * str){
	char str1[27];
	snprintf(str1,27,"%016.6lf",*value);
	strncat(str,str1,27);
}

void date_numbers_to_instant_string(DATE_NUMBERS * dn, char * str_out){
	char str_tmp[28];
	snprintf(str_tmp, 28, "%06d-%02d-%02dT%02d:%02d:%06.3lfZ",dn->yea,dn->mon,dn->day,dn->hou,dn->min,dn->sec);
	strncat( str_out ,str_tmp, 27 );
}

// TODO comments.
void dn_duration_to_string(DATE_NUMBERS * dn, char * str){
	char str1[33];
	memset( str1 , 0 , 33 );
	sprintf(str1,"P");
	strcat(str,str1);
	if(dn->yea != 0){
		sprintf(str1,"%dY",dn->yea);
		strcat(str,str1);
	}
	if(dn->mon != 0){
		sprintf(str1,"%dM",dn->mon);
		strcat(str,str1);
	}
	if(dn->day != 0){
		sprintf(str1,"%dD",dn->day);
		strcat(str,str1);
	}
	if(dn->hou != 0 || dn->min != 0 || dn->sec != 0){
		sprintf(str1,"T");
		strcat(str,str1);
	}
	if(dn->hou != 0){
		sprintf(str1,"%dH",dn->hou);
		strcat(str,str1);
	}
	if(dn->min != 0){
		sprintf(str1,"%dM",dn->min);
		strcat(str,str1);
	}
	if(dn->sec != 0){
		sprintf(str1,"%.3lfS",dn->sec);
		strcat(str,str1);
	}
}

void ptime_to_string_calendar( POSTTIME * ptime , char * str_out , int32 * int_count ){
	int32 i = 0;
	size_t size_needed;
	size_needed =  sizeof(DATE_NUMBERS) * *int_count;
	if(ptime->type == 5) size_needed = sizeof(DATE_NUMBERS) * 2;
	else if(ptime->type == 6) size_needed = sizeof(DATE_NUMBERS) * 3;
	char period_sep = PERIOD_SEPARATOR;
	char primitive_sep = PRIMITIVE_SEPARATOR;
	int32 rvalue = 0;
	DATE_NUMBERS * dn_tmp = palloc( size_needed );
	DATE_NUMBERS dn_0;
	memset( dn_tmp , 0 , size_needed );
	memset( &dn_0 , 0 , sizeof(DATE_NUMBERS) );
	calendar_era * cal_system = get_system_from_key( ptime->refsys.instance );
	if( ptime->type > 4 ) {
		dn_0 = cal_system->jday_to_dnumbers( &(ptime->data[0]) );
		memcpy(dn_tmp, &dn_0, sizeof(DATE_NUMBERS));
		jday_pack_to_dn_period( dn_tmp + 1, (JULIAN_DAY *) &ptime->data[1], &rvalue);
		if( ptime->type == 6 ) {
			jday_pack_to_dn_period( dn_tmp + 2, (JULIAN_DAY *) &ptime->data[5], &i);
		}
		char str_r[15];
		sprintf(str_r,"R%d",rvalue);
		strcat(str_out,str_r);
		strncat(str_out,&period_sep,1);
	}
	else {
		for( i = 0; i < *int_count; i++ ){
			dn_0 = cal_system->jday_to_dnumbers( &(ptime->data[i]) );
			memcpy(dn_tmp+i, &dn_0, sizeof(DATE_NUMBERS));
			memset( &dn_0 , 0 , sizeof(DATE_NUMBERS) );
		}
	}

	switch(ptime->type){
	case 1:
		date_numbers_to_instant_string( dn_tmp , str_out); //TODO
		break;
	case 2:
		date_numbers_to_instant_string( dn_tmp , str_out);
		strncat( str_out , &period_sep , 1 );
		date_numbers_to_instant_string( dn_tmp + 1 , str_out);
		break;
	case 3:
		date_numbers_to_instant_string( dn_tmp , str_out);
		for(i = 1; i < *int_count; i++){
			strncat( str_out , &primitive_sep , 1 );
			date_numbers_to_instant_string( dn_tmp + i , str_out);
		}
		break;
	case 4:
		date_numbers_to_instant_string( dn_tmp , str_out );
		strncat(str_out,&period_sep,1);
		date_numbers_to_instant_string( dn_tmp + 1 , str_out );
		for(i = 2; i < *int_count; i=i+2){
			strncat(str_out,&primitive_sep,1);
			date_numbers_to_instant_string( dn_tmp + i , str_out );
			strncat(str_out,&period_sep,1);
			date_numbers_to_instant_string( dn_tmp + i + 1 , str_out );
		}
		break;
	case 5:
		date_numbers_to_instant_string( dn_tmp , str_out );
		strncat(str_out,&period_sep,1);
		dn_duration_to_string( dn_tmp+1 , str_out );
		break;
	case 6:
		date_numbers_to_instant_string( dn_tmp , str_out );
		strncat(str_out,&period_sep,1);
		dn_duration_to_string( dn_tmp+1 , str_out );
		strncat(str_out,&period_sep,1);
		dn_duration_to_string( dn_tmp+2 , str_out );
		break;
	}
	FREE_MEM(dn_tmp);
}


void ptime_to_string_tcs( POSTTIME * ptime , char * str_out , int32 * int_count ){
	// TODO convert all JULIAN_DAYS in float8s
	int32 i = 0, ii = 0, int_bound = 0;
	char period_sep = PERIOD_SEPARATOR;
	char primitive_sep = PRIMITIVE_SEPARATOR;
	coordinate_system * tcs_system = get_tcs_system_from_key( ptime->refsys.instance );
	char str_tmp[200];
	if( ptime->type > 4 ) {
		ii = 1;
		if(ptime->type == 5) int_bound = 3;
		else int_bound = 4;
		char str_r[15];
		sprintf( str_r , "R%d" , (int32) ptime->data[0] );
		strcat(str_out,str_r);
		strncat(str_out,&period_sep,1);
	}
	else int_bound = *int_count;

	float8 * float_values = palloc(sizeof(float8) * int_bound);
	memset( float_values , 0 , sizeof(float8) * int_bound);
	for( i = ii ; i < int_bound ; i++ ){
		float_values[i] = jday_to_tcs(&ptime->data[i], tcs_system);
	}
	switch(ptime->type){
	case 1:
		julianday_cat_instantstring( &float_values[0] , str_out);
		break;
	case 2:
		julianday_cat_instantstring( &float_values[0] , str_out);
		strncat( str_out , &period_sep , 1 );
		julianday_cat_instantstring( &float_values[1] , str_out);
		break;
	case 3:
		julianday_cat_instantstring( &float_values[0] , str_out);
		for(i = 1; i < *int_count; i=i+1){
			strncat( str_out , &primitive_sep , 1 );
			julianday_cat_instantstring( &float_values[i] , str_out);
		}
		break;
	case 4:
		julianday_cat_instantstring( &float_values[0] , str_out );
		strncat(str_out,&period_sep,1);
		julianday_cat_instantstring( &float_values[1] , str_out );
		for(i = 2; i < *int_count; i=i+2){
			strncat(str_out,&primitive_sep,1);
			julianday_cat_instantstring( &float_values[i] , str_out );
			strncat(str_out,&period_sep,1);
			julianday_cat_instantstring( &float_values[i+1] , str_out );
		}
		break;
	case 5:
		julianday_cat_instantstring( &float_values[1] , str_out );
		strncat(str_out,&period_sep,1);
		sprintf( str_tmp , "P%lf" , float_values[2] );
		strcat( str_out , str_tmp );
		break;
	case 6:
		julianday_cat_instantstring( &float_values[1] , str_out );
		strncat( str_out , &period_sep,1);
		sprintf( str_tmp , "P%lf" , float_values[2] );
		strcat( str_out , str_tmp );
		memset( str_tmp , 0 , 200 );
		strncat(str_out,&period_sep,1);
		sprintf( str_tmp , "P%lf" , float_values[3] );
		strcat( str_out , str_tmp );
		break;
	}
	FREE_MEM(float_values);
}

void ptime_to_string_ordinal( POSTTIME * ptime , char * str_out , int32 * int_count ){
	// TODO convert all JULIAN_DAYS in float8s
	int32 i = 0;
	char period_sep = PERIOD_SEPARATOR;
	char primitive_sep = PRIMITIVE_SEPARATOR;
	ordinal_system * ord_system = get_ordinal_system_from_key( ptime->refsys.instance );
	if( !ord_system ) {
		return; //TODO error handling.
	}
	switch(ptime->type){
	case 1:
		strcat( str_out , jday_to_ordinal( (int32) ptime->data[0], ord_system));
		break;
	case 2:
		strcat( str_out , jday_to_ordinal( (int32) ptime->data[0], ord_system));
		strncat( str_out , &period_sep , 1 );
		strcat( str_out , jday_to_ordinal( (int32) ptime->data[1], ord_system));
		break;
	case 3:
		strcat( str_out , jday_to_ordinal( (int32) ptime->data[0], ord_system));
		for(i = 1; i < *int_count; i=i+1){
			strncat( str_out , &primitive_sep , 1 );
			strcat( str_out , jday_to_ordinal( (int32) ptime->data[i], ord_system));
		}
		break;
	case 4:
		strcat( str_out , jday_to_ordinal( (int32) ptime->data[0], ord_system));
		strncat(str_out,&period_sep,1);
		strcat( str_out , jday_to_ordinal( (int32) ptime->data[1], ord_system));
		for(i = 2; i < *int_count; i=i+2){
			strncat(str_out,&primitive_sep,1);
			strcat( str_out , jday_to_ordinal( (int32) ptime->data[i], ord_system));
			strncat(str_out,&period_sep,1);
			strcat( str_out , jday_to_ordinal( (int32) ptime->data[ i+1 ], ord_system));
		}
		break;
	}
}

/*!Produce the alpha-numerical presentation of the given POSTTIME instance.
 * @param[in] ptime. A POSTTIME instance.
 * @param[out] str_out The produced string.
 * @param[in] int_count Count of this instance's primitives.
 * \return  */
int32 ptime_to_string( POSTTIME * ptime , char * str_out , int32 * int_count ){
	refsys_tag( ptime , str_out);
	switch ( ptime->refsys.type ) {
	case 0:
	case 1:
		ptime_to_string_calendar( ptime , str_out , int_count );
		break;
	case 2:
		ptime_to_string_tcs( ptime , str_out , int_count );
		break;
	case 3:
		ptime_to_string_ordinal( ptime , str_out , int_count );
		break;
	}
	return 0;

}


