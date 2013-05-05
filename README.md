# PostTIME
The PostTIME project intends to enhance PostgreSQL's capabilities to handle the temporal dimension with a focus on processing spatial information. For that purpose it provides:

1. *__6__ different __object types__ to abstract time*
    * Instants and periods of time
    * MultiInstants and MultiPeriods (composition types)
    * RegularMultiInstant and RegularMultiPeriod (to model regular repeating events)
2. *Various __temporal reference systems__ of __3__ different general types*
    * Temporal coordinate systems e.g. UNIX-Time
    * Calendar and clock systems like the Gregorian calendar with UTC
    * Ordinal systems e.g. the geological eras.
3. *Several __SQL functions__ for your __analysis__ tasks* - please see [below](https://github.com/52North/PostTIME#posttime-sql-functions) for more information about this.

The concept is basing on ISO19108. The project is in an __early stage__, so please keep in mind that PostTIME is more or less unstable and anything but complete.

#Install instructions - Set up as PostgreSQL extension
PostTIME makes use of the PostgreSQL extension feature so you need PostgreSQL version 9.1 or higher. For more information see the [PostgreSQL documentation](http://www.postgresql.org/docs/ "www.postgresql.org/docs/").

To make PostTIME available as an extension module please download the *TimeCoding* and the *pg_extension_makefile* folder in the same directory. Then run `make` and `make install`.

You can run local regression tests of your PostTIME installation with __`make installcheck`__.

Now you can easily add the PostTIME extension to a database with a CREATE EXTENSION statement like:

    CREATE EXTENSION posttime;
 
You can remove the extension with:

    DROP EXTENSION posttime;

If you want to undo the installation just remove all files naming posttime from your PostgreSQL *extension* directory and the shared library object from PostgreSQL's *$libdir*.

#PostTIME SQL functions
##ISO19108 Functions
The following functions are taken over from ISO19108 - for that reason these are only defined for the primitive object types. However, if you call one of these with a multi object you will get 'NAP' if the return type is text or '-1' if the return type is a number. That means you can call these functions with any PostTIME instance without causing an error.   
##### tm\_relative\_position( PostTIME , PostTIME ) : text 
Determine the relative position of one inctance to another and returns the result as a string according to ISO19108. *Examples:*

    // Calendar system...
    SELECT tm_relative_position('2013-6-11T9:12','2013-5-30/2016-2-29');
    // result
    'During'

     // ...coordinate system...
    SELECT tm_relative_position('TCS0028765/9765','TCS0029000/10000');
    // result
    'Overlaps'

    // ...or also using an ordinal system.
    SELECT tm_relative_position('ORD001Neoproterozoic/Ordovician','ORD001Devonian');
    // result
    'Before'

The function __tm\_relative\_position\_int( PostTIME , PostTIME ) : integer__ performs the same operation but returns an integer with the matching value from the ISO19108 enumeration type TM_RelativePosition.

##Basic Functions

##### pt\_temporal\_bbox( PostTIME , ... ) : PostTIME 
Calculates the temporal bounding box, i.e. the temporal extent that is covered by the PostTIME instances used as arguments. This is an overloaded function, so you can use a single PostTIME instance or also SETs as arguments. It work's for all reference systems and types. *Examples:*

    // Use it with a single instant.
    SELECT pt_temporal_bbox('2013-4-29T12:32');
    // result
    'CAL0012013-04-29T12:32:00.000Z/2013-04-29T12:32:00.000Z'

    // Up to the regular types.
    SELECT pt_temporal_bbox('R12/2013-4-29T8/PT8H30M/PT15H30M');
    // result
    'CAL0012013-04-29T08:00:00.000Z/2013-05-11T16:30:00.000Z'

    // Or aggregate the temporal bbox directly from a table's attribute.
    SELECT pt_temporal_bbox(foo_ptime_attribute) FROM foo_table;
    // result
    'CAL0012012-09-03T16:33:22.212Z/2021-07-01T00:00:00.000Z'

    // And why not use it with ordinal systems?
    SELECT pt_temporal_bbox('ORD001Triassic,Paleozoic,Mesozoic,Neogene');
    // result
    'ORD001Paleozoic/Neogene'

##### pt\_transform\_system( PostTIME , text ) : PostTIME 
Transforms a PostTIME instance into the reference system specified by the given key. *Examples:*

    // UNIX time to gregorian calendar.
    SELECT pt_transform_system('TCS0021', 'CAL001');
    // result
    'CAL0011970-01-01T00:00:01.000Z'

     // or calendar date to julian day
    SELECT pt_transform_system('CAL0012013-4-30T12:20', 'TCS001');
    // result
    'TCS0012456413.013889'

##### pt\_regular\_multi_to\_multi( PostTIME ) : PostTIME 
This function transform's an RegularMultiObject into a normal MultiObject, what means you get concrete instants. *Examples:*

    SELECT pt_regular_multi_to_multi('R3/2012-1-1/P3M/P9M');
    // result:  
    CAL0012012-01-01T00:00:00.000Z/2012-04-01T00:00:00.000Z,
    2013-01-01T00:00:00.000Z/2013-04-01T00:00:00.000Z,
    2014-01-01T00:00:00.000Z/2014-04-01T00:00:00.000Z,
    2015-01-01T00:00:00.000Z/2015-04-01T00:00:00.000Z'

    SELECT pt_regular_multi_to_multi('R3/2012-1-1/P30D');
    // result:
    'CAL0012012-01-01T00:00:00.000Z,2012-01-31T00:00:00.000Z,
    2012-03-01T00:00:00.000Z,2012-03-31T00:00:00.000Z'

    SELECT pt_regular_multi_to_multi('TCS002R2/21224211/P3123321');
    // result
    'TCS00221224211.000000,210891107532.000000,421760990853.000000'

###Doxygen source code documentation
Follow [link](http://141.30.100.164:8080) to the doxygen documentation files of the source code.