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
-- ASSIGNMENT CAST FROM TEXT
-------------------------------------------------------------------

CREATE OR REPLACE FUNCTION posttime_in(text)
        RETURNS posttime
        AS '$libdir/posttime'
        LANGUAGE 'c' IMMUTABLE STRICT;

CREATE CAST (text AS posttime)
        WITH FUNCTION posttime_in(text)
        AS ASSIGNMENT;

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
	
CREATE OR REPLACE FUNCTION pt_transform_system(posttime, cstring)
	RETURNS posttime
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;

CREATE OR REPLACE FUNCTION pt_regular_multi_to_multi(posttime)
        RETURNS posttime
        AS '$libdir/posttime'
        LANGUAGE 'c' IMMUTABLE STRICT;

