SELECT predecessor( 
'POLYGON((1 1,1 20,20 20,20 1,1 1))',
posttime_in('2011/2014'),
'POLYGON((3 3,0 10,10 10,10 3,3 3))',
posttime_in('2015/2016'));

SELECT predecessor( 
'POLYGON((1 1,1 20,20 20,20 1,1 1))',
posttime_in('2013/2014'),
'POLYGON((1 1,5 10,20 20,20 1,1 1))',
posttime_in('2010/2012'));

SELECT predecessor( 
'POLYGON((1 1,1 20,20 20,20 1,1 1))',
posttime_in('2012/2014'),
'POLYGON((1 1,5 10,20 20,20 1,1 1))',
posttime_in('2015/2016'));

SELECT successor( 
'POLYGON((1 1,1 20,20 20,20 1,1 1))',
posttime_in('2011/2014'),
'POLYGON((3 3,0 10,10 10,10 3,3 3))',
posttime_in('2008/2010'));

SELECT successor( 
'POLYGON((1 1,1 20,20 20,20 1,1 1))',
posttime_in('2013/2014'),
'POLYGON((1 1,5 10,20 20,20 1,1 1))',
posttime_in('2010/2012'));

SELECT successor( 
'POLYGON((1 1,1 20,20 20,20 1,1 1))',
posttime_in('2012/2014'),
'POLYGON((1 1,5 10,20 20,20 1,1 1))',
posttime_in('2015/2016'));