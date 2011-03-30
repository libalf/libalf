# $Id$
# biermann testsuite Makefile

.PHONY: all clean jpegs

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lm -lalf -lAMoRE++ -llangen

all: biermann_static biermann_regex

nodata:
	-rm -f *.dot *.jpg

clean: nodata
	-rm -f *.o biermann_regex biermann_static

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

biermann_static: biermann_static.cpp

biermann_regex: biermann_regex.cpp

