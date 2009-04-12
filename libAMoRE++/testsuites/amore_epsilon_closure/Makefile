# $Id$
# serialization testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lAMoRE++

all: epsilon_closure

clean: nodata
	-rm *.o
	-rm epsilon_closure

nodata:
	-rm *.dot
	-rm *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

epsilon_closure: epsilon_closure.cpp

