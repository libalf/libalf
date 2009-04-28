# $Id$
# DFArandomgenerator testsuite Makefile

.PHONY: all clean run nodata jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -llangen -lgmpxx -lgmp -lAMoRE++

all: DFArandomgenerator

clean: nodata
	-rm *.o
	-rm DFArandomgenerator

nodata:
	-rm *.dot
	-rm *.jpg

run:
	LD_LIBRARY_PATH=../../src ./DFArandomgenerator

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

DFArandomgenerator: DFArandomgenerator.cpp

