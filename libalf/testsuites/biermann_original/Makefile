# $Id$
# biermann testsuite Makefile

.PHONY: all clean jpegs

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lm -lalf -llangen

all: learn_regex learn_generic

nodata:
	-rm -f *.dot *.jpg hypothesis knowledgebase

clean: nodata
	-rm -f *.o learn_regex learn_generic

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_regex: learn_regex.cpp
learn_regex: LDFLAGS += -lAMoRE++

learn_generic: learn_generic.cpp

