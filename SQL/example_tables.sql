/*************************** TESTDATA *****************************************************************/

CREATE TABLE test4geotime
(
  id serial NOT NULL,
  the_geotime geotime,
  another_geotime geotime,
  CONSTRAINT test4geotime_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);

CREATE TABLE build_in_datetypes
(
  id serial NOT NULL,
  datetype date,
  timestamp_wo timestamp without time zone,
  timestamp_w timestamp with time zone,
  CONSTRAINT build_in_datetypes_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);

/* data for early&simple tests - the testdata tables contain 'real' data */
INSERT INTO test4geotime (the_geotime, another_geotime)
VALUES 
('2012-9-3T16:33:22.212','2012-9-17T12'),
('2009-7-1/2009-10-1T12','2009-10-1/2012-10-1'),
('CALJUL2001-1-1,2002-1-1,2003-1-1','CALJUL2001-2-1'),
('TCS0012456157.0157','TCS0012456156.09/2456157.07553'),
('TCS0012456157.07553','TCS0012456156.09'),
/* ('ORDGEOCretaceous','ORDGEOPaleocene'),
('ORDGEOTriassic','ORDGEOPaleocene'),*/
('2012-5-14T19:27:22.2','2012-7-14T19:27:22.2'),
('2012-8-14T19:27:22.2','2012-7-14T19:27:22.2'),
('2012-7-14T19:27:22.2','2012-7-14T20:27:22.2+1'),
('2012-7-10T10:00/2012-9-17T12:12:12.122,2012-7-10T10:00/2012-9-17T12:12:12.122','2012-7-10T10:00/2012-9-17T12:12:12.122'),
('2010-3-12T14:23:42,2010-4-12T14:23:42', '2010-4-12T14:23:42'), 
('2010-4-12T14:23:42','2010-4-12T14:23:42'),
('2010-5-12T14:23:42','2010-4-12T14:23:42'),
('2010-4-12T14:23:42/2010-4-13T14:23:42','2010-5-12T14:23:42'), 
('2010-4-12T14:23:42/2010-4-13T14:23:42','2010-4-13T14:23:42'), 
('2010-4-12T14:23:42/2010-4-13T14:23:42','2010-4-12T17:23:42'), 
('2010-4-12T14:23:42/2010-4-13T14:23:42','2010-4-12T14:23:42'), 
('2010-4-12T14:23:42/2010-4-13T14:23:42','2009-5-12T14:23:42'), 
('1996-5-12T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1997-4-12T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('2005-5-12T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('2010-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('2012-5-12T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1994-4-12T14:23:42/1996-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1994-4-12T14:23:42/1997-4-12T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1994-4-12T14:23:42/2005-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1997-4-12T14:23:42/1999-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1997-4-12T14:23:42/2012-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1999-4-12T14:23:42/2001-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1994-4-12T14:23:42/2012-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1997-4-12T14:23:42/2010-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1998-4-12T14:23:42/2012-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1999-4-12T14:23:42/2010-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('1994-4-12T14:23:42/2010-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('2010-4-13T14:23:42/2012-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'), 
('2012-4-13T14:23:42/5454-4-13T14:23:42','1997-4-12T14:23:42/2010-4-13T14:23:42'),
('2002-12-30T12:20:30.456/P2Y3M4DT5H6M7.89S','2002-11-29T12:20:30.123/P1Y/P2Y3M4DT5H6M7.89S'),
('R8/2002-12-30T12:20:30.456/P2Y3M4DT5H6M7.89S','R8/2002-11-29T12:20:30.123/P1Y2M2D/P2Y3M4DT5H6M'),
('2002-12-30T12:20:30.456/P2Y3M4DT5H6M7.89S','2002-11-29T12:20:30.123/P2Y3M/P2Y3M4DT5H6M'), 
('TCS0012111222.345678','TCS0012111223.345678'),
('TCS0012111222.345678,2111223.345678,2111224.345678','TCS0012111223.345678/2111225.345678,2111226.345678/2111227.345678'),
('2000,2001,2002,2003,2004,2005,2006,2007,2008,2009,2010,2011,2012,2013,2014,2015,2016,2017,2018,2019,2020','3000'); 
INSERT INTO build_in_datetypes (datetype, timestamp_wo, timestamp_w)
VALUES ('2011-12-31','2011-12-31','2011-12-31'),
('2012-01-01','2012-1-1','2012-1-1'),
('2012-01-02','2012-01-02 12:00:00.001','2012-01-02 12:00:00.001-1'),
('2012-01-03','2012-01-02 12:00:02.002','2012-01-02 12:00:00.001+1'),
('2000-01-01','2012-01-02 12:00:03.003','2012-01-02 12:00:00.001+2:30'),
('1587-01-01','1587-01-02 12:00:03.003','2012-01-02 12:00:00.001-1:30');

CREATE VIEW v_relative_position AS SELECT the_geotime AS timeobject1, another_geotime AS timeobject2, tm_relative_position(the_geotime,another_geotime)::text AS string, tm_relative_position_integer(the_geotime,another_geotime) AS code FROM test4geotime;
CREATE VIEW v_duration AS SELECT the_geotime AS timeobject1, tm_duration_julday(the_geotime)::double precision AS duration_value, tm_duration_string(the_geotime)::text AS duration_ISO8601_string FROM test4geotime;
CREATE VIEW v_distance AS SELECT the_geotime AS timeobject1, another_geotime AS timeobject2, tm_distance_julday(the_geotime,another_geotime)::double precision AS distance_value, tm_distance_string(the_geotime,another_geotime)::text AS distance_ISO8601_string FROM test4geotime;
CREATE VIEW v_build_in_datetypes_casts AS SELECT datetype::geotime, timestamp_wo::geotime, timestamp_w::geotime FROM build_in_datetypes;

CREATE TABLE ex_get_pre_and_succesors AS
SELECT id As ID,
	cntry_name As country_name,
	ARRAY(SELECT cntry_name FROM testdata_cshape JOIN get_predecessor(id, 'testdata_cshape') As f ON id = f) AS predeccors,
	ARRAY(SELECT cntry_name FROM testdata_cshape JOIN get_successor(id, 'testdata_cshape') As f ON id = f) AS successor,
	validtime As validtime,
	the_geom As the_geom
	FROM testdata_cshape;

CREATE TABLE ex_when_where_maximum AS
SELECT * FROM when_where_maximum(20,'testdata_airports','testdata_states');

CREATE TABLE ex_when_maximum AS
SELECT * FROM when_maximum(20, 'testdata_airports');



/*
CREATE TABLE ex_when_where_maximum2 AS
SELECT * FROM when_where_maximum(10,'testdata_tornados','testdata_states');

CREATE TABLE ex_when_maximum_tornados AS
SELECT * FROM when_maximum(20, 'testdata_tornados');

 */