/*
 * pt_interface_casts.c
 *      Author: ptr
 */

#include "pg_interface_casts.h"

Datum pt_cast_from_timestamp(PG_FUNCTION_ARGS);
Datum pt_cast_to_timestamp(PG_FUNCTION_ARGS);
Datum pt_cast_from_date(PG_FUNCTION_ARGS);
Datum pt_cast_to_date(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pt_cast_from_timestamp);
Datum
pt_cast_from_timestamp(PG_FUNCTION_ARGS)
{
	Timestamp tstamp = PG_GETARG_TIMESTAMP(0);
	POSTTIME * ptime = (POSTTIME *) palloc(sizeof(POSTTIME));
	memset(ptime, '\0', sizeof(POSTTIME));
	ptime->type = 1;
	ptime->refsys.type = 1;
	ptime->refsys.instance = 1;
	ptime->data[0] = ( tstamp / (int64) 1000 ) + ( (int64) 2451545 * (int64) MILLIS ) - MILLIS / 2;
	SET_VARSIZE( ptime , sizeof(POSTTIME) );
	PG_RETURN_POINTER(ptime);
}

PG_FUNCTION_INFO_V1(pt_cast_to_timestamp);
Datum
pt_cast_to_timestamp(PG_FUNCTION_ARGS)
{
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);

	if(ptime->refsys.type == 3){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[FUNCTION_UNDEFINED_FOR_ORDINAL].msg)));
		PG_RETURN_NULL();
	}
	if(ptime->type != 1){
			ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					errmsg("%s",pt_error_msgs[CAN_ONLY_CAST_PTIME_INSTANTS].msg)));
			PG_RETURN_NULL();
	}

	Timestamp tstamp;
	tstamp = (ptime->data[0] - ( (int64) 2451545 * (int64) MILLIS ) + (int64) MILLIS / 2) * (int64) 1000;
	PG_RETURN_TIMESTAMP(tstamp);
}

PG_FUNCTION_INFO_V1(pt_cast_from_date);
Datum
pt_cast_from_date(PG_FUNCTION_ARGS)
{
	DateADT date = PG_GETARG_DATEADT(0);
	POSTTIME * ptime = (POSTTIME *) palloc(sizeof(POSTTIME));
	memset(ptime, '\0', sizeof(POSTTIME));
	ptime->type = 2;
	ptime->refsys.type = 1;
	ptime->refsys.instance = 1;
	ptime->data[0] = ( ( (int64) date + (int64) 2451545 ) * (int64) MILLIS ) - MILLIS / 2;
	ptime->data[1] = ( ( (int64) date + (int64) 2451545 ) * (int64) MILLIS ) + MILLIS / 2;
	SET_VARSIZE( ptime , sizeof(POSTTIME) + sizeof(JULIAN_DAY) );
	PG_RETURN_POINTER(ptime);
}

PG_FUNCTION_INFO_V1(pt_cast_to_date);
Datum
pt_cast_to_date(PG_FUNCTION_ARGS)
{
	POSTTIME * ptime = (POSTTIME *) PG_GETARG_POINTER(0);
	DateADT date;

	if(ptime->refsys.type == 3){
		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
				errmsg("%s",pt_error_msgs[FUNCTION_UNDEFINED_FOR_ORDINAL].msg)));
		PG_RETURN_NULL();
	}
	if(ptime->type > 2){
			ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					errmsg("%s",pt_error_msgs[CAN_ONLY_CAST_PTIME_INSTANTS].msg)));
			PG_RETURN_NULL();
	}
	else if(ptime->type == 2){
		int32 data_1 = (ptime->data[0] + MILLIS / 2) / (int64) MILLIS;
		int32 data_2 = (ptime->data[1] - 1 + MILLIS / 2) / (int64) MILLIS;
		if( data_1 == data_2 )
			date = data_1 - 2451545;
		else{
			ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
					errmsg("%s",pt_error_msgs[CAN_ONLY_CAST_PERIOD_IF_SAME_DAY].msg)));
			PG_RETURN_NULL();
		}
	}
	else if(ptime->type == 1){
		date = ( ( ptime->data[0] + MILLIS / 2 ) / (int64) MILLIS ) - (int64) 2451545;
	}
	PG_RETURN_DATEADT(date);
}
