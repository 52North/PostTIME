/*
 * relative_position.c
 *
 *  Created on: 05.05.2013
 *      Author: ptr
 */

#include "relative_position.h"

void set_str(int32 * i, char * str){
	switch(*i){
	case 0:
		strcpy(str , "Before");
		break;
	case 1:
		strcpy(str , "Meets");
		break;
	case 2:
		strcpy(str , "Overlaps");
		break;
	case 3:
		strcpy(str , "Begins");
		break;
	case 4:
		strcpy(str , "BegunBy");
		break;
	case 5:
		strcpy(str , "During");
		break;
	case 6:
		strcpy(str , "Contains");
		break;
	case 7:
		strcpy(str , "Equals");
		break;
	case 8:
		strcpy(str , "OverlappedBy");
		break;
	case 9:
		strcpy(str , "Ends");
		break;
	case 10:
		strcpy(str , "EndedBy");
		break;
	case 11:
		strcpy(str , "MetBy");
		break;
	case 12:
		strcpy(str , "After");
		break;
	case 13:
		strcpy(str , "NAP");
		break;
	default:
		break;
	}
}


int32 relative_position_ii(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[0] < ptime_2->data[0])return 0;
	else if(ptime_1->data[0] == ptime_2->data[0])return 7;
	else if(ptime_1->data[0] > ptime_2->data[0])return 12;
	else return 13;
}

int32 relative_position_pi(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[1] < ptime_2->data[0])return 0;
	else if(ptime_1->data[1] == ptime_2->data[0])return 10;
	else if((ptime_1->data[0] < ptime_2->data[0]) && (ptime_1->data[1] > ptime_2->data[0]))return 6;
	else if(ptime_1->data[0] == ptime_2->data[0])return 4;
	else if(ptime_1->data[0] > ptime_2->data[0])return 12;
	else return 13;
}

int32 relative_position_ip(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[0] < ptime_2->data[0])return 0;
	else if(ptime_1->data[0] == ptime_2->data[0])return 3;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[0] < ptime_2->data[1])return 5;
	else if(ptime_1->data[0] == ptime_2->data[1])return 9;
	else if(ptime_1->data[0] > ptime_2->data[1])return 12;
	else return 13;
}

int32 relative_position_pp(POSTTIME * ptime_1,POSTTIME * ptime_2){
	if(ptime_1->data[1] < ptime_2->data[0])return 0;
	else if(ptime_1->data[1] == ptime_2->data[0])return 1;
	else if(ptime_1->data[0] < ptime_2->data[0] && ptime_1->data[1] > ptime_2->data[0] && ptime_1->data[1] < ptime_2->data[1])return 2;
	else if(ptime_1->data[0] == ptime_2->data[0] && ptime_1->data[1] < ptime_2->data[1])return 3;
	else if(ptime_1->data[0] == ptime_2->data[0] && ptime_1->data[1] > ptime_2->data[1])return 4;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[1] < ptime_2->data[1])return 5;
	else if(ptime_1->data[0] < ptime_2->data[0] && ptime_1->data[1] > ptime_2->data[1])return 6;
	else if(ptime_1->data[0] == ptime_2->data[0] && ptime_1->data[1] == ptime_2->data[1])return 7;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[0] < ptime_2->data[1] && ptime_1->data[1] > ptime_2->data[1])return 8;
	else if(ptime_1->data[0] > ptime_2->data[0] && ptime_1->data[1] == ptime_2->data[1])return 9;
	else if(ptime_1->data[0] < ptime_2->data[0] && ptime_1->data[1] == ptime_2->data[1])return 10;
	else if(ptime_1->data[0] == ptime_2->data[1])return 11;
	else if(ptime_1->data[0] > ptime_2->data[1])return 12;
	else return 13;
}

pt_error_type relative_position_str(POSTTIME * ptime_1 , POSTTIME * ptime_2 , char * str){
	pt_error_type err_ret = NO_ERROR;
	int32 rp = 13;
//	if(ptime_1->refsys > 199){
//		if(ptime_1->type > 1 || ptime_2->type > 1){
//		    ereport(ERROR,
//			    (errcode(ERRCODE_INVALID_PARAMETER_VALUE),
//			     errmsg("Sorry, this function is only for ordinal INSTANTS available")));
//		}
//		int32 rp = relative_position_pp(ptime_1,ptime_2);
//		set_str(&rp,str);
//		return TRUE;
//	}
	// Multi --> only defined fr primitive types!
	if( (ptime_1->type > 2) || (ptime_2->type > 2) ){
		rp = 13;
		set_str(&rp,str);
	}
	else if((ptime_1->type == 1) && (ptime_2->type == 1)){
		rp = relative_position_ii(ptime_1 , ptime_2);
		set_str(&rp,str);
	}
	else if((ptime_1->type == 2) && (ptime_2->type == 1)){
		rp = relative_position_pi(ptime_1,ptime_2);
		set_str(&rp,str);
	}
	else if((ptime_1->type == 1) && (ptime_2->type == 2)){
		rp = relative_position_ip(ptime_1,ptime_2);
		set_str(&rp,str);
	}
	else if((ptime_1->type == 2) && (ptime_2->type == 2)){
		rp = relative_position_pp(ptime_1,ptime_2);
		set_str(&rp,str);
	}
	return err_ret;
}

pt_error_type relative_position_int(POSTTIME * ptime_1,POSTTIME * ptime_2, int32 * ret_int32){
	pt_error_type err_ret = NO_ERROR;
	if( (ptime_1->type > 2) || (ptime_2->type > 2) ){
		// err_ret = ISO19108_ONLY_FOR_PRIMITIVES;
		*ret_int32 = -1;
	}
	else if((ptime_1->type == 1) && (ptime_2->type == 1)){
		*ret_int32 = relative_position_ii(ptime_1,ptime_2);
	}
	else if((ptime_1->type == 2) && (ptime_2->type == 1)){
		*ret_int32 = relative_position_pi(ptime_1,ptime_2);
	}
	else if((ptime_1->type == 1) && (ptime_2->type == 2)){
		*ret_int32 = relative_position_ip(ptime_1,ptime_2);
	}
	else if((ptime_1->type == 2) && (ptime_2->type == 2)){
		*ret_int32 = relative_position_pp(ptime_1,ptime_2);
	}
	return err_ret;
}



