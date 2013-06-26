/*
 * relative_position.c
 *
 *  Created on: 05.05.2013
 *      Author: ptr
 */

#include "relative_position.h"


/*!ISO19108 relative position subroutine - case instant to instant.
 * \return rel_pos 0\13 Allen's relationships + Not a primitive. */
rel_pos relative_position_ii(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[0] < ptime_2->data[0])return BEFORE;
	else if(ptime_1->data[0] == ptime_2->data[0])return EQUALS;
	else if(ptime_1->data[0] > ptime_2->data[0])return AFTER;
	else return NAP;
}

/*!ISO19108 relative position subroutine - case period to instant.
 * \return rel_pos 0\13 Allen's relationships + Not a primitive. */
rel_pos relative_position_pi(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[1] < ptime_2->data[0])return BEFORE;
	else if(ptime_1->data[1] == ptime_2->data[0])return ENDED_BY;
	else if((ptime_1->data[0] < ptime_2->data[0]) && (ptime_1->data[1] > ptime_2->data[0]))return CONTAINS;
	else if(ptime_1->data[0] == ptime_2->data[0])return BEGUN_BY;
	else if(ptime_1->data[0] > ptime_2->data[0])return AFTER;
	else return NAP;
}

/*!ISO19108 relative position subroutine - case instant to period.
 * \return rel_pos 0\13 Allen's relationships + Not a primitive. */
rel_pos relative_position_ip(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[0] < ptime_2->data[0])return BEFORE;
	else if(ptime_1->data[0] == ptime_2->data[0])return BEGINS;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[0] < ptime_2->data[1])return DURING;
	else if(ptime_1->data[0] == ptime_2->data[1])return ENDS;
	else if(ptime_1->data[0] > ptime_2->data[1])return AFTER;
	else return NAP;
}

/*!ISO19108 relative position subroutine - case period to period.
 * \return rel_pos 0\13 Allen's relationships + Not a primitive. */
rel_pos relative_position_pp(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[1] < ptime_2->data[0]) return BEFORE;
	else if(ptime_1->data[1] == ptime_2->data[0]) return MEETS;
	else if(ptime_1->data[0] < ptime_2->data[0] && ptime_1->data[1] > ptime_2->data[0] && ptime_1->data[1] < ptime_2->data[1])return OVERLAPS;
	else if(ptime_1->data[0] == ptime_2->data[0] && ptime_1->data[1] < ptime_2->data[1])return BEGINS;
	else if(ptime_1->data[0] == ptime_2->data[0] && ptime_1->data[1] > ptime_2->data[1])return BEGUN_BY;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[1] < ptime_2->data[1])return DURING;
	else if(ptime_1->data[0] < ptime_2->data[0] && ptime_1->data[1] > ptime_2->data[1])return CONTAINS;
	else if(ptime_1->data[0] == ptime_2->data[0] && ptime_1->data[1] == ptime_2->data[1])return EQUALS;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[0] < ptime_2->data[1] && ptime_1->data[1] > ptime_2->data[1])return OVERLAPPED_BY;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[1] == ptime_2->data[1])return ENDS;
	else if(ptime_1->data[0] < ptime_2->data[0] && ptime_1->data[1] == ptime_2->data[1])return ENDED_BY;
	else if(ptime_1->data[0] == ptime_2->data[1])return MET_BY;
	else if(ptime_1->data[0] > ptime_2->data[1])return AFTER;
	else return NAP;
}

/*!ISO19108 relative position implementation.
 * \return rel_pos 0\12 Allen's relationships or 13= Not a primitive. */
void relative_position_rp(POSTTIME * ptime_1,POSTTIME * ptime_2, rel_pos * ret_rp){
	if( (ptime_1->type > 2) || (ptime_2->type > 2) ){
		*ret_rp = NAP;
	}
	else if((ptime_1->type == 1) && (ptime_2->type == 1)){
		*ret_rp = relative_position_ii(ptime_1,ptime_2);
	}
	else if((ptime_1->type == 2) && (ptime_2->type == 1)){
		*ret_rp = relative_position_pi(ptime_1,ptime_2);
	}
	else if((ptime_1->type == 1) && (ptime_2->type == 2)){
		*ret_rp = relative_position_ip(ptime_1,ptime_2);
	}
	else if((ptime_1->type == 2) && (ptime_2->type == 2)){
		*ret_rp = relative_position_pp(ptime_1,ptime_2);
	}
}

/*!ISO19108 relative position implementation.
 * \return TM_RelativePosition enumeration type. */
void relative_position_str(POSTTIME * ptime_1 , POSTTIME * ptime_2 , char * str){
	static const char* strings[] = { "Before", "Meets", "Overlaps", "Begins", "BegunBy", "During", "Contains", \
		"Equals", "OverlappedBy", "Ends", "EndedBy", "MetBy", "After", "NAP"};
	rel_pos rp = NAP;
	relative_position_rp(ptime_1 , ptime_2 , &rp);
	strcpy(str, strings[rp]);
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
			for( i = 0 ; i < count_1 ; i++ ){
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

/*! Check if two instances share a simultaneous instant.
 * @param[in] ptime1
 * @param[in] ptime2
 * @param[out] is_simultaneous
 * \return Makes use of the pt_error_type. */
pt_error_type simultaneous_excluded_end_instants(POSTTIME * ptime1 , POSTTIME * ptime2 , bool * is_simultaneous){
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
					if( ptime1_intern->data[i] <= ptime2_intern->data[j] && ptime1_intern->data[i+1] > ptime2_intern->data[j] ){
						*is_simultaneous = TRUE;
						break;
					}
				}
				if(*is_simultaneous == TRUE) break;
			}
		}
		else if( type_1 == 1 && type_2 == 2 ){
			for( i = 0 ; i < count_1 ; i++ ){
				for( j = 0 ; j < count_2 ; j = j + 2 ){
					if( ptime2_intern->data[j] <= ptime1_intern->data[i] && ptime2_intern->data[j+1] > ptime1_intern->data[i] ){
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
					if( (ptime2_intern->data[j] <= ptime1_intern->data[i] && ptime2_intern->data[j+1] > ptime1_intern->data[i]) ||
							(ptime2_intern->data[j] < ptime1_intern->data[i+1] && ptime2_intern->data[j+1] > ptime1_intern->data[i]) ){
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
