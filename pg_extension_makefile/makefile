PKGNAME = posttime
PKGVERS = 1.0.0
EXTVERS = 1.0.0

EXTENSION = posttime
MODULE_big = posttime
OBJS = ../TimeCoding/PostTIME/blackbox_converter.o ../TimeCoding/PostTIME/pg_interface.o ../TimeCoding/PostTIME/calendars.o ../TimeCoding/PostTIME/coordinate_system.o ../TimeCoding/PostTIME/geotime.o ../TimeCoding/PostTIME/ordinal.o ../TimeCoding/PostTIME/parser.o ../TimeCoding/PostTIME/pt_errors.o ../TimeCoding/PostTIME/textualizator.o
DATA = sql/posttime--1.0.0.sql

PG_CONFIG ?= pg_config
PGXS = $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
