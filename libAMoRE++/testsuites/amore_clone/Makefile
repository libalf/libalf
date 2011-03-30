# $Id$
# clone testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lAMoRE++

all: amore_clone

clean: nodata
	-rm -f *.o amore_clone

nodata:
	-rm -f *.dot *.jpg testlog

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

amore_clone: amore_clone

