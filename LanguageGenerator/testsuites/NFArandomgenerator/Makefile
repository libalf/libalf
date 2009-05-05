# $Id$
# NFArandomgenerator testsuite Makefile

.PHONY: all clean run nodata jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -llangen -lAMoRE++

all: NFArandomgenerator

clean: nodata
	-rm *.o
	-rm NFArandomgenerator

nodata:
	-rm *.dot
	-rm *.jpg

run:
	LD_LIBRARY_PATH=../../src ./NFArandomgenerator

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

