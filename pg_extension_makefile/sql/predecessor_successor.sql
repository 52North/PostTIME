SELECT pt_predecessor( 
'POLYGON((1 1,1 4,4 4,4 1,1 1))',
'2012',
'POLYGON((1 1,1 2,2 2,2 1,1 1))',
'2013');

SELECT pt_predecessor( 
'POLYGON((1 1,1 4,4 4,4 1,1 1))',
'2013',
'POLYGON((1 1,1 2,2 2,2 1,1 1))',
'2012');

SELECT pt_predecessor( 
'POLYGON((1 1,1 20,20 20,20 1,1 1))',
'2012/2014',
'POLYGON((1 1,5 10,20 20,20 1,1 1))',
'2015/2016');

SELECT pt_successor( 
'POLYGON((1 1,1 20,20 20,20 1,1 1))',
'2011/2014',
'POLYGON((3 3,0 10,10 10,10 3,3 3))',
'2008/2010');

SELECT pt_successor( 
'POLYGON((1 1,1 20,20 20,20 1,1 1))',
'2013/2014',
'POLYGON((1 1,5 10,20 20,20 1,1 1))',
'2010/2012');

SELECT pt_successor( 
'POLYGON((1 1,1 20,20 20,20 1,1 1))',
'2012/2014',
'POLYGON((1 1,5 10,20 20,20 1,1 1))',
'2015/2016');