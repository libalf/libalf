# $Id$
# ASO testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lalf -lAMoRE++

all: learn_regex

clean: nodata
	-rm -f *.o learn_regex learn_static learn_bug learn_file

nodata:
	-rm -f *.dot *.jpg *.angluin testlog

run:
	LD_LIBRARY_PATH=../../src ./learn_regex

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_regex: learn_regex.cpp
	g++ ${CPPFLAGS} ${LDFLAGS} learn_regex.cpp
