# $Id$
# conjecture testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -Wextra -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lalf

all: finite_automaton_from_file

clean: nodata
	-rm -f *.o finite_automaton_from_file

nodata:
	-rm -f *.dot *.jpg *.ser

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

finite_automaton_from_file: finite_automaton_from_file.cpp

