/*
 * relative_position.h
 *
 *  Created on: 05.05.2013
 *      Author: ptr
 */

#include "geotime.h"

#ifndef RELATIVE_POSITION_H_
#define RELATIVE_POSITION_H_

void set_str(int32 * i, char * str);
int32 relative_position_ii(POSTTIME * ptime_1,POSTTIME * ptime_2);
int32 relative_position_pi(POSTTIME * ptime_1,POSTTIME * ptime_2);
int32 relative_position_ip(POSTTIME * ptime_1,POSTTIME * ptime_2);
int32 relative_position_pp(POSTTIME * ptime_1,POSTTIME * ptime_2);
pt_error_type relative_position_str(POSTTIME * ptime_1 , POSTTIME * ptime_2 , char * str);
pt_error_type relative_position_int(POSTTIME * ptime_1 , POSTTIME * ptime_2 , int32 * ret_int32);
pt_error_type simultaneous(POSTTIME * ptime1 , POSTTIME * ptime2 , bool * is_simultaneous);
pt_error_type overlaps(POSTTIME * ptime1 , POSTTIME * ptime2 , bool * overlaps);
pt_error_type simultaneous_excluded_end_instants(POSTTIME * ptime1 , POSTTIME * ptime2 , bool * is_simultaneous);

#endif /* RELATIVE_POSITION_H_ */
