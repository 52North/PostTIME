SELECT (('2013-5-11')::timestamp without time zone)::posttime;
SELECT (('2013-5-11 00:00:00z')::timestamp with time zone)::posttime;
SELECT (('2013-5-11 12:20')::timestamp without time zone)::posttime;
SELECT (('2013-5-11T12')::posttime)::timestamp without time zone;
SELECT (('2013-5-11T12')::posttime)::timestamp with time zone;
SELECT (('2013-5-11')::date)::posttime;
SELECT (('2013-7-11')::date)::posttime;
SELECT (('2013-5-11T12:30')::posttime)::date;
SELECT (('2013-7-11T12')::posttime)::date;
SELECT (('2013-7-11')::posttime)::date;
SELECT (('2013-7-11T00:00:00/2013-7-12T00:00:00')::posttime)::date;



SELECT (('2013-7-11T00:00:00/2013-7-12T00:00:00.001')::posttime)::date;
SELECT (('2013-7')::posttime)::date;
SELECT (('2013-5-11')::posttime)::timestamp without time zone;
SELECT (('ORD001Triassic')::posttime)::timestamp with time zone;