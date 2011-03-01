# $Id$
# clone testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lAMoRE++

all: regex_stuff

clean: nodata
	-rm -f *.o regex_stuff

nodata:
	-rm -f *.dot *.jpg testlog

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

regex_stuff: regex_stuff

