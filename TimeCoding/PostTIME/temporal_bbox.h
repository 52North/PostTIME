/*
 * temporal_bbox.h
 *
 *  Created on: 28.04.2013
 *      Author: ptr
 */

#include "geotime.h"

#ifndef TEMPORAL_BBOX_H_
#define TEMPORAL_BBOX_H_

void temporal_bbox_single_instance(POSTTIME * ptime, POSTTIME * bbox);
pt_error_type temporal_bbox_single_two_instances( POSTTIME * ptime_1 , POSTTIME * ptime_2 , POSTTIME * bbox );
pt_error_type centroid(POSTTIME * ptime, POSTTIME * centroid);

#endif /* TEMPORAL_BBOX_H_ */
