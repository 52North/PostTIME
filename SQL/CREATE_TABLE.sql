/*************************** TESTDATA *****************************************************************/

CREATE TABLE test4time
(
  id serial NOT NULL,
  validtime posttime,
  CONSTRAINT test4time_pkey PRIMARY KEY (id)
)
WITH (
  OIDS=FALSE
);

INSERT INTO test4time (validtime)
VALUES
('2012-9-3T16:33:22.212'),
('2012-9-17T12'),
('2012-9-17,2012-10-12,2013-4-5');
