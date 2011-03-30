# $Id$
# a libmVCA testsuite Makefile

.PHONY: all clean

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lmVCA

all: intersect

clean:
	-rm -f *.o intersect *.png *.dot

pngs:
	for EACH in *.dot; do dot -Tpng $$EACH > `echo $$EACH | sed 's/dot$$/png/'`; done

intersect: intersect.cpp

