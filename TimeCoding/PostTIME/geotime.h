/*
 * geotime.h
 * Purpose: Global needed macros & structs
 */

#include "postgres.h"
#include "fmgr.h"

#include "executor/spi.h"
#include "utils/builtins.h"

#include "pt_errors.h"
#include "utils/timestamp.h"
#include "utils/date.h"

#ifndef GEOTIME_H_
#define GEOTIME_H_

// #define POSTGIS_SHLIB_NAME "postgis-2.0.so"

extern char postgis_sharedlib[];
PGFunction pgis_relate_pattern;

/* Less code macros */
#define FREE_MEM(p)   do { pfree(p); (p) = NULL; } while(0);
#define MILLIS 86400000
/* Syntax defining macros */
#define PRIMITIVE_SEPARATOR ','
#define PERIOD_SEPARATOR '/'
#define REGULAR_SEPARATOR '/'

/* Refsys-tags */
#define CALENDAR_FLAG "CAL"
#define TEMPORAL_COORDINATE_FLAG "TCS"
#define ORDINAL_FLAG "ORD"
#define NUMBER_REFSYS_TYPES 3

extern const char * refsys_types[];

typedef int64 JULIAN_DAY; /*!< JULIAN_DAY represents internal every time instant,
what means that we ignore the external used reference system.  */
typedef int8 BYTE; /*!< Yes, a byte macro. */

/*!
 * REFSYS is used to bind a temporal reference system to an instance.
 * It does not contain a detailed description off the system, but is using an id-based approach.
 * For practical reasons the systems general type (Calendar, Coordinate or Ordinal) is stored explicit.
 */
typedef struct {
	BYTE type; /*!< Refsys-type: Calendar=1, Coordinate=2, Ordinal=3 */
	int16 instance; /*!< This refsys-instance's id */
} REFSYS;


typedef enum {
	INSTANT = 1,
	PERIOD = 2,
	MULTIINSTANT = 3,
	MULTIPERIOD = 4,
	TERM_REGULARMULTIINSTANT = 5,
	TERM_REGULARMULTIPERIOD = 6,
	REGULARMULTIINSTANT = 7,
	REGULARMULTIPERIOD = 8	
} postime_type;

/*!
 * The internal representation for any instance.
 * This must verify an universal interface for all objects and reference systems.
 * From the PostgreSQL point-of-view this is an user defined base type with a variable length -
 * for information about the related restrictions see the PostgreSQL doc.
 */
typedef struct {
        char vl_len_[4]; /*!< PostgreSQL field to store this instance's length */
        REFSYS refsys;   /*!< An ID-based approach for refsys-managment */
        postime_type type; /* type of posttime datum */
        JULIAN_DAY data[1];  /*!< This object's instants. Stored as JULIAN_DAY. */
} POSTTIME;

/*!Enumeration to handle instances with different granularity. */
typedef enum calendar_gran { YEAR, MONTH, DAY, HOUR, MINUTE, SECOND } calendar_gran;

void regular_multi_to_multi( POSTTIME * ptime_reg , POSTTIME * ptime_multi );
pt_error_type get_rvalue(POSTTIME * ptime_reg, int32 * rvalue);
pt_error_type transform_regular_system( POSTTIME * ptime , REFSYS * refsys_new , POSTTIME * ptime_new );

#endif /* GEOTIME_H_ */
