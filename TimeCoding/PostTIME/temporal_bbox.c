/*
 * temporal_bbox.c
 *
 *  Created on: 28.04.2013
 *      Author: ptr
 */

#include "temporal_bbox.h"

void temporal_bbox_single_instance(POSTTIME * ptime, POSTTIME * bbox){
	POSTTIME * ptime_tmp;
	int32 rvalue, count, i;
	if( ptime->type > 4 ) {
		get_rvalue( ptime , &rvalue );
		size_t size_needed = sizeof(POSTTIME) + ( sizeof(JULIAN_DAY) * (( rvalue + 1 ) * ( ptime->type - 4 ) - 1));
		ptime_tmp = palloc( size_needed );
		memset(ptime_tmp,'\0', size_needed );
		regular_multi_to_multi( ptime, ptime_tmp );
		SET_VARSIZE( ptime_tmp, size_needed );
	}
	else ptime_tmp = ptime;
	bbox->refsys.type = ptime_tmp->refsys.type;
	bbox->refsys.instance = ptime_tmp->refsys.instance;
	bbox->type = 2;
	count = ( ( VARSIZE(ptime_tmp) ) - sizeof(POSTTIME) )/sizeof(JULIAN_DAY) + 1;
	bbox->data[0] = ptime_tmp->data[0];
	bbox->data[1] = ptime_tmp->data[0];
	for( i = 1 ; i < count ; i++ ){
		if( ptime_tmp->data[i] < bbox->data[0] ) bbox->data[0] = ptime_tmp->data[i];
		else if( ptime_tmp->data[i] > bbox->data[1] ) bbox->data[1] = ptime_tmp->data[i];
	}
	if(ptime->type > 4 ) FREE_MEM(ptime_tmp);
	SET_VARSIZE( bbox , sizeof(POSTTIME) + sizeof(JULIAN_DAY));
}

pt_error_type temporal_bbox_single_two_instances( POSTTIME * ptime_1 , POSTTIME * ptime_2 , POSTTIME * bbox ){
	if(ptime_1->refsys.type != ptime_2->refsys.type || ptime_1->refsys.instance != ptime_2->refsys.instance) return ALL_ARGS_MUST_SHARE_SAME_RS;
	else {
		POSTTIME * bbox_1 = palloc( sizeof(POSTTIME) + sizeof(JULIAN_DAY) );
		POSTTIME * bbox_2 = palloc( sizeof(POSTTIME) + sizeof(JULIAN_DAY) );
		memset(bbox_1,'\0', sizeof(POSTTIME) + sizeof(JULIAN_DAY) );
		memset(bbox_1,'\0', sizeof(POSTTIME) + sizeof(JULIAN_DAY) );
		temporal_bbox_single_instance( ptime_1 , bbox_1 );
		temporal_bbox_single_instance( ptime_2 , bbox_2 );

		bbox->refsys.type = ptime_1->refsys.type;
		bbox->refsys.instance = ptime_1->refsys.instance;
		bbox->type = 2;

		if( bbox_1->data[0] > bbox_2->data[0] ) bbox->data[0] = bbox_2->data[0];
		else bbox->data[0] = bbox_1->data[0];
		if( bbox_1->data[1] < bbox_2->data[1] ) bbox->data[1] = bbox_2->data[1];
		else bbox->data[1] = bbox_1->data[1];

		SET_VARSIZE( bbox , sizeof(POSTTIME) + sizeof(JULIAN_DAY));

		FREE_MEM(bbox_1);
		FREE_MEM(bbox_2);
	}
	return NO_ERROR;
}

pt_error_type centroid(POSTTIME * ptime, POSTTIME * centroid){
	if( ptime->refsys.type == 3 ) return FUNCTION_UNDEFINED_FOR_ORDINAL;
	centroid->type = 1;
	centroid->refsys.type = ptime->refsys.type;
	centroid->refsys.instance = ptime->refsys.instance;

	POSTTIME * bbox = palloc( sizeof(POSTTIME) + sizeof(JULIAN_DAY) );
	memset( bbox , 0 , sizeof(POSTTIME) + sizeof(JULIAN_DAY));
	temporal_bbox_single_instance( ptime , bbox );
	centroid->data[0] = ( bbox->data[0] + bbox->data[1] ) / 2;

	SET_VARSIZE(centroid, sizeof(POSTTIME));
	FREE_MEM(bbox);
	return NO_ERROR;
}
