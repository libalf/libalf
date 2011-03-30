# $Id$
# dfa_enumerator testsuite Makefile

.PHONY: all clean run nodata jpegs

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -llangen -lalf

all: dfa_enumerator

clean: nodata
	-rm -f *.o dfa_enumerator

nodata:
	-rm -f *.dot *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

