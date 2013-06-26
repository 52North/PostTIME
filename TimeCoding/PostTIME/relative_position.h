/*
 * relative_position.h
 *
 *  Created on: 05.05.2013
 *      Author: ptr
 */

#include "geotime.h"

#ifndef RELATIVE_POSITION_H_
#define RELATIVE_POSITION_H_

typedef enum {
  BEFORE = 0,
  MEETS = 1,
  OVERLAPS = 2,
  BEGINS = 3,
  BEGUN_BY = 4,
  DURING = 5,
  CONTAINS = 6,
  EQUALS = 7,
  OVERLAPPED_BY = 8,
  ENDS = 9,
  ENDED_BY = 10,
  MET_BY = 11,
  AFTER = 12,
  NAP = 13
} rel_pos;


rel_pos relative_position_ii(POSTTIME * ptime_1,POSTTIME * ptime_2);
rel_pos relative_position_pi(POSTTIME * ptime_1,POSTTIME * ptime_2);
rel_pos relative_position_ip(POSTTIME * ptime_1,POSTTIME * ptime_2);
rel_pos relative_position_pp(POSTTIME * ptime_1,POSTTIME * ptime_2);
void relative_position_str(POSTTIME * ptime_1 , POSTTIME * ptime_2 , char * str);
void relative_position_rp(POSTTIME * ptime_1 , POSTTIME * ptime_2 , rel_pos * ret_rp);
pt_error_type simultaneous(POSTTIME * ptime1 , POSTTIME * ptime2 , bool * is_simultaneous);
pt_error_type overlaps(POSTTIME * ptime1 , POSTTIME * ptime2 , bool * overlaps);
pt_error_type simultaneous_excluded_end_instants(POSTTIME * ptime1 , POSTTIME * ptime2 , bool * is_simultaneous);





#endif /* RELATIVE_POSITION_H_ */
