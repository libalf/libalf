# $Id$
# clone testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lAMoRE++

all: amore_clone

clean: nodata
	-rm *.o
	-rm amore_clone

nodata:
	-rm *.dot
	-rm *.jpg
	-rm *.angluin
	-rm testlog

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

amore_clone: amore_clone

