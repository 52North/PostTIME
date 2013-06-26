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
   LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION posttime_out(posttime)
   RETURNS cstring
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;

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
        AS $$
        BEGIN
        RETURN posttime_in($1::cstring);
        END;
      $$ LANGUAGE plpgsql;

CREATE CAST (text AS posttime)
        WITH FUNCTION posttime(text)
        AS ASSIGNMENT;

-------------------------------------------------------------------
-- Casts from and to PostgreSQL's basic date types.

-- timestamp
CREATE OR REPLACE FUNCTION posttime(timestamp with time zone)
RETURNS posttime
AS '$libdir/posttime', 'pt_cast_from_timestamp'
LANGUAGE c IMMUTABLE STRICT;

CREATE CAST (timestamp with time zone AS posttime)
    WITH FUNCTION posttime(timestamp with time zone);
    
CREATE OR REPLACE FUNCTION posttime(timestamp without time zone)
RETURNS posttime
AS '$libdir/posttime', 'pt_cast_from_timestamp'
LANGUAGE c IMMUTABLE STRICT;

CREATE CAST (timestamp without time zone AS posttime)
    WITH FUNCTION posttime(timestamp without time zone);
    
CREATE OR REPLACE FUNCTION timestamp_without_time_zone(posttime)
RETURNS timestamp without time zone
AS '$libdir/posttime', 'pt_cast_to_timestamp'
LANGUAGE c IMMUTABLE STRICT;

CREATE CAST (posttime AS timestamp without time zone )
    WITH FUNCTION timestamp_without_time_zone(posttime);
    
CREATE OR REPLACE FUNCTION timestamp_with_time_zone(posttime)
RETURNS timestamp with time zone
AS '$libdir/posttime', 'pt_cast_to_timestamp'
LANGUAGE c IMMUTABLE STRICT;

CREATE CAST (posttime AS timestamp with time zone)
    WITH FUNCTION timestamp_with_time_zone(posttime);

-- date
CREATE OR REPLACE FUNCTION posttime(date)
RETURNS posttime
AS '$libdir/posttime', 'pt_cast_from_date'
LANGUAGE c IMMUTABLE STRICT;

CREATE CAST (date AS posttime)
    WITH FUNCTION posttime(date);
    
CREATE OR REPLACE FUNCTION date(posttime)
RETURNS date
AS '$libdir/posttime', 'pt_cast_to_date'
LANGUAGE c IMMUTABLE STRICT;

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
   LANGUAGE c STRICT;

CREATE OR REPLACE FUNCTION pt_temporal_bbox(posttime, posttime)
   RETURNS posttime
   AS '$libdir/posttime' , 'pt_temporal_bbox_two_args'
   LANGUAGE c STRICT;
   
CREATE AGGREGATE pt_temporal_bbox (posttime) (
   SFUNC = pt_temporal_bbox,
   STYPE = posttime,
   FINALFUNC = pt_temporal_bbox_instance
);
-------------------------------------------------------------------
-- more basic stuff

CREATE OR REPLACE FUNCTION pt_centroid(posttime)
   RETURNS posttime
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;  

CREATE OR REPLACE FUNCTION pt_transform_system(posttime, cstring)
   RETURNS posttime
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION pt_regular_multi_to_multi(posttime)
        RETURNS posttime
        AS '$libdir/posttime'
        LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION pt_simultaneous(posttime ,posttime)
   RETURNS boolean
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;
   
CREATE OR REPLACE FUNCTION pt_overlaps(posttime ,posttime)
   RETURNS boolean
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;
   
CREATE OR REPLACE FUNCTION pt_simultaneous_excluded_end_instants(posttime ,posttime)
   RETURNS boolean
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;
   
CREATE OR REPLACE FUNCTION pt_weekday_int(posttime)
   RETURNS integer
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;
   
CREATE OR REPLACE FUNCTION pt_type(posttime)
   RETURNS integer
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;  
   
CREATE OR REPLACE FUNCTION pt_refsys_type(posttime)
   RETURNS integer
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;  
   
CREATE OR REPLACE FUNCTION pt_refsys_instance(posttime)
   RETURNS integer
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;  
-------------------------------------------------------------------
-- ISO19108 basic analysis for primitives

CREATE OR REPLACE FUNCTION tm_relative_position(posttime, posttime)
   RETURNS cstring
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION tm_relative_position_int(posttime, posttime)
   RETURNS integer
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;
   
CREATE OR REPLACE FUNCTION tm_distance(posttime, posttime)
   RETURNS cstring
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION tm_distance_dec_day(posttime, posttime)
   RETURNS double precision
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;
   
CREATE OR REPLACE FUNCTION tm_duration(posttime)
   RETURNS cstring
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION tm_duration_dec_day(posttime)
   RETURNS double precision
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;

-------------------------------------------------------------------
-- SET RETURNING
-------------------------------------------------------------------

CREATE OR REPLACE FUNCTION pt_histogram_intervals( posttime , cstring )
    RETURNS SETOF posttime
    AS '$libdir/posttime'
    LANGUAGE C IMMUTABLE STRICT;

-------------------------------------------------------------------
-- POSTGIS DEPENDENCIES
-------------------------------------------------------------------  

CREATE OR REPLACE FUNCTION pt_successor(geometry, posttime, geometry, posttime)
   RETURNS boolean
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;
   
CREATE OR REPLACE FUNCTION pt_predecessor(geometry, posttime, geometry, posttime)
   RETURNS boolean
   AS '$libdir/posttime'
   LANGUAGE c IMMUTABLE STRICT;

-------------------------------------------------------------------
-------------------------------------------------------------------
-- PLPGSQL FUNCTIONS
-------------------------------------------------------------------
-------------------------------------------------------------------

CREATE OR REPLACE FUNCTION pt_spatiotemporal_statistics(IN event text, IN reference_geom text,IN duration text,IN bigint_id text) RETURNS TABLE(id bigint, ptime posttime, count bigint) AS $$
DECLARE
  temporal_bbox posttime;
  ptime_itr posttime;
  posttime_set_first_name text;
  geometry_set_first_name text;
  geometry_set_second_name text;
  int_id_second_name ALIAS FOR $4;
  geo_itr_static geometry;
  int_itr bigint;
BEGIN
   EXECUTE 'SELECT (SELECT attname FROM pg_attribute WHERE 
   attrelid = '
   || quote_ident($1)::regclass::oid
   || ' AND atttypid = '
   || quote_literal('posttime')
   || '::regtype::oid)::text;'
   INTO posttime_set_first_name;
   EXECUTE 'SELECT (SELECT attname FROM pg_attribute WHERE 
   attrelid = '
   || quote_ident($1)::regclass::oid
   || ' AND atttypid = '
   || quote_literal('geometry')
   || '::regtype::oid)::text;'
   INTO geometry_set_first_name;
   EXECUTE 'SELECT (SELECT attname FROM pg_attribute WHERE 
   attrelid = '
   || quote_ident($2)::regclass::oid
   || ' AND atttypid = '
   || quote_literal('geometry')
   || '::regtype::oid)::text;'
   INTO geometry_set_second_name;
   FOR ptime_itr IN EXECUTE format('SELECT pt_histogram_intervals(pt_temporal_bbox( %s ), %L::cstring) As pt_intervals FROM %I',
   posttime_set_first_name , $3 , $1) 
   LOOP
         FOR int_itr, geo_itr_static IN EXECUTE format(
            'SELECT %s , %s FROM %I',
                int_id_second_name::text , geometry_set_second_name , $2)
                LOOP
                        RETURN QUERY EXECUTE format('SELECT 
                        %L::bigint , 
                        %L::posttime , 
                        count(simul.%s) FROM 
                        (SELECT %s , %s FROM %I As inters WHERE 
                        pt_simultaneous_excluded_end_instants( pt_centroid(%s) , %L ) = TRUE) As simul
                        WHERE ST_Intersects( %s , %L::geometry ) GROUP BY %L::bigint',
                        int_itr,
                        ptime_itr,
                        posttime_set_first_name,
                        posttime_set_first_name, 
                        geometry_set_first_name, 
                        $1 , 
                        posttime_set_first_name , 
                        ptime_itr,
                        geometry_set_first_name,
                        geo_itr_static,
                        int_itr);
                 END LOOP;
       END LOOP;
END;
$$ LANGUAGE plpgsql;



-----------------------------
-- JIRIS NEW FANCY FUNCTIONS
-----------------------------

CREATE FUNCTION posttime_after(posttime, posttime)
    RETURNS boolean
    AS '$libdir/posttime', 'tm_after'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR > (
  leftarg = posttime,
  rightarg = posttime,
  procedure =  posttime_after
);


CREATE FUNCTION posttime_before(posttime, posttime)
    RETURNS boolean
    AS '$libdir/posttime', 'tm_before'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR < (
  leftarg = posttime,
  rightarg = posttime,
  procedure =  posttime_before
);

CREATE FUNCTION posttime_equals(posttime, posttime)
    RETURNS boolean
    AS '$libdir/posttime', 'tm_equal'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR = (
    leftarg = posttime,
    rightarg = posttime,
    procedure =  posttime_equals
);

CREATE FUNCTION posttime_before_or_equals(posttime, posttime)
    RETURNS boolean
    AS '$libdir/posttime', 'tm_ltequal'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR <= (
    leftarg = posttime,
    rightarg = posttime,
    procedure =  posttime_before_or_equals
);

CREATE FUNCTION posttime_after_or_equals(posttime, posttime)
    RETURNS boolean
    AS '$libdir/posttime', 'tm_gtequal'
    LANGUAGE C IMMUTABLE STRICT;

CREATE OPERATOR >= (
    leftarg = posttime,
    rightarg = posttime,
    procedure =  posttime_after_or_equals
);

CREATE FUNCTION tm_helpfunc(posttime, posttime)
    RETURNS integer
    AS '$libdir/posttime', 'tm_helpfunc'
    LANGUAGE C IMMUTABLE STRICT;
    
CREATE OPERATOR CLASS posttime_ops
    DEFAULT FOR TYPE posttime USING btree AS
        OPERATOR        1       < ,
        OPERATOR        2       <= ,
        OPERATOR        3       = ,
        OPERATOR        4       >= ,
        OPERATOR        5       > ,
        FUNCTION        1       tm_helpfunc(posttime, posttime);



