/*
 * basic_statistics.c
 *
 *      Author: ptr
 */

#include "basic_statistics.h"

Datum pt_histogram_intervals(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(pt_histogram_intervals);
Datum
pt_histogram_intervals(PG_FUNCTION_ARGS){
	FuncCallContext  *funcctx;
	normal_dn_histogram_fctx * fctx;

    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext oldcontext;

        funcctx = SRF_FIRSTCALL_INIT();

        fctx = (normal_dn_histogram_fctx *) palloc(sizeof(normal_dn_histogram_fctx) + sizeof(JULIAN_DAY));

         memset( fctx , 0 , sizeof(normal_dn_histogram_fctx) + sizeof(JULIAN_DAY) );

        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);
        /* One-time Setup fctx, calculate first 'next' */
        POSTTIME * ptime_bbox_in = (POSTTIME *) PG_GETARG_POINTER(0);
        DATE_NUMBERS dn_first;

        // Get duration.
        pt_error_type ret_err = parse_single_duration_string( PG_GETARG_CSTRING(1) , &fctx->period );
        if( ret_err != NO_ERROR ){
        	FREE_MEM(fctx);
    		ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),
    				errmsg("%s",pt_error_msgs[ret_err].msg)));
    		PG_RETURN_NULL();
        }
        // End instant for this function.
        fctx->end_instant = ptime_bbox_in->data[1];

        // Find beginning.
        fctx->cal_system = get_system_from_key( ptime_bbox_in->refsys.instance );
        dn_first = fctx->cal_system->jday_to_dnumbers( &(ptime_bbox_in->data[0]) );
        if(fctx->period.mon == 0) dn_first.mon = 1;
        if(fctx->period.day == 0) dn_first.day = 1;
        if(fctx->period.hou == 0) dn_first.hou = 0;
        if(fctx->period.min == 0) dn_first.min = 0;
        if(fctx->period.sec == 0) dn_first.sec = 0;

        fctx->next_ptime.type = 2;
        fctx->next_ptime.refsys.instance = ptime_bbox_in->refsys.instance;
        fctx->next_ptime.refsys.type = 1;
        fctx->next_ptime.data[0] = fctx->cal_system->dnumber_to_jday( &dn_first );
        dn_first = dn_plus_dn( &dn_first , &fctx->period , fctx->cal_system );
        fctx->next_ptime.data[1] = fctx->cal_system->dnumber_to_jday( &dn_first );
        fctx->next_dn = dn_first;

        SET_VARSIZE( &fctx->next_ptime , sizeof(POSTTIME) + sizeof(JULIAN_DAY));

        funcctx->user_fctx = fctx;

        MemoryContextSwitchTo(oldcontext);
    }
    /* Each-time setup code appears here: */

    funcctx = SRF_PERCALL_SETUP();

    fctx = funcctx->user_fctx;

    /* this is just one way we might test whether we are done: */
    if ( fctx->next_ptime.data[0] < fctx->end_instant)
    {
    	POSTTIME * ptime_return = palloc(sizeof(POSTTIME) + sizeof(JULIAN_DAY));
    	memcpy( ptime_return , &fctx->next_ptime , sizeof(POSTTIME) + sizeof(JULIAN_DAY) );
    	SET_VARSIZE(ptime_return , sizeof(POSTTIME) + sizeof(JULIAN_DAY));

    	fctx->next_ptime.data[0] = fctx->next_ptime.data[1];

    	fctx->next_dn = dn_plus_dn( &fctx->next_dn , &fctx->period , fctx->cal_system );

    	fctx->next_ptime.data[1] = fctx->cal_system->dnumber_to_jday( &fctx->next_dn );

    	SET_VARSIZE(&fctx->next_ptime , sizeof(POSTTIME) + sizeof(JULIAN_DAY));

		SRF_RETURN_NEXT(funcctx, ptime_return);
    }
    else
    {
        /* Here we are done returning items and just need to clean up: */
        SRF_RETURN_DONE(funcctx);
    }

}
