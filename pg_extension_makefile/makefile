PKGNAME = posttime
EXTVERS = 1.0.0

EXTENSION = posttime
MODULE_big = posttime
OBJS = ../TimeCoding/PostTIME/temporal_bbox.o ../TimeCoding/PostTIME/blackbox_converter.o ../TimeCoding/PostTIME/pg_interface.o ../TimeCoding/PostTIME/calendars.o ../TimeCoding/PostTIME/coordinate_system.o ../TimeCoding/PostTIME/geotime.o ../TimeCoding/PostTIME/ordinal.o ../TimeCoding/PostTIME/parser.o ../TimeCoding/PostTIME/pt_errors.o ../TimeCoding/PostTIME/textualizator.o ../TimeCoding/PostTIME/relative_position.o ../TimeCoding/PostTIME/distance_length.o ../TimeCoding/PostTIME/pg_interface_casts.o ../TimeCoding/PostTIME/pg_interface_spatiotemporal.o ../TimeCoding/PostTIME/basic_statistics.o
DATA = posttime--1.0.0.sql

REGRESS = simpleio regular_multi_to_multi temporal_bbox transform_system relative_position distance simultaneous casts predecessor_successor basic_statistics

PG_CONFIG ?= pg_config
PGXS = $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
