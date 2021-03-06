
#include "distance_length.h"

/*!ISO19108 primitive function. tm_distance */
void distance_jul_day(POSTTIME * ptime1, POSTTIME * ptime2, float8 * jul_day_out){
	int32 relative_pos = 0;
	relative_position_rp(ptime1, ptime2, &relative_pos);
	if( (ptime1->type > 2) || (ptime2->type > 2) ){
		*jul_day_out = -1;
	}
	else {
		/* both types are primitive */
		/* I&I=4 ;I&P=7 ;P&I=5 ;P&P=8 ptime1 is 'Before' ptime2  if 'After' -> +12 */
		switch(relative_pos + ptime1->type +ptime2->type*3){
		case 4:
		case 7:
			*jul_day_out = ptime2->data[0] - ptime1->data[0];
			*jul_day_out = *jul_day_out / (float8) MILLIS;
			break;
		case 5:
		case 8:
			*jul_day_out = ptime2->data[0] - ptime1->data[1];
			*jul_day_out = *jul_day_out / (float8) MILLIS;
			break;
		case 16:
		case 17:
			*jul_day_out = ptime1->data[0] - ptime2->data[0];
			*jul_day_out = *jul_day_out / (float8) MILLIS;
			break;
		case 19:
		case 20:
			*jul_day_out = ptime1->data[0] - ptime2->data[1];
			*jul_day_out = *jul_day_out / (float8) MILLIS;
			break;
		}
	}
}

/*!ISO19108 primitive function. tm_distance */
void distance_string(POSTTIME * ptime1, POSTTIME * ptime2, char * distance_out){
	int32 relative_pos = 0;

	if( (ptime1->type > 2) || (ptime2->type > 2) ){
		strcpy( distance_out , "NAP" );
	}
	else{
		DATE_NUMBERS * dn_1 = (DATE_NUMBERS *) palloc(sizeof(DATE_NUMBERS));
		DATE_NUMBERS * dn_2 = (DATE_NUMBERS *) palloc(sizeof(DATE_NUMBERS));
		DATE_NUMBERS * dn_3 = (DATE_NUMBERS *) palloc(sizeof(DATE_NUMBERS));
		memset(dn_1, '\0', sizeof(DATE_NUMBERS));
		memset(dn_2, '\0', sizeof(DATE_NUMBERS));
		memset(dn_3, '\0', sizeof(DATE_NUMBERS));

		relative_position_rp(ptime1, ptime2, &relative_pos);

		calendar_era * the_cal;
		if( ptime1->refsys.type == 2 ){
			the_cal = get_system_from_key( 1 );
		}
		else{
			the_cal = get_system_from_key( ptime1->refsys.instance );
		}
		/* I&I=4 ;I&P=7 ;P&I=5 ;P&P=8 ptime1 is 'Before' ptime2  if 'After' +12 */
		switch(relative_pos + ptime1->type +ptime2->type*3){
		case 4:
		case 7:
			*dn_1 = the_cal->jday_to_dnumbers( &ptime1->data[0] );
			*dn_2 = the_cal->jday_to_dnumbers( &ptime2->data[0] );
			*dn_3 = dn_minus_dn(dn_2,dn_1,the_cal);
			dn_duration_to_string(dn_3, distance_out);
			break;
		case 5:
		case 8:
			*dn_1 = the_cal->jday_to_dnumbers( &ptime1->data[1] );
			*dn_2 = the_cal->jday_to_dnumbers( &ptime2->data[0] );
			*dn_3 = dn_minus_dn(dn_2,dn_1,the_cal);
			dn_duration_to_string(dn_3, distance_out);
			break;
		case 16:
		case 17:
			*dn_1 = the_cal->jday_to_dnumbers( &ptime1->data[0] );
			*dn_2 = the_cal->jday_to_dnumbers( &ptime2->data[0] );
			*dn_3 = dn_minus_dn(dn_1,dn_2,the_cal);
			dn_duration_to_string(dn_3, distance_out);
			break;
		case 19:
		case 20:
			*dn_1 = the_cal->jday_to_dnumbers( &ptime1->data[0] );
			*dn_2 = the_cal->jday_to_dnumbers( &ptime2->data[1] );
			*dn_3 = dn_minus_dn(dn_1,dn_2,the_cal);
			dn_duration_to_string(dn_3, distance_out);
			break;
		default:
			// distance is 0
			strcpy( distance_out , P0_8601_STRING );
			break;
		}
		FREE_MEM(dn_1);
		FREE_MEM(dn_2);
		FREE_MEM(dn_3);
	}
}

/*!ISO19108 primitive function. tm_distance */
void duration_jul_day( POSTTIME * ptime , float8 * jul_day_out){
	if( ptime->type > 2 ){
		*jul_day_out = -1;
	}
	else if ( ptime->type == 1 ){
		*jul_day_out = 0;
	}
	else if ( ptime->type == 2 ){
		*jul_day_out = (ptime->data[1] - ptime->data[0]) / (float8) MILLIS;
	}
}

/*!ISO19108 primitive function. tm_distance */
void duration_string(POSTTIME * ptime, char * duration_out){
	if( ptime->type > 2 ){
		strcpy( duration_out , "NAP" );
	}
	else if ( ptime->type == 1 ){
		strcpy( duration_out , P0_8601_STRING );
	}
	else if ( ptime->type == 2 ){
		calendar_era * the_cal;
		if( ptime->refsys.type == 2 ){
			the_cal = get_system_from_key( 1 );
		}
		else{
			the_cal = get_system_from_key( ptime->refsys.instance );
		}
		DATE_NUMBERS * dn_1 = (DATE_NUMBERS *) palloc(sizeof(DATE_NUMBERS));
		DATE_NUMBERS * dn_2 = (DATE_NUMBERS *) palloc(sizeof(DATE_NUMBERS));
		DATE_NUMBERS * dn_3 = (DATE_NUMBERS *) palloc(sizeof(DATE_NUMBERS));
		memset(dn_1, '\0', sizeof(DATE_NUMBERS));
		memset(dn_2, '\0', sizeof(DATE_NUMBERS));
		memset(dn_3, '\0', sizeof(DATE_NUMBERS));

		*dn_1 = the_cal->jday_to_dnumbers( &ptime->data[0] );
		*dn_2 = the_cal->jday_to_dnumbers( &ptime->data[1] );
		*dn_3 = dn_minus_dn(dn_2,dn_1,the_cal);
		dn_duration_to_string(dn_3, duration_out);

		FREE_MEM(dn_1);
		FREE_MEM(dn_2);
		FREE_MEM(dn_3);
	}
}



