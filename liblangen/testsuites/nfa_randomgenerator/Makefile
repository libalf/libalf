# $Id$
# nfa_randomgenerator testsuite Makefile

.PHONY: all clean run nodata jpegs

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -llangen -lAMoRE++

all: nfa_randomgenerator

clean: nodata
	-rm -f *.o nfa_randomgenerator 2>&1

nodata:
	-rm -f *.dot *.jpg 2>&1

run:
	LD_LIBRARY_PATH=../../src ./nfa_randomgenerator

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

