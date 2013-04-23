-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
-- This is free software; you can redistribute and/or modify it under
-- the terms of the GNU General Public Licence. See the COPYING file.
--
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
--
-- WARNING: Any change in this file must be evaluated for compatibility.
--          Changes cleanly handled by postgis_upgrade.sql are fine,
--	    other changes will require a bump in Major version.
--	    Currently only function replaceble by CREATE OR REPLACE
--	    are cleanly handled.
--
-- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -












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
-- BASIC DATA PROCESSING FUNCTIONALITY
-------------------------------------------------------------------

CREATE OR REPLACE FUNCTION transform_system(posttime, cstring)
	RETURNS posttime
	AS '$libdir/posttime'
	LANGUAGE 'c' IMMUTABLE STRICT;
