SELECT pt_transform_system('2013-12-24','TCS001');
SELECT pt_transform_system('-4713','TCS001');
SELECT pt_transform_system('1970-1-1T0:0:0.001','TCS002');
SELECT pt_transform_system('TCS0021','CAL001');
SELECT pt_transform_system('TCS0021','TCS001');
SELECT pt_transform_system('TCS0011','TCS003');

SELECT pt_transform_system('1970-1-1T0:0:0.001','CAL002');
SELECT pt_transform_system('1980-12-1T0:0:0.001','CAL002');
SELECT pt_transform_system('2000-12-31T0:0:0.001','CAL002');
SELECT pt_transform_system('1970-1-1T0:0:0.001/1980-12-1T11:22:33.123,1981-1-1T0:0:0.001/1991-12-1T11:22:33.123','CAL002');
SELECT pt_transform_system('1970-1-1T0:0:0.001,1980-12-1T11:22:33.123,1981-1-1T0:0:0.001,1991-12-1T11:22:33.123','CAL002');
/* regular types */
SELECT pt_transform_system('R10/1970-1-1T0:1/PT1S','TCS002');
SELECT pt_transform_system('TCS002R10/2/P1','CAL001');

/* error */
SELECT pt_transform_system('1970-1-1T0:0:0.001','ORD001');
SELECT pt_transform_system('ORD001Triassic','TCS002');
SELECT pt_transform_system('1970-1-1T0:0:0.001','CAL003');
SELECT pt_transform_system('1970-1-1T0:0:0.001','dasdfa');
SELECT pt_transform_system('1970-1-1T0:0:0.001','CALCAL');

