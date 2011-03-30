# $Id$
# basic alphabet testsuite Makefile

.PHONY: all clean

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lalf -lAMoRE++ -llangen

all: graded_inc_dec

clean:
	-rm -f *.o graded_inc_dec

graded_inc_dec: graded_inc_dec.cpp

