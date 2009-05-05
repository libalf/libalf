# $Id$
# DFArandomgenerator testsuite Makefile

.PHONY: all clean run performance_test nodata jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -llangen -lAMoRE++

all: DFArandomgenerator performance_tester

clean: nodata
	-rm *.o
	-rm DFArandomgenerator performance_tester

nodata:
	-rm *.dot
	-rm *.jpg

run:
	LD_LIBRARY_PATH=../../src ./DFArandomgenerator

performance_test: performance_tester
	LD_LIBRARY_PATH=../../src ./performance_tester

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

