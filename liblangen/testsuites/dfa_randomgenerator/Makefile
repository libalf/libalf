# $Id$
# dfa_randomgenerator testsuite Makefile

.PHONY: all clean run performance_test nodata jpegs

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -llangen -lAMoRE++

all: dfa_randomgenerator performance_tester

clean: nodata
	-rm -f *.o dfa_randomgenerator performance_tester 2>&1

nodata:
	-rm -f *.dot *.jpg 2>&1

run:
	LD_LIBRARY_PATH=../../src ./dfa_randomgenerator

performance_test: performance_tester
	LD_LIBRARY_PATH=../../src ./performance_tester

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

