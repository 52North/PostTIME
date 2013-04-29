CREATE EXTENSION posttime;

SELECT '2012'::posttime;
SELECT '2013/2014/2016'::posttime;

CREATE TABLE foo_table_from_regression(id serial);
