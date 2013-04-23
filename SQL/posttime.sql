/*************************************** TYPE DECLARATION */

-- declare as shell
CREATE TYPE posttime;

/*************************************** FUNDAMENTAL TYPE FUNCTIONS */
-- input function
CREATE FUNCTION posttime_in(cstring)
    RETURNS posttime
    AS '$libdir/libposttime.so'
    LANGUAGE C IMMUTABLE STRICT;
-- extern function
CREATE FUNCTION posttime_out(posttime)
    RETURNS cstring
    AS '$libdir/libposttime.so'
    LANGUAGE C IMMUTABLE STRICT;

/***************************************** CREATE TYPE */
-- complete type declaration
CREATE TYPE posttime (
        internallength = VARIABLE,
        input = posttime_in,
        output = posttime_out,
        alignment = double
);

