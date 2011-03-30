# $Id$
# regex testsuite Makefile

.PHONY: all clean

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lAMoRE++

all: regex_stuff

regex_stuff: regex_stuff

clean:
	-rm regex_stuff

