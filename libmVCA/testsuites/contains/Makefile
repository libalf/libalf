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

all: contains

clean:
	-rm -f *.o contains *.png *.dot

contains: contains.cpp

