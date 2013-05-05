/*
 * relative_position.h
 *
 *  Created on: 05.05.2013
 *      Author: ptr
 */

#include "geotime.h"

#ifndef RELATIVE_POSITION_H_
#define RELATIVE_POSITION_H_

pt_error_type relative_position_str(POSTTIME * ptime_1 , POSTTIME * ptime_2 , char * str);
pt_error_type relative_position_int(POSTTIME * ptime_1 , POSTTIME * ptime_2 , int32 * ret_int32);


#endif /* RELATIVE_POSITION_H_ */
