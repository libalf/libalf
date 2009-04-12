# $Id$
# serialization testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lAMoRE++

all: amore_serialization

clean: nodata
	-rm *.o
	-rm amore_serialization

nodata:
	-rm *.dot
	-rm *.jpg
	-rm *.angluin
	-rm testlog

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

amore_serialization: amore_serialization.cpp

