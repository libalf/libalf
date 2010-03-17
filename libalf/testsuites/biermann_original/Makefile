# $Id$
# biermann testsuite Makefile

.PHONY: all clean jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lm -lalf -lAMoRE++ -llangen

all: learn_regex

nodata:
	-rm -f *.dot *.jpg

clean: nodata
	-rm -f *.o learn_regex

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_regex: learn_regex.cpp

