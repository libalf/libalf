# $Id: Makefile 583 2009-05-05 19:24:10Z davidpiegdon $
# dfa_enumerator testsuite Makefile

.PHONY: all clean run nodata jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -llangen -lalf

all: dfa_enumerator

clean: nodata
	-rm -f *.o dfa_enumerator

nodata:
	-rm -f *.dot *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

