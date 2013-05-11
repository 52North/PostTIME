/*
 * relative_position.c
 *
 *  Created on: 05.05.2013
 *      Author: ptr
 */

#include "relative_position.h"

/*!Set str to the adequate TM_RelativePosition string.
 * param[in] i The enumeration number.
 * param[in] str Write into this string. */
void set_str(int32 * i, char * str){
	switch(*i){
	case 0:
		strcpy(str , "Before");
		break;
	case 1:
		strcpy(str , "Meets");
		break;
	case 2:
		strcpy(str , "Overlaps");
		break;
	case 3:
		strcpy(str , "Begins");
		break;
	case 4:
		strcpy(str , "BegunBy");
		break;
	case 5:
		strcpy(str , "During");
		break;
	case 6:
		strcpy(str , "Contains");
		break;
	case 7:
		strcpy(str , "Equals");
		break;
	case 8:
		strcpy(str , "OverlappedBy");
		break;
	case 9:
		strcpy(str , "Ends");
		break;
	case 10:
		strcpy(str , "EndedBy");
		break;
	case 11:
		strcpy(str , "MetBy");
		break;
	case 12:
		strcpy(str , "After");
		break;
	case 13:
		strcpy(str , "NAP");
		break;
	default:
		break;
	}
}

/*!ISO19108 relative position subroutine - case instant to instant.
 * \return int32 0\13 Allen's relationships + Not a primitive. */
int32 relative_position_ii(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[0] < ptime_2->data[0])return 0;
	else if(ptime_1->data[0] == ptime_2->data[0])return 7;
	else if(ptime_1->data[0] > ptime_2->data[0])return 12;
	else return 13;
}

/*!ISO19108 relative position subroutine - case period to instant.
 * \return int32 0\13 Allen's relationships + Not a primitive. */
int32 relative_position_pi(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[1] < ptime_2->data[0])return 0;
	else if(ptime_1->data[1] == ptime_2->data[0])return 10;
	else if((ptime_1->data[0] < ptime_2->data[0]) && (ptime_1->data[1] > ptime_2->data[0]))return 6;
	else if(ptime_1->data[0] == ptime_2->data[0])return 4;
	else if(ptime_1->data[0] > ptime_2->data[0])return 12;
	else return 13;
}

/*!ISO19108 relative position subroutine - case instant to period.
 * \return int32 0\13 Allen's relationships + Not a primitive. */
int32 relative_position_ip(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[0] < ptime_2->data[0])return 0;
	else if(ptime_1->data[0] == ptime_2->data[0])return 3;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[0] < ptime_2->data[1])return 5;
	else if(ptime_1->data[0] == ptime_2->data[1])return 9;
	else if(ptime_1->data[0] > ptime_2->data[1])return 12;
	else return 13;
}

/*!ISO19108 relative position subroutine - case period to period.
 * \return int32 0\13 Allen's relationships + Not a primitive. */
int32 relative_position_pp(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[1] < ptime_2->data[0])return 0;
	else if(ptime_1->data[1] == ptime_2->data[0])return 1;
	else if(ptime_1->data[0] < ptime_2->data[0] && ptime_1->data[1] > ptime_2->data[0] && ptime_1->data[1] < ptime_2->data[1])return 2;
	else if(ptime_1->data[0] == ptime_2->data[0] && ptime_1->data[1] < ptime_2->data[1])return 3;
	else if(ptime_1->data[0] == ptime_2->data[0] && ptime_1->data[1] > ptime_2->data[1])return 4;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[1] < ptime_2->data[1])return 5;
	else if(ptime_1->data[0] < ptime_2->data[0] && ptime_1->data[1] > ptime_2->data[1])return 6;
	else if(ptime_1->data[0] == ptime_2->data[0] && ptime_1->data[1] == ptime_2->data[1])return 7;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[0] < ptime_2->data[1] && ptime_1->data[1] > ptime_2->data[1])return 8;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[1] == ptime_2->data[1])return 9;
	else if(ptime_1->data[0] < ptime_2->data[0] && ptime_1->data[1] == ptime_2->data[1])return 10;
	else if(ptime_1->data[0] == ptime_2->data[1])return 11;
	else if(ptime_1->data[0] > ptime_2->data[1])return 12;
	else return 13;
}

/*!ISO19108 relative position implementation.
 * \return int32 0\12 Allen's relationships or 13= Not a primitive. */
pt_error_type relative_position_int(POSTTIME * ptime_1,POSTTIME * ptime_2, int32 * ret_int32){
	pt_error_type err_ret = NO_ERROR;
	if( (ptime_1->type > 2) || (ptime_2->type > 2) ){
		*ret_int32 = -1;
	}
	else if((ptime_1->type == 1) && (ptime_2->type == 1)){
		*ret_int32 = relative_position_ii(ptime_1,ptime_2);
	}
	else if((ptime_1->type == 2) && (ptime_2->type == 1)){
		*ret_int32 = relative_position_pi(ptime_1,ptime_2);
	}
	else if((ptime_1->type == 1) && (ptime_2->type == 2)){
		*ret_int32 = relative_position_ip(ptime_1,ptime_2);
	}
	else if((ptime_1->type == 2) && (ptime_2->type == 2)){
		*ret_int32 = relative_position_pp(ptime_1,ptime_2);
	}
	return err_ret;
}

/*!ISO19108 relative position implementation.
 * \return TM_RelativePosition enumeration type. */
pt_error_type relative_position_str(POSTTIME * ptime_1 , POSTTIME * ptime_2 , char * str){
	int32 rp = 13;
	pt_error_type err_ret = relative_position_int(ptime_1 , ptime_2 , &rp);
	if( err_ret == NO_ERROR ){
		if(rp == -1){
			rp = 13;
		}
		set_str(&rp,str);
	}
	return err_ret;
}

/*! Check if two instances share a simultaneous instant.
 * @param[in] ptime1
 * @param[in] ptime2
 * @param[out] is_simultaneous
 * \return Makes use of the pt_error_type. */
pt_error_type simultaneous(POSTTIME * ptime1 , POSTTIME * ptime2 , bool * is_simultaneous){
	*is_simultaneous = FALSE;
	pt_error_type err_ret = NO_ERROR;
	int32 type_1 = 0 , type_2 = 0 , i = 0 , j = 0 , count_1 = 0 , count_2 = 0;
	size_t size_needed = 0;
	POSTTIME * ptime1_intern;
	POSTTIME * ptime2_intern;

	if( ptime1->refsys.type == 3 || ptime2->refsys.type == 3 ){
		err_ret = FUNCTION_UNDEFINED_FOR_ORDINAL;
	}
	else {

		/* RegularMultiObjects into MultiObjects */
		if( ptime1->type > 4 ){
			get_rvalue(ptime1 , &i );
			size_needed = sizeof(POSTTIME) + ( sizeof(JULIAN_DAY) * (( i + 1 ) * ( ptime1->type - 4 ) - 1));
			ptime1_intern = palloc( size_needed );
			memset(ptime1_intern,'\0', size_needed );
			regular_multi_to_multi( ptime1, ptime1_intern );
			SET_VARSIZE( ptime1_intern, size_needed );
		}
		else ptime1_intern = ptime1;
		if( ptime2->type > 4 ){
			get_rvalue(ptime2 , &i );
			size_needed = sizeof(POSTTIME) + ( sizeof(JULIAN_DAY) * (( i + 1 ) * ( ptime2->type - 4 ) - 1));
			ptime2_intern = palloc( size_needed );
			memset(ptime2_intern,'\0', size_needed );
			regular_multi_to_multi( ptime2, ptime2_intern );
			SET_VARSIZE( ptime2_intern, size_needed );
		}
		else ptime2_intern = ptime2;

		count_1 = ( ( VARSIZE(ptime1) ) - sizeof(POSTTIME) )/sizeof(JULIAN_DAY) + 1;
		count_2 = ( ( VARSIZE(ptime2) ) - sizeof(POSTTIME) )/sizeof(JULIAN_DAY) + 1;

		if(ptime1_intern->type == 4 || ptime1_intern->type == 2) type_1 = 2;
		else type_1 = 1;
		if(ptime2_intern->type == 4 || ptime2_intern->type == 2) type_2 = 2;
		else type_2 = 1;

		if( type_1 == 1 && type_2 == 1){
			for( i = 0 ; i < count_1 ; i++ ){
				for( j = 0 ; j < count_2 ; j++ ){
					if( ptime1_intern->data[i] == ptime2_intern->data[j] ){
						*is_simultaneous = TRUE;
						break;
					}
				}
				if(*is_simultaneous == TRUE) break;
			}
		}
		else if( type_1 == 2 && type_2 == 1 ){
			for( i = 0 ; i < count_1 ; i = i + 2 ){
				for( j = 0 ; j < count_2 ; j++ ){
					if( ptime1_intern->data[i] <= ptime2_intern->data[j] && ptime1_intern->data[i+1] >= ptime2_intern->data[j] ){
						*is_simultaneous = TRUE;
						break;
					}
				}
				if(*is_simultaneous == TRUE) break;
			}
		}
		else if( type_1 == 1 && type_2 == 2 ){
			for( i = 0 ; i < count_1 ; i = i++ ){
				for( j = 0 ; j < count_2 ; j = j + 2 ){
					if( ptime2_intern->data[j] <= ptime1_intern->data[i] && ptime2_intern->data[j+1] >= ptime1_intern->data[i] ){
						*is_simultaneous = TRUE;
						break;
					}
				}
				if(*is_simultaneous == TRUE) break;
			}
		}
		else if( type_1 == 2 && type_2 == 2 ){
			for( i = 0 ; i < count_1 ; i = i + 2 ){
				for( j = 0 ; j < count_2 ; j = j + 2 ){
					if( (ptime2_intern->data[j] <= ptime1_intern->data[i] && ptime2_intern->data[j+1] >= ptime1_intern->data[i]) ||
							(ptime2_intern->data[j] <= ptime1_intern->data[i+1] && ptime2_intern->data[j+1] >= ptime1_intern->data[i]) ){
						*is_simultaneous = TRUE;
						break;
					}
				}
				if(*is_simultaneous == TRUE) break;
			}
		}
		if( ptime1->type > 4 ) { FREE_MEM(ptime1_intern); };
		if( ptime2->type > 4 ) { FREE_MEM(ptime2_intern); };
	}
	return err_ret;
}

/*! Check if two instances share a simultaneous period.
 * @param[in] ptime1
 * @param[in] ptime2
 * @param[out] overlaps
 * \return Makes use of the pt_error_type. */
pt_error_type overlaps(POSTTIME * ptime1 , POSTTIME * ptime2 , bool * overlaps){
	*overlaps = FALSE;
	pt_error_type err_ret = NO_ERROR;
	int32 i = 0 , j = 0 , count_1 = 0 , count_2 = 0;
	size_t size_needed = 0;
	POSTTIME * ptime1_intern;
	POSTTIME * ptime2_intern;

	if( ptime1->refsys.type == 3 || ptime2->refsys.type == 3 ){
		err_ret = FUNCTION_UNDEFINED_FOR_ORDINAL;
	}
	else {

		/* RegularMultiObjects into MultiObjects */
		if( ptime1->type > 4 ){
			get_rvalue(ptime1 , &i );
			size_needed = sizeof(POSTTIME) + ( sizeof(JULIAN_DAY) * (( i + 1 ) * ( ptime1->type - 4 ) - 1));
			ptime1_intern = palloc( size_needed );
			memset(ptime1_intern,'\0', size_needed );
			regular_multi_to_multi( ptime1, ptime1_intern );
			SET_VARSIZE( ptime1_intern, size_needed );
		}
		else ptime1_intern = ptime1;
		if( ptime2->type > 4 ){
			get_rvalue(ptime2 , &i );
			size_needed = sizeof(POSTTIME) + ( sizeof(JULIAN_DAY) * (( i + 1 ) * ( ptime2->type - 4 ) - 1));
			ptime2_intern = palloc( size_needed );
			memset(ptime2_intern,'\0', size_needed );
			regular_multi_to_multi( ptime2, ptime2_intern );
			SET_VARSIZE( ptime2_intern, size_needed );
		}
		else ptime2_intern = ptime2;

		count_1 = ( ( VARSIZE(ptime1) ) - sizeof(POSTTIME) )/sizeof(JULIAN_DAY) + 1;
		count_2 = ( ( VARSIZE(ptime2) ) - sizeof(POSTTIME) )/sizeof(JULIAN_DAY) + 1;

		if( (ptime1_intern->type == 4 || ptime1_intern->type == 2) && (ptime2_intern->type == 4 || ptime2_intern->type == 2) ){
			for( i = 0 ; i < count_1 ; i = i + 2 ){
				for( j = 0 ; j < count_2 ; j = j + 2 ){
					if( (ptime2_intern->data[j] <= ptime1_intern->data[i] && ptime2_intern->data[j+1] > ptime1_intern->data[i]) ||
							(ptime2_intern->data[j] < ptime1_intern->data[i+1] && ptime2_intern->data[j+1] >= ptime1_intern->data[i]) ){
						*overlaps = TRUE;
						break;
					}
				}
				if(*overlaps == TRUE) break;
			}
		}
		if( ptime1->type > 4 ) { FREE_MEM(ptime1_intern); };
		if( ptime2->type > 4 ) { FREE_MEM(ptime2_intern); };
	}
	return err_ret;
}
