# $Id: Makefile 583 2009-05-05 19:24:10Z davidpiegdon $
# DFArandomgenerator testsuite Makefile

.PHONY: all clean run nodata jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -llangen -lalf

all: DFAenumerator

clean: nodata
	-rm *.o
	-rm DFAenumerator

nodata:
	-rm *.dot
	-rm *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

