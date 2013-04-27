### PostTIME
The PostTIME project intends to enhance PostgreSQL's capabilities to handle the temporal dimension with a focus on processing spatial information. For that purpose it provides:

1. *__6__ different __object types__ to abstract time*
    * Instants and periods of time
    * MultiInstants and MultiPeriods (composition types)
    * RegularMultiInstant and RegularMultiPeriod (to model regular repeating events)
2. *Various __temporal reference systems__ of __3__ different general types*
    * Temporal coordinate systems e.g. UNIX-Time
    * Calendar and clock systems like the Gregorian calendar with UTC
    * Ordinal systems e.g. the geological eras.
3. *Several __SQL functions__ for your __analysis__ tasks - please see below for more* information about this.

The concept is basing on ISO19108. The project is in an __early stage__, so please keep in mind that PostTIME is more or less unstable and anything but complete.

###Install instructions - Set up as PostgreSQL extension
PostTIME makes use of the PostgreSQL extension feature so you need PostgreSQL version 9.1 or higher. For more information see the [PostgreSQL documentation](http://www.postgresql.org/docs/ "www.postgresql.org/docs/").

To make PostTIME available as an extension module please download the *TimeCoding* and the *pg_extension_makefile* folder in the same directory. Then run `make` and `make install`.

Now you can easily add the PostTIME extension to a database with a CREATE EXTENSION statement like:

    CREATE EXTENSION posttime;
 
You can remove the extension with:

    DROP EXTENSION posttime;

If you want to undo the installation just remove all files naming posttime from your PostgreSQL *extension* directory and the shared library object from PostgreSQL's *$libdir*.

###PostTIME SQL functions

##### pt\_transform\_system( PostTIME , text ) : PostTIME 
Transforms a PostTIME instance into the reference system specified by the given key. *Examples:*

    // UNIX time to gregorian calendar.
    SELECT pt_transform_system('TCS0021'::posttime, 'CAL001')
    //result
    'CAL001001970-01-01T00:00:01.000Z'

     // or calendar date to julian day
    SELECT pt_transform_system('CAL0012013-4-30'::posttime, 'TCS001')
    'TCS001002456412.500000'

##### pt\_regular\_multi_to\_multi( PostTIME ) : PostTIME 
This function transform's an RegularMultiObject into a normal MultiObject, what means you get concrete instants. *Examples:*

    SELECT pt_regular_multi_to_multi('R3/2012-1-1/P3M/P9M'::posttime);
    //result:  
    'CAL001002012-01-01T00:00:00.000Z/002012-04-01T00:00:00.000Z,
    002013-01-01T00:00:00.000Z/002013-04-01T00:00:00.000Z,
    002014-01-01T00:00:00.000Z/002014-04-01T00:00:00.000Z,
    002015-01-01T00:00:00.000Z/002015-04-01T00:00:00.000Z'

    SELECT pt_regular_multi_to_multi('R3/2012-1-1/P30D'::posttime);
    //result:
    'CAL001002012-01-01T00:00:00.000Z,002012-01-31T00:00:00.000Z,
    002012-03-01T00:00:00.000Z,002012-03-31T00:00:00.000Z'

    SELECT pt_regular_multi_to_multi('TCS002R2/21224211/P3123321'::posttime);
    //result
    'TCS002021224211.000000,210891107532.000000,421760990853.000000'