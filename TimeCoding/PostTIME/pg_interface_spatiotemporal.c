/*
 * pg_interface_spatiotemporal.c
 *
 *      Author: ptr
 */

#include "relative_position.h"

Datum pt_predecessor(PG_FUNCTION_ARGS);
Datum pt_successor(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pt_predecessor);
Datum
pt_predecessor(PG_FUNCTION_ARGS){
	Datum geom_1 = PG_GETARG_DATUM(0);
	POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(1);
	Datum geom_2 = PG_GETARG_DATUM(2);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(3);

	Datum datum_ret = 0;
	void * * pgis_filehandle = (void *) 0;
	bool overlaps = false;
	bool ptime_ret = false;
	int32 ret_relativepos = 0;

	if( ptime_1->type != 2 && ptime_2->type != 2 ){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("Successor and predecessor are expecting POSTTIME periods.")));
		PG_RETURN_NULL();
	}
	else {
		relative_position_int( ptime_1 , ptime_2 , &ret_relativepos);
		if( ret_relativepos == 11 ){
			text * my_pattern = palloc(sizeof(char) * 14);
			memcpy( my_pattern->vl_dat , "T********" , 10 );
			SET_VARSIZE( my_pattern , sizeof(char) * 14);

			PGFunction pgis_overlaps = load_external_function(POSTGIS_SHLIB_NAME,"relate_pattern", 1 ,pgis_filehandle);
			datum_ret = DirectFunctionCall3Coll(pgis_overlaps, 2, geom_1 , geom_2, my_pattern);
			overlaps = DatumGetBool(datum_ret);

			ptime_ret = overlaps;
			FREE_MEM( my_pattern );
		}
	}
	PG_RETURN_BOOL(ptime_ret);
}

PG_FUNCTION_INFO_V1(pt_successor);
Datum
pt_successor(PG_FUNCTION_ARGS){
	Datum geom_1 = PG_GETARG_DATUM(0);
	POSTTIME * ptime_1 = (POSTTIME *) PG_GETARG_POINTER(1);
	Datum geom_2 = PG_GETARG_DATUM(2);
	POSTTIME * ptime_2 = (POSTTIME *) PG_GETARG_POINTER(3);

	Datum datum_ret = 0;
	void * * pgis_filehandle = (void *) 0;
	bool overlaps = false;
	bool ptime_ret = false;
	int32 ret_relativepos = 0;

	if( ptime_1->type != 2 && ptime_2->type != 2 ){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("Successor and predecessor are expecting POSTTIME periods.")));
		PG_RETURN_NULL();
	}
	else {
		relative_position_int( ptime_1 , ptime_2 , &ret_relativepos);
		if( ret_relativepos == 1 ){
			text * my_pattern = palloc(sizeof(char) * 14);
			memcpy( my_pattern->vl_dat , "T********" , 10 );
			SET_VARSIZE( my_pattern , sizeof(char) * 14);

			PGFunction pgis_overlaps = load_external_function(POSTGIS_SHLIB_NAME,"relate_pattern", 1 ,pgis_filehandle);
			datum_ret = DirectFunctionCall3Coll(pgis_overlaps, 2, geom_1 , geom_2, my_pattern);
			overlaps = DatumGetBool(datum_ret);

			ptime_ret = overlaps;
			FREE_MEM( my_pattern );
		}
	}
	PG_RETURN_BOOL(ptime_ret);
}
