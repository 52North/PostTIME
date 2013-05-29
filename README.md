# PostTIME

![Logo](logo.png?raw=true)

The PostTIME project intends to enhance PostgreSQL's capabilities to handle the temporal dimension with a focus on processing spatial information. For that purpose it provides:

1. *__6__ different __object types__ to abstract time*
    * Instants and periods of time
    * MultiInstants and MultiPeriods (composition types)
    * RegularMultiInstant and RegularMultiPeriod (to model regular repeating events)
2. *Various __temporal reference systems__ of __3__ different general types*
    * Temporal coordinate systems e.g. UNIX-Time
    * Calendar and clock systems like the Gregorian calendar with UTC
    * Ordinal systems e.g. the geological eras.
3. *Several __SQL functions__ for your __analysis__ tasks*.

The concept is basing on ISO19108. The project is in an __early stage__, so please keep in mind that PostTIME is more or less unstable and anything but complete.

Please see __[the wiki](http://www.github.com/52North/PostTIME/wiki)__ for more information.

There is also a __[doxygen documentation](http://141.30.100.164:8080/files.html)__ of the source code available. Maybe not up-to-date.

#Application Examples

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
* * * 
Cshape: _Weidmann, Nils B., Doreen Kuse, and Kristian Skrede Gleditsch. 2010. The Geography of the International System: The CShapes Dataset. International Interactions 36 (1)._
* * * 

Another powerful tool is the function

__pt\_spatiotemporal\_statistics( *[tablename events]* , *[tablename reference geometry]* , *[ISO8601-duration]* , *[id reference geometry]* ) : TABLE( id , intervals , count )__ 

If you have one table with events, whose geometry is stored as Point with PostGIS's geometry type and the temporal information as
PostTIME, and another table, which you want to use as the reference geometry, you can easily calculate some statistics.
To do so, hand in those tables' names with an interval step size and the reference geometry's id-name and you get in return a table, which contains 
the count of the events for each reference object and interval step, respectively.

To clarify this, we show an example with data from [nationalatlas.gov](http://nationalatlas.gov/). The event dataset should be the construction of airports and the static reference are the USA.
After we have [cast](https://github.com/52North/PostTIME#casts) the temporal information into PostTIME, we can call the function:

```mysql
SELECT * FROM  pt_spatiotemporal_statistics('testdata_airports' , 'testdata_states' , 'P10Y', 'id' );
```

And we get:

```psql
 id |                          ptime                          | count 
----+---------------------------------------------------------+-------
  5 | CAL0011928-01-01T00:00:00.000Z/1938-01-01T00:00:00.000Z |     4
  6 | CAL0011928-01-01T00:00:00.000Z/1938-01-01T00:00:00.000Z |     1
 15 | CAL0011928-01-01T00:00:00.000Z/1938-01-01T00:00:00.000Z |     4
 22 | CAL0011928-01-01T00:00:00.000Z/1938-01-01T00:00:00.000Z |     2
 ...
  1 | CAL0011938-01-01T00:00:00.000Z/1948-01-01T00:00:00.000Z |     7
  2 | CAL0011938-01-01T00:00:00.000Z/1948-01-01T00:00:00.000Z |     9
  3 | CAL0011938-01-01T00:00:00.000Z/1948-01-01T00:00:00.000Z |     8
  4 | CAL0011938-01-01T00:00:00.000Z/1948-01-01T00:00:00.000Z |    11
  ...
```

We can adjust the interval step size:
```mysql
SELECT * FROM pt_spatiotemporal_statistics('testdata_airports' , 'testdata_states' , 'P1Y6M', 'id' );
``` 
please note that the result contains only id / ptime matches which have a count of at least 1:
```psql
 id |                          ptime                          | count 
----+---------------------------------------------------------+-------
  6 | CAL0011928-04-01T00:00:00.000Z/1929-10-01T00:00:00.000Z |     1
 41 | CAL0011929-10-01T00:00:00.000Z/1931-04-01T00:00:00.000Z |     1
 41 | CAL0011931-04-01T00:00:00.000Z/1932-10-01T00:00:00.000Z |     1
  5 | CAL0011937-04-01T00:00:00.000Z/1938-10-01T00:00:00.000Z |     4
  ...
```

The id can be used for a JOIN with the original reference geometry:
```mysql
SELECT a.* , b.state 
FROM pt_spatiotemporal_statistics('testdata_airports' , 'testdata_states' , 'P20Y', 'id' ) As a 
JOIN testdata_states As b ON a.id = b.id;
``` 
```psql
 id |                          ptime                          | count |        state        
----+---------------------------------------------------------+-------+---------------------
  1 | CAL0011928-01-01T00:00:00.000Z/1948-01-01T00:00:00.000Z |     7 | Alabama
  2 | CAL0011928-01-01T00:00:00.000Z/1948-01-01T00:00:00.000Z |     9 | Indiana
  ...
``` 
The table contains a lot of information, so it can be used as a basis for various analysis, e.g. let us extract only the maximum count:
```mysql
SELECT a.* , b.state 
FROM pt_spatiotemporal_statistics('testdata_airports' , 'testdata_states' , 'P5Y', 'id' ) As a 
JOIN testdata_states As b ON a.id = b.id 
ORDER BY count DESC LIMIT 1;
``` 
```psql
 id |                          ptime                          | count | state  
----+---------------------------------------------------------+-------+--------
 10 | CAL0011948-01-01T00:00:00.000Z/1953-01-01T00:00:00.000Z |    37 | Alaska
```

