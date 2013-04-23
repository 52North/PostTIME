PKGNAME = posttime
PKGVERS = 1.0.0
EXTVERS = 1.0.0

EXTENSION = posttime
MODULE_big = posttime
OBJS = src/blackbox_converter.o src/pg_interface.o src/calendars.o src/coordinate_system.o src/geotime.o src/ordinal.o src/parser.o src/pt_errors.o src/textualizator.o
DATA = sql/posttime--1.0.0.sql

PG_CONFIG ?= pg_config
PGXS = $(shell $(PG_CONFIG) --pgxs)
include $(PGXS)
