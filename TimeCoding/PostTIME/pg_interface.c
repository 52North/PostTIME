/*
 * pg_interface.c
 * Purpose: Define the general macros needed as interface for the SQL declarations.
 */

#include "parser.h"
#include "textualizator.h"
#include "temporal_bbox.h"
#include "relative_position.h"
#include "distance_length.h"

// Function Declaration
Datum posttime_in(PG_FUNCTION_ARGS);
Datum posttime_out(PG_FUNCTION_ARGS);
Datum pt_transform_system(PG_FUNCTION_ARGS);
Datum pt_regular_multi_to_multi(PG_FUNCTION_ARGS);
Datum pt_temporal_bbox_instance(PG_FUNCTION_ARGS);
Datum pt_temporal_bbox_two_args(PG_FUNCTION_ARGS);
Datum tm_relative_position(PG_FUNCTION_ARGS);
Datum tm_relative_position_int(PG_FUNCTION_ARGS);
Datum tm_distance(PG_FUNCTION_ARGS);
Datum tm_distance_dec_day(PG_FUNCTION_ARGS);
Datum tm_duration(PG_FUNCTION_ARGS);
Datum tm_duration_dec_day(PG_FUNCTION_ARGS);
Datum pt_simultaneous(PG_FUNCTION_ARGS);
Datum pt_simultaneous_excluded_end_instants(PG_FUNCTION_ARGS);
Datum pt_overlaps(PG_FUNCTION_ARGS);
Datum pt_weekday_int(PG_FUNCTION_ARGS);
Datum pt_centroid(PG_FUNCTION_ARGS);
Datum pt_type(PG_FUNCTION_ARGS);
Datum pt_refsys_type(PG_FUNCTION_ARGS);
Datum pt_refsys_instance(PG_FUNCTION_ARGS);

#ifdef PG_MODULE_MAGIC
PG_MODULE_MAGIC; /*!< The mandatory PG_MODUL_MAGIC macro is defined here. */
#endif


void _PG_init(){
	SPI_connect();
	int32 ret = SPI_execute("SELECT extversion FROM pg_extension WHERE extname = 'postgis'" , TRUE , 1);

	if (ret > 0 && SPI_tuptable != NULL){
		char * str_out = SPI_getvalue( SPI_tuptable->vals[0] , SPI_tuptable->tupdesc , 1 );

		memset(postgis_sharedlib, '\0', sizeof(char)*( 17 ));
		strncpy(postgis_sharedlib, "postgis-", 9);
		strncpy(&postgis_sharedlib[8] , str_out, 3);
		strncpy(&postgis_sharedlib[11] , ".so", 3);
	}
	else {
		memset(postgis_sharedlib, '\0', sizeof(char)*( 16 ));
		strncpy(postgis_sharedlib, "postgis-", 9);
		strncpy(&postgis_sharedlib[8]  , "1.5", 3);
		strncpy(&postgis_sharedlib[11] , ".so", 3);
	}
	void * * pgis_filehandle = (void *) 0;
	pgis_relate_pattern = load_external_function(postgis_sharedlib,"relate_pattern", 1 ,pgis_filehandle);
	SPI_finish();
}

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
    FREE_MEM(str_out);
    PG_RETURN_CSTRING(str_final);
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
	if( ptime_ret->refsys.type == 3 ) err = FUNCTION_UNDEFINED_FOR_ORDINAL;
	if(*(dummy[0]) != 0 && err == NO_ERROR) err = SPECIFY_ONLY_THE_NEW_SYSTEM;

	if( err != NO_ERROR ){
		//FREE_MEM(ptime_ret);
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[err].msg)));
		PG_RETURN_NULL();
	}
	memcpy((void *) ( VARDATA(ptime_ret) + sizeof(REFSYS) ),
			(void *) ( VARDATA(ptime) + sizeof(REFSYS) ),
			VARSIZE(ptime) - VARHDRSZ -sizeof(REFSYS) );
	SET_VARSIZE( ptime_ret, VARSIZE(ptime) );

	if( ptime->type > 4 && ( ptime->refsys.type != ptime_ret->refsys.type ) ){
		int32 data_count_needed = 0;
		if( ptime_ret->refsys.type == 1 ){
			if( ptime->type == 5 ) data_count_needed = 5;
			else data_count_needed = 9;
		}
		else if( ptime_ret->refsys.type == 2 ){
			if( ptime->type == 5 ) data_count_needed = 3;
			else data_count_needed = 4;
		}
		size_t size_needed = sizeof(POSTTIME) + sizeof(JULIAN_DAY) * (data_count_needed - 1);
		POSTTIME * ptime_ret_reg = (POSTTIME *) palloc(size_needed);
		memset( ptime_ret_reg, 0 ,size_needed );
		transform_regular_system( ptime , &ptime_ret->refsys , ptime_ret_reg );
		FREE_MEM(ptime_ret);
		SET_VARSIZE(ptime_ret_reg , size_needed);
		PG_RETURN_POINTER(ptime_ret_reg);
	}
	else PG_RETURN_POINTER(ptime_ret);
}

PG_FUNCTION_INFO_V1(pt_type);
Datum
pt_type(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);
	int32 int_ret;
	int_ret = (int32) ptime->type;
	PG_RETURN_INT32(int_ret);
}

PG_FUNCTION_INFO_V1(pt_refsys_type);
Datum
pt_refsys_type(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);
	int32 int_ret;
	int_ret = (int32) ptime->refsys.type;
	PG_RETURN_INT32(int_ret);
}

PG_FUNCTION_INFO_V1(pt_refsys_instance);
Datum
pt_refsys_instance(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);
	int32 int_ret;
	int_ret = (int32) ptime->refsys.instance;
	PG_RETURN_INT32(int_ret);
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

PG_FUNCTION_INFO_V1(pt_temporal_bbox_instance);
Datum
pt_temporal_bbox_instance(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_bbox = palloc(sizeof(POSTTIME) + sizeof(JULIAN_DAY));
	memset(ptime_bbox,'\0', sizeof(POSTTIME) + sizeof(JULIAN_DAY) );
	
	temporal_bbox_single_instance( ptime, ptime_bbox );
	
	PG_RETURN_POINTER(ptime_bbox);
}

PG_FUNCTION_INFO_V1(pt_temporal_bbox_two_args);
Datum
pt_temporal_bbox_two_args(PG_FUNCTION_ARGS){
	POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);

	POSTTIME * ptime_bbox = palloc(sizeof(POSTTIME) + sizeof(JULIAN_DAY));
	memset(ptime_bbox,'\0', sizeof(POSTTIME) + sizeof(JULIAN_DAY) );

	pt_error_type ret_err = temporal_bbox_single_two_instances( ptime_1 , ptime_2 , ptime_bbox );
	if( ret_err != NO_ERROR ){
		FREE_MEM(ptime_bbox);
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}

	PG_RETURN_POINTER(ptime_bbox);
}

PG_FUNCTION_INFO_V1(tm_relative_position);
Datum
tm_relative_position(PG_FUNCTION_ARGS){
	POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);

	char * str_out = palloc(sizeof(char) * 20);
	memset(str_out, '\0', sizeof(char) * 20);

	pt_error_type ret_err = relative_position_str( ptime_1 , ptime_2 , str_out );
	if( ret_err != NO_ERROR ){
		FREE_MEM(str_out);
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}

	int32 str_length = 0;
	str_length = strlen(str_out);

    char * str_final = palloc(sizeof(char)*(str_length+1));
    memset(str_final, '\0', sizeof(char)*(str_length+1));
    strncpy(str_final, str_out, str_length);
    FREE_MEM(str_out);
    PG_RETURN_CSTRING(str_final);
}

PG_FUNCTION_INFO_V1(tm_relative_position_int);
Datum
tm_relative_position_int(PG_FUNCTION_ARGS){
	POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
	int32 int_ret = 0;

	pt_error_type ret_err = relative_position_int( ptime_1 , ptime_2 , &int_ret );
	if( ret_err != NO_ERROR ){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}

    PG_RETURN_INT32(int_ret);
}

//simple ordering functions

PG_FUNCTION_INFO_V1(tm_after);
Datum
tm_after(PG_FUNCTION_ARGS) {
  POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
	int32 int_ret = 0;

	pt_error_type ret_err = relative_position_int( ptime_1 , ptime_2 , &int_ret );
	PG_RETURN_BOOL(int_ret == 12);
}


PG_FUNCTION_INFO_V1(tm_before);
Datum
tm_before(PG_FUNCTION_ARGS) {
  POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
	int32 int_ret = 0;

	pt_error_type ret_err = relative_position_int( ptime_1 , ptime_2 , &int_ret );
	PG_RETURN_BOOL(int_ret == 0);
}

PG_FUNCTION_INFO_V1(tm_equal);
Datum
tm_equal(PG_FUNCTION_ARGS) {
  POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
  POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
  int32 int_ret = 0;

  pt_error_type ret_err = relative_position_int( ptime_1 , ptime_2 , &int_ret );
  PG_RETURN_BOOL(int_ret == 7);
}

PG_FUNCTION_INFO_V1(tm_gtequal);
Datum
tm_gtequal(PG_FUNCTION_ARGS) {
  POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
  POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
  int32 int_ret = 0;

  pt_error_type ret_err = relative_position_int( ptime_1 , ptime_2 , &int_ret );
  PG_RETURN_BOOL((int_ret == 7) || (int_ret == 12));
}

PG_FUNCTION_INFO_V1(tm_ltequal);
Datum
tm_ltequal(PG_FUNCTION_ARGS) {
  POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
  POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
  int32 int_ret = 0;

  pt_error_type ret_err = relative_position_int( ptime_1 , ptime_2 , &int_ret );
  PG_RETURN_BOOL((int_ret == 7) || (int_ret == 0));
}


PG_FUNCTION_INFO_V1(tm_helpfunc);
Datum
tm_helpfunc(PG_FUNCTION_ARGS) {
  POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
  POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
  int32 int_ret = 0;

  pt_error_type ret_err = relative_position_int( ptime_1 , ptime_2 , &int_ret );
  int32 btree_ret = 3;
  switch (int_ret) {
  case 0:
      btree_ret=-1;
      break;
  case 7:
      btree_ret= 0;
      break;
  case 12:
      btree_ret = 1;
      break;
  }
  PG_RETURN_INT32(btree_ret);
}




//end simple ordering functions

PG_FUNCTION_INFO_V1(tm_distance);
Datum
tm_distance(PG_FUNCTION_ARGS){
	POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);

	char * str_out = palloc(sizeof(char) * 80);
	memset(str_out, '\0', sizeof(char) * 80);

	pt_error_type ret_err = NO_ERROR;
	if( ptime_1->refsys.type == 3 || ptime_2->refsys.type == 3 ){
		ret_err = ISO19108_NOT_FOR_ORDINAL_SYSTEMS;
	}
	else ret_err = distance_string( ptime_1 , ptime_2 , str_out );

	if( ret_err != NO_ERROR ){
		FREE_MEM(str_out);
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}

	int32 str_length = 0;
	str_length = strlen(str_out);

    char * str_final = palloc(sizeof(char)*(str_length+1));
    memset(str_final, '\0', sizeof(char)*(str_length+1));
    strncpy(str_final, str_out, str_length);

	FREE_MEM(str_out);
    PG_RETURN_CSTRING(str_final);
}

PG_FUNCTION_INFO_V1(tm_distance_dec_day);
Datum
tm_distance_dec_day(PG_FUNCTION_ARGS){
	POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
	float8 float_ret = 0;

	pt_error_type ret_err = NO_ERROR;
	if( ptime_1->refsys.type == 3 || ptime_2->refsys.type == 3 ){
		ret_err = ISO19108_NOT_FOR_ORDINAL_SYSTEMS;
	}
	else ret_err = distance_jul_day( ptime_1 , ptime_2 , &float_ret );

	if( ret_err != NO_ERROR ){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}

    PG_RETURN_FLOAT8(float_ret);
}

PG_FUNCTION_INFO_V1(tm_duration);
Datum
tm_duration(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);

	char * str_out = palloc(sizeof(char) * 80);
	memset(str_out, '\0', sizeof(char) * 80);

	pt_error_type ret_err = NO_ERROR;
	if( ptime->refsys.type == 3 ){
		ret_err = ISO19108_NOT_FOR_ORDINAL_SYSTEMS;
	}
	else ret_err = duration_string( ptime , str_out );

	if( ret_err != NO_ERROR ){
		FREE_MEM(str_out);
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}

	int32 str_length = 0;
	str_length = strlen(str_out);

    char * str_final = palloc(sizeof(char)*(str_length+1));
    memset(str_final, '\0', sizeof(char)*(str_length+1));
    strncpy(str_final, str_out, str_length);

	FREE_MEM(str_out);
    PG_RETURN_CSTRING(str_final);
}

PG_FUNCTION_INFO_V1(tm_duration_dec_day);
Datum
tm_duration_dec_day(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);
	float8 float_ret = 0;

	pt_error_type ret_err = NO_ERROR;
	if( ptime->refsys.type == 3 ){
		ret_err = ISO19108_NOT_FOR_ORDINAL_SYSTEMS;
	}
	else ret_err = duration_jul_day( ptime , &float_ret );

	if( ret_err != NO_ERROR ){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}

    PG_RETURN_FLOAT8(float_ret);
}

PG_FUNCTION_INFO_V1(pt_centroid);
Datum
pt_centroid(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_centroid = palloc(sizeof(POSTTIME));
	memset(ptime_centroid,'\0', sizeof(POSTTIME));
	pt_error_type ret_err = NO_ERROR;

	ret_err = centroid( ptime, ptime_centroid );
	if( ret_err != NO_ERROR ){
		FREE_MEM(ptime_centroid);
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}
	PG_RETURN_POINTER(ptime_centroid);
}

PG_FUNCTION_INFO_V1(pt_simultaneous);
Datum
pt_simultaneous(PG_FUNCTION_ARGS){
	POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
	bool ret = FALSE;
	pt_error_type ret_err = NO_ERROR;
	ret_err = simultaneous( ptime_1 , ptime_2 , &ret );
	if( ret_err != NO_ERROR ){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}

    PG_RETURN_BOOL( ret );
}

PG_FUNCTION_INFO_V1(pt_simultaneous_excluded_end_instants);
Datum
pt_simultaneous_excluded_end_instants(PG_FUNCTION_ARGS){
	POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
	bool ret = FALSE;
	pt_error_type ret_err = NO_ERROR;
	ret_err = simultaneous_excluded_end_instants( ptime_1 , ptime_2 , &ret );
	if( ret_err != NO_ERROR ){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}

    PG_RETURN_BOOL( ret );
}

PG_FUNCTION_INFO_V1(pt_overlaps);
Datum
pt_overlaps(PG_FUNCTION_ARGS){
	POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(0);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(1);
	bool ret = FALSE;
	pt_error_type ret_err = NO_ERROR;
	ret_err = overlaps( ptime_1 , ptime_2 , &ret );
	if( ret_err != NO_ERROR ){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}

    PG_RETURN_BOOL( ret );
}

PG_FUNCTION_INFO_V1(pt_weekday_int);
Datum
pt_weekday_int(PG_FUNCTION_ARGS){
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);
	int32 weekday;
	pt_error_type ret_err = NO_ERROR;

	if(ptime->type != 1) ret_err = ONLY_INSTANTS_FUNCTIONS;
	else if(ptime->refsys.type == 3) ret_err = FUNCTION_UNDEFINED_FOR_ORDINAL;
	if( ret_err != NO_ERROR ){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[ret_err].msg)));
		PG_RETURN_NULL();
	}
	else weekday = ( (ptime->data[0] + MILLIS / 2) / (int64) MILLIS ) % 7;

    PG_RETURN_INT32( weekday );
}


