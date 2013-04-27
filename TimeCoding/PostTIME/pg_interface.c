/*
 * pg_interface.c
 * Purpose: Define the general macros needed as interface for the SQL declarations.
 */

#include "parser.h"
#include "textualizator.h"

// Function Declaration
Datum posttime_in(PG_FUNCTION_ARGS);
Datum posttime_out(PG_FUNCTION_ARGS);
Datum pt_transform_system(PG_FUNCTION_ARGS);
Datum pt_regular_multi_to_multi(PG_FUNCTION_ARGS);

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC; /*!< The mandatory PG_MODUL_MAGIC macro is defined here. */
#endif

PG_FUNCTION_INFO_V1(posttime_in);
/*! The type's constructor. Has to extract all information from a given
 * string and has to verify syntactical and semantical correctness.
 * \return An instance of posttime. */
Datum
posttime_in(PG_FUNCTION_ARGS){
	char * str_in = PG_GETARG_CSTRING(0);

	size_t size_ptime = sizeof(POSTTIME);
	size_t size_jul = sizeof(JULIAN_DAY);
	// Determine count of (potential) primitives.
	int32 int_count = 0;
	int_count = count_of_primitives(str_in);
	size_t size_max_needed = 0;
	if(int_count == 1) size_max_needed = size_ptime + (size_jul * 8);
	else size_max_needed = size_ptime + (size_jul * ( (2 * int_count)-1 ));
	// Allocate memory for the new instance.
	POSTTIME * ptime_tmp = palloc(size_max_needed);
	memset(ptime_tmp,'\0', size_max_needed );

	pt_error_type ret_err = string_to_ptime(str_in, &int_count, ptime_tmp);

	if( ret_err != NO_ERROR ) {
		FREE_MEM(ptime_tmp);
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	};

	// Set count to finally needed count of data elements.
	if ( (ptime_tmp->type % 2) == 0 ) int_count = int_count * 2;
	if ( ptime_tmp->type == 5) int_count = 5;
	else if ( ptime_tmp->type == 6) int_count = 9;
	size_t size_fin = size_ptime + (size_jul * ( int_count-1 ));

	POSTTIME * ptime_fin = palloc( size_fin );
	memset(ptime_fin, 0 ,sizeof(POSTTIME));
	memcpy((void *) VARDATA(ptime_fin),
			(void *) VARDATA(ptime_tmp),
			size_fin - VARHDRSZ );
	SET_VARSIZE( ptime_fin, size_fin );

	// Clear and finish.
	FREE_MEM(ptime_tmp);

	PG_RETURN_POINTER(ptime_fin);
}

PG_FUNCTION_INFO_V1(posttime_out);
Datum
posttime_out(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);

	int32 count = ( ( VARSIZE(ptime) ) - sizeof(POSTTIME) )/sizeof(JULIAN_DAY) + 1;

	char * str_out = palloc(sizeof(char) * count * 50);
	memset(str_out, '\0', sizeof(char) * count * 50);

	ptime_to_string( ptime , str_out , &count );

	int32 str_length = 0;
	str_length = strlen(str_out);

    char * str_final = palloc(sizeof(char)*(str_length+1));
    memset(str_final, '\0', sizeof(char)*(str_length+1));
    strncpy(str_final, str_out, str_length);

    PG_RETURN_CSTRING(str_out);
}

PG_FUNCTION_INFO_V1(pt_transform_system);
Datum
pt_transform_system(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);
	char * str_in = PG_GETARG_CSTRING(1);
	char * char_tmp = "P";
	char * * dummy = &char_tmp;
	POSTTIME * ptime_ret = (POSTTIME *) palloc(VARSIZE(ptime));
	memset( ptime_ret, 0 ,VARSIZE(ptime) );

	pt_error_type err = determine_refsys( str_in , dummy , ptime_ret );
	if( ptime->refsys.type == 3 ) err = FUNCTION_UNDEFINED_FOR_ORDINAL;
	if(*(dummy[0]) != 0 && err == NO_ERROR) err = SPECIFY_ONLY_THE_NEW_SYSTEM;

	if( err != NO_ERROR ){
		FREE_MEM(ptime_ret);
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[err].msg)));
		PG_RETURN_NULL();
	}
	memcpy((void *) ( VARDATA(ptime_ret) + sizeof(REFSYS) ),
			(void *) ( VARDATA(ptime) + sizeof(REFSYS) ),
			VARSIZE(ptime) - VARHDRSZ -sizeof(REFSYS) );
	SET_VARSIZE( ptime_ret, VARSIZE(ptime) );

	PG_RETURN_POINTER(ptime_ret);
}

PG_FUNCTION_INFO_V1(pt_regular_multi_to_multi);
Datum
pt_regular_multi_to_multi(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);
	int32 rvalue = 0;
	POSTTIME * ptime_ret;
	pt_error_type ret_err = get_rvalue( ptime , &rvalue );
	if( ret_err == NO_ERROR ){
		size_t size_needed = sizeof(POSTTIME) + ( sizeof(JULIAN_DAY) * (( rvalue + 1 ) * ( ptime->type - 4 ) - 1));
		ptime_ret = palloc( size_needed );
		memset(ptime_ret,'\0', size_needed );
		regular_multi_to_multi( ptime, ptime_ret );
		SET_VARSIZE( ptime_ret, size_needed );
	}
	else {
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}
	PG_RETURN_POINTER(ptime_ret);
}


