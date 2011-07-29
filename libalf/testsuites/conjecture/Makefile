# $Id$
# conjecture testsuite Makefile

.PHONY: all clean run jpegs test

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -Wextra -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lalf

all: finite_automaton_from_file

clean: nodata
	-rm -f *.o finite_automaton_from_file 2>&1

nodata:
	-rm -f *.dot *.jpg *.ser 2>&1

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

finite_automaton_from_file: finite_automaton_from_file.cpp

