-------------------------------------------------------------------
-------------------------------------------------------------------
--   Copyright 2013-04-23 52North
--
--   Licensed under the Apache License, Version 2.0 (the "License");
--   you may not use this file except in compliance with the License.
--   You may obtain a copy of the License at
--
--       http://www.apache.org/licenses/LICENSE-2.0
--
--   Unless required by applicable law or agreed to in writing, software
--   distributed under the License is distributed on an "AS IS" BASIS,
--   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
--   See the License for the specific language governing permissions and
--   limitations under the License.
-------------------------------------------------------------------
-------------------------------------------------------------------


SET client_min_messages TO warning;

-- INSTALL VERSION: 1.0.0



-------------------------------------------------------------------
--  POSTTIME TYPE
-------------------------------------------------------------------
CREATE OR REPLACE FUNCTION posttime_in(cstring)
	RETURNS posttime
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION posttime_out(posttime)
	RETURNS cstring
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE TYPE posttime (
	alignment = double,
	internallength = VARIABLE,
	input = posttime_in,
	output = posttime_out
);

-------------------------------------------------------------------
-- CASTS
-------------------------------------------------------------------

-------------------------------------------------------------------
-- Assignment cast from text

CREATE OR REPLACE FUNCTION posttime(text)
        RETURNS posttime
        AS '$libdir/posttime','posttime_in'
        LANGUAGE 'c' IMMUTABLE STRICT;

CREATE CAST (text AS posttime)
        WITH FUNCTION posttime(text)
        AS ASSIGNMENT;

-------------------------------------------------------------------
-- Casts from and to PostgreSQL's basic date types.

-- timestamp
CREATE OR REPLACE FUNCTION posttime(timestamp with time zone)
RETURNS posttime
AS '$libdir/posttime', 'pt_cast_from_timestamp'
LANGUAGE 'C' IMMUTABLE STRICT;

CREATE CAST (timestamp with time zone AS posttime)
    WITH FUNCTION posttime(timestamp with time zone);
    
CREATE OR REPLACE FUNCTION posttime(timestamp without time zone)
RETURNS posttime
AS '$libdir/posttime', 'pt_cast_from_timestamp'
LANGUAGE 'C' IMMUTABLE STRICT;

CREATE CAST (timestamp without time zone AS posttime)
    WITH FUNCTION posttime(timestamp without time zone);
    
CREATE OR REPLACE FUNCTION timestamp_without_time_zone(posttime)
RETURNS timestamp without time zone
AS '$libdir/posttime', 'pt_cast_to_timestamp'
LANGUAGE 'C' IMMUTABLE STRICT;

CREATE CAST (posttime AS timestamp without time zone )
    WITH FUNCTION timestamp_without_time_zone(posttime);
    
CREATE OR REPLACE FUNCTION timestamp_with_time_zone(posttime)
RETURNS timestamp with time zone
AS '$libdir/posttime', 'pt_cast_to_timestamp'
LANGUAGE 'C' IMMUTABLE STRICT;

CREATE CAST (posttime AS timestamp with time zone)
    WITH FUNCTION timestamp_with_time_zone(posttime);

-- date
CREATE OR REPLACE FUNCTION posttime(date)
RETURNS posttime
AS '$libdir/posttime', 'pt_cast_from_date'
LANGUAGE 'C' IMMUTABLE STRICT;

CREATE CAST (date AS posttime)
    WITH FUNCTION posttime(date);
    
CREATE OR REPLACE FUNCTION date(posttime)
RETURNS date
AS '$libdir/posttime', 'pt_cast_to_date'
LANGUAGE 'C' IMMUTABLE STRICT;

CREATE CAST (posttime AS date)
    WITH FUNCTION date(posttime);

-------------------------------------------------------------------
-- BASIC DATA PROCESSING FUNCTIONALITY
-------------------------------------------------------------------        

-------------------------------------------------------------------
-- Temporal Bounding Box        
        
CREATE OR REPLACE FUNCTION pt_temporal_bbox_instance(posttime)
	RETURNS posttime
	AS '$libdir/posttime'
	LANGUAGE 'c' STRICT;

CREATE OR REPLACE FUNCTION pt_temporal_bbox(posttime, posttime)
	RETURNS posttime
	AS '$libdir/posttime' , 'pt_temporal_bbox_two_args'
	LANGUAGE 'c' STRICT;
	
CREATE AGGREGATE pt_temporal_bbox (posttime) (
	SFUNC = pt_temporal_bbox,
	STYPE = posttime,
	FINALFUNC = pt_temporal_bbox_instance
);
-------------------------------------------------------------------
-- more basic stuff

CREATE OR REPLACE FUNCTION pt_transform_system(posttime, cstring)
	RETURNS posttime
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION pt_regular_multi_to_multi(posttime)
        RETURNS posttime
        AS '$libdir/posttime'
        LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION pt_simultaneous(posttime ,posttime)
	RETURNS boolean
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;
	
CREATE OR REPLACE FUNCTION pt_overlaps(posttime ,posttime)
	RETURNS boolean
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;   
	
CREATE OR REPLACE FUNCTION pt_weekday_int(posttime)
	RETURNS integer
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;   	
-------------------------------------------------------------------
-- ISO19108 basic analysis for primitives

CREATE OR REPLACE FUNCTION tm_relative_position(posttime, posttime)
	RETURNS cstring
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION tm_relative_position_int(posttime, posttime)
	RETURNS integer
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;
	
CREATE OR REPLACE FUNCTION tm_distance(posttime, posttime)
	RETURNS cstring
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION tm_distance_dec_day(posttime, posttime)
	RETURNS double precision
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;
	
CREATE OR REPLACE FUNCTION tm_duration(posttime)
	RETURNS cstring
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION tm_duration_dec_day(posttime)
	RETURNS double precision
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;

-------------------------------------------------------------------
-- POSTGIS DEPENDENCIES
-------------------------------------------------------------------  
	
CREATE OR REPLACE FUNCTION pt_successor(geometry, posttime, geometry, posttime)
	RETURNS boolean
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;
	
CREATE OR REPLACE FUNCTION pt_predecessor(geometry, posttime, geometry, posttime)
	RETURNS boolean
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;
	