# $Id$
# serialization testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lAMoRE++

all: amore_serialization

clean: nodata
	-rm -f *.o amore_serialization

nodata:
	-rm -f *.dot *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

amore_serialization: amore_serialization.cpp

