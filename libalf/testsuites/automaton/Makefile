# $Id$
# basic automaton testsuite Makefile

.PHONY: all clean jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lalf -lAMoRE++ -llangen

all: automaton

clean: nodata
	-rm *.o
	-rm automaton

nodata:
	-rm *.dot
	-rm *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

automaton: automaton.cpp

