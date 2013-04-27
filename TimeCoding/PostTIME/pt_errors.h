/*
 * pt_errors.h
 */

#ifndef PT_ERRORS_H_
#define PT_ERRORS_H_

#define ERRCHECK(func_return) if(func_return == 0) byte_ret = 0;
#define PG_ERRCHECK(func_return) if(func_return != NO_ERROR) {ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),errmsg(pt_error_msgs[func_return].msg))); PG_RETURN_NULL()};

#define DIRECT_SYNTAX_ER(message) {ereport(ERROR,(errcode(ERRCODE_INVALID_PARAMETER_VALUE),errmsg(message))); return 0 ;}

typedef enum pt_error_type {
	NO_ERROR = 0,
	PERIOD_START_BEFORE_END,
	PRIMITIVES_MUST_BE_OF_HOMOGENOUS_TYPE,
	PERIOD_CAN_ONLY_CONTAIN_SINGLE_SLASH,
	AN_INSTANT_STRING_ENDS_WITH_DELIMITER,
	CAN_NOT_PARSE_SECONDS,
	CAN_NOT_PARSE_INTEGER_VALUE,
	CALENDAR_SYSTEM_NOT_FOUND,
	CAN_NOT_PARSE_TCS_VALUE,
	TCS_SYSTEM_NOT_FOUND,
	SPECIFY_ONLY_THE_NEW_SYSTEM,
	ORD_SYSTEM_NOT_FOUND,
	ORD_ERA_UNKNOWN,
	EXPECTED_REGULAR_STR_IS_NOT_REGULAR,
	INVALID_SYNTAX_AT_OR_NEAR_REGULAR_VALUE,
	REGULAR_MULTI_FUNCTION,
	FUNCTION_UNDEFINED_FOR_ORDINAL,
	CALENDAR_VALUES_NOT_VALID,
} pt_error_type;

typedef struct pt_error_msg {
	pt_error_type code;
	const char *msg;
} pt_error_msg;

extern const pt_error_msg pt_error_msgs[];


#endif /* PT_ERRORS_H_ */
