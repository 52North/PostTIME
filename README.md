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

For example PostTIME allows to extract the predecessor-relationships from the [Cshape](http://nils.weidmann.ws/projects/cshapes) Dataset, which contains several countries as dynamic features, by the following SQL-Statement on the condition that the object's validtime is stored as PostTIME:

```mysql
SELECT a.cntry_name , b.cntry_name AS predecessor
FROM testdata_cshape AS a
	INNER JOIN testdata_cshape AS b
	ON pt_predecessor(a.the_geom , a.validtime , b.the_geom, b.validtime);
```

The result includes among other rows:
  
```psql
          cntry_name          |         predecessor          
------------------------------+------------------------------
 Germany                      | Germany Federal Republic
 Germany                      | Germany Democratic Republic
```


Cshape: _Weidmann, Nils B., Doreen Kuse, and Kristian Skrede Gleditsch. 2010. The Geography of the International System: The CShapes Dataset. International Interactions 36 (1)._

#Install instructions - Set up as PostgreSQL extension
PostTIME makes use of the PostgreSQL extension feature so you need PostgreSQL version 9.1 or higher. For more information see the [PostgreSQL documentation](http://www.postgresql.org/docs/ "www.postgresql.org/docs/").

To make PostTIME available as an extension module please download the *TimeCoding* and the *pg_extension_makefile* folder in the same directory. Then run `make` and `make install`.

You can run local regression tests of your PostTIME installation with __`make installcheck`__.

Now you can easily add the PostTIME extension to a database with a CREATE EXTENSION statement like:

```mysql
CREATE EXTENSION posttime;
``` 

You can remove the extension with:

```mysql
DROP EXTENSION posttime;
```

If you want to undo the installation just remove all files naming posttime from your PostgreSQL *extension* directory and the shared library object from PostgreSQL's *$libdir*.

#PostTIME SQL functions
##ISO19108 Functions
The following functions are taken over from ISO19108 - for that reason these are only defined for the primitive object types. However, if you call one of these with a multi object you will get 'NAP' if the return type is text or '-1' if the return type is numeric. That means you can call these functions with any PostTIME instance without causing an error.   
##### tm\_relative\_position( PostTIME , PostTIME ) : text 
Determine the relative position of one inctance to another and returns the result as a string according to ISO19108. *Examples:*

```mysql
-- Calendar system...
SELECT tm_relative_position('2013-6-11T9:12','2013-5-30/2016-2-29');
-- result
'During'

-- ...coordinate system...
SELECT tm_relative_position('TCS0028765/9765','TCS0029000/10000');
-- result
'Overlaps'

-- ...or also using an ordinal system.
SELECT tm_relative_position('ORD001Neoproterozoic/Ordovician','ORD001Devonian');
-- result
'Before'
```

The function __tm\_relative\_position\_int( PostTIME , PostTIME ) : integer__ performs the same operation but returns an integer with the matching value from the ISO19108 enumeration type TM_RelativePosition.

##### tm\_distance( PostTIME , PostTIME ) : text 
This function calculates the distance between two primitive objects and return the result als ISO8601-duration-string. If the instances are using a temporal coordinate system the gregrioan calendar's rules will be used for calculation. *Examples*:

```mysql
SELECT tm_distance('2013-5-8T16:14:12.345','2013-5-9T22:21:20.191');
-- result
'P1DT6H7M7.846S'

SELECT tm_distance('TCS0021368029652.345000','TCS0021368138080.191000');
-- result
'P1DT6H7M7.846S'

SELECT tm_distance('2010/2016-2-29T12','2038-1-19T03:14:08');
-- result
'P21Y10M18DT15H14M8.000S'
```

Use instead the __tm\_distance\_dec\_day( PostTIME , PostTIME ) : double precision__ function to get the duration between the two instances in decimal days:

```mysql
SELECT tm_distance_dec_day('2013-5-8T16:14:12.345','2013-5-9T22:21:20.191');
-- result
1.2549519212963

SELECT tm_distance_dec_day('2010-5-12T14:23:42','2010-4-12T14:23:42')
-- result
30
```

The functions __tm\_duration( PostTIME ) : text__ and __tm\_duration\_dec\_day( PostTIME) : double precision__ are quite similar to the distance functions, with the difference that they determine the duration of a single primitive:

```mysql
SELECT tm_duration('2013-5-30T8/2013-5-30T12:45');
-- result
'PT4H45M'
```

##Basic Functions

##### pt\_temporal\_bbox( PostTIME , ... ) : PostTIME 
Calculates the temporal bounding box, i.e. the temporal extent that is covered by the PostTIME instances used as arguments. This is an overloaded function, so you can use a single PostTIME instance or also SETs as arguments. It work's for all reference systems and types. *Examples:*

```mysql
-- Use it with a single instant.
SELECT pt_temporal_bbox('2013-4-29T12:32');
-- result
'CAL0012013-04-29T12:32:00.000Z/2013-04-29T12:32:00.000Z'

-- Up to the regular types.
SELECT pt_temporal_bbox('R12/2013-4-29T8/PT8H30M/PT15H30M');
-- result
'CAL0012013-04-29T08:00:00.000Z/2013-05-11T16:30:00.000Z'

-- Or aggregate the temporal bbox directly from a table's attribute.
SELECT pt_temporal_bbox(foo_ptime_attribute) FROM foo_table;
-- result
'CAL0012012-09-03T16:33:22.212Z/2021-07-01T00:00:00.000Z'

-- And why not use it with ordinal systems?
SELECT pt_temporal_bbox('ORD001Triassic,Paleozoic,Mesozoic,Neogene');
-- result
'ORD001Paleozoic/Neogene'
```

##### pt\_transform\_system( PostTIME , text ) : PostTIME 
Transforms a PostTIME instance into the reference system specified by the given key. *Examples:*

```mysql
-- UNIX time to gregorian calendar.
SELECT pt_transform_system('TCS0021', 'CAL001');
-- result
'CAL0011970-01-01T00:00:01.000Z'

 -- or calendar date to julian day
SELECT pt_transform_system('CAL0012013-4-30T12:20', 'TCS001');
-- result
'TCS0012456413.013889'
```

##### pt\_simultaneous( PostTIME , PostTIME ) : boolean 
Check out if two instances share at least one instant.

```mysql
SELECT pt_simultaneous('2013-5-4T8:2','R10/2013-5-3/PT8H/PT16H');
-- result
f

SELECT pt_simultaneous('2013-5-4T8','R10/2013-5-3/PT8H/PT16H');
-- result
t
```

##### pt\_overlaps( PostTIME , PostTIME ) : boolean 
Similar to simultaneous, but with the difference that it proofs if the two instances share at least one period in time.

```mysql
SELECT pt_overlaps('2013-2-12/2013-2-28','2013-2-12T8');
-- result
f

SELECT pt_overlaps('2013-2-12/2013-2-28','2013-2-12T8/2013-2-12T9');
-- result
t
```

##### pt\_weekday_int( PostTIME ) : integer 
Determine the day of the week of the given instant. Result is an enumeration, beginning with 0 on Monday.

```mysql
SELECT pt_weekday_int('2013-12-24T12');
-- result
1
```

##### pt\_regular\_multi_to\_multi( PostTIME ) : PostTIME 
This function transform's an RegularMultiObject into a normal MultiObject, what means you get concrete instants. *Examples:*

```mysql
SELECT pt_regular_multi_to_multi('R3/2012-1-1/P3M/P9M');
-- result  
'CAL0012012-01-01T00:00:00.000Z/2012-04-01T00:00:00.000Z,
2013-01-01T00:00:00.000Z/2013-04-01T00:00:00.000Z,
2014-01-01T00:00:00.000Z/2014-04-01T00:00:00.000Z,
2015-01-01T00:00:00.000Z/2015-04-01T00:00:00.000Z'

SELECT pt_regular_multi_to_multi('R3/2012-1-1/P30D');
-- result
'CAL0012012-01-01T00:00:00.000Z,2012-01-31T00:00:00.000Z,
2012-03-01T00:00:00.000Z,2012-03-31T00:00:00.000Z'

SELECT pt_regular_multi_to_multi('TCS002R2/21224211/P3123321');
-- result
'TCS00221224211.000000,210891107532.000000,421760990853.000000'
```

##Casts
The PostTIME extension includes casts to and from _date_, _timestamp with time zone_, and _timestamp without time zone_.

A cast from a timestamp will produce an instant in any case and you can only cast into timestamps with instants. A cast to _timestamp with time zone_ will convert the PostTIME instance into your time zone, according to the PostgreSQL settings.

On the contrary casts into _date_ are also possible with periods, on the condition that the period's start and end instants have to belong to the same day. If you cast _date_ to PostTIME you get a period, representing the whole day.

##Spatio-temporal Functions
In general, this functions combine the Basic functions with functions from the PostGIS extension to allow spatio-temporal analysis.

##### pt\_predecessor( geometry, PostTIME, geometry, PostTIME ) : boolean
This function returns true if the two geometries' interior intersects and the [relative position](https://github.com/52North/PostTIME#tm_relative_position-posttime--posttime---text)  from the first PostTIME argument to the second is _MetBy_. In other words it returns true if the second geometry-posttime tuple represents an object that is a predecessor of the object represented by the first tuple. *Examples:*

```mysql
SELECT pt_predecessor( 
'POLYGON((1 1,1 4,4 4,4 1,1 1))',
'2013',
'POLYGON((1 1,1 2,2 2,2 1,1 1))',
'2012');
-- result
 t

SELECT pt_predecessor( 
'POLYGON((1 1,1 4,4 4,4 1,1 1))',
'2012',
'POLYGON((1 1,1 2,2 2,2 1,1 1))',
'2013');
-- result
 f
```

The antagonist function __pt\_successor( geometry, PostTIME, geometry, PostTIME ) : boolean__ is instead true if the relative position is _Meets_.

#Doxygen source code documentation
Follow [link](http://141.30.100.164:8080/files.html) to the doxygen documentation files of the source code. Maybe not up-to-date.
