/*
 * pt_errors.c
 */

#include "pt_errors.h"

const pt_error_msg pt_error_msgs[] = {
		{ NO_ERROR,
		"No Error occurred."},
		{ PERIOD_START_BEFORE_END,
		"A valid period's begin instant is before the end instant."	},
		{ PRIMITIVES_MUST_BE_OF_HOMOGENOUS_TYPE,
		"Primitives must be of homogeneous type." },
		{ PERIOD_CAN_ONLY_CONTAIN_SINGLE_SLASH,
		"Syntax error: an valid period can only contain one '/'."},
		{ AN_INSTANT_STRING_ENDS_WITH_DELIMITER,
		"Syntax error: an instant-string can not end with a ISO8601-delimiter as '-','T' or ':' ."},
		{ CAN_NOT_PARSE_SECONDS,
		"Syntax error: error while trying to parse seconds."},
		{ CAN_NOT_PARSE_INTEGER_VALUE,
		"Syntax error: error while trying to parse a single year, month, day, hour, or minute."},
		{ CALENDAR_SYSTEM_NOT_FOUND ,
		"A calendar system with the given id is unknown."},
		{ CAN_NOT_PARSE_TCS_VALUE ,
		"Syntax error: within a temporal coordinate system an instant is defined by a single floating point number." } ,
		{ TCS_SYSTEM_NOT_FOUND ,
		"A temporal coordinate system with the given id is unknown."},
		{ SPECIFY_ONLY_THE_NEW_SYSTEM ,
		"Please specify only the new reference system."},
		{ ORD_SYSTEM_NOT_FOUND ,
		"An ordinal system with the given id is unknown."},
		{ ORD_ERA_UNKNOWN ,
		"The given ordinal-era-key is unknown in the specified system."},
		{ EXPECTED_REGULAR_STR_IS_NOT_REGULAR ,
		"Assumed that the given string represents a RegularMultiObject, but is not valid in this way."},
		{ INVALID_SYNTAX_AT_OR_NEAR_REGULAR_VALUE ,
		"An error occurred during parsing a single regular value"	},
		{ REGULAR_MULTI_FUNCTION ,
		"This function is only defined for RegularMultiObjects."},
		{FUNCTION_UNDEFINED_FOR_ORDINAL ,
		"This function does not work with Ordinal Systems"},
		{CALENDAR_VALUES_NOT_VALID ,
		"This values are not representing an valid datum in this calendar"},
		{SHORT_FORM_AGREEMENT_CONFUSION ,
		"If you make use of the short form period agreement in a MultiInstant you have to use it for all containing single instants."},
};

