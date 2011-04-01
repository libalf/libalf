# $Id$
# nfa_randomgenerator testsuite Makefile

.PHONY: all clean run nodata jpegs

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -llangen -lAMoRE++

all: nfa_randomgenerator

clean: nodata
	-rm -f *.o nfa_randomgenerator

nodata:
	-rm -f *.dot *.jpg

run:
	LD_LIBRARY_PATH=../../src ./nfa_randomgenerator

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

