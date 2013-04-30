SELECT pt_transform_system('2013-12-24','TCS001');
SELECT pt_transform_system('-4713','TCS001');
SELECT pt_transform_system('1970-1-1T0:0:0.001','TCS002');
SELECT pt_transform_system('TCS0021','CAL001');
SELECT pt_transform_system('TCS0021','TCS001');
SELECT pt_transform_system('TCS0011','TCS003');
/* error */
SELECT pt_transform_system('1970-1-1T0:0:0.001','ORD001');
SELECT pt_transform_system('ORD001Triassic','TCS002');
SELECT pt_transform_system('1970-1-1T0:0:0.001','CAL002');
SELECT pt_transform_system('1970-1-1T0:0:0.001','dasdfa');
SELECT pt_transform_system('1970-1-1T0:0:0.001','CALCAL');

