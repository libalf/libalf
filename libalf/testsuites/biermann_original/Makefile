# $Id$
# biermann testsuite Makefile

.PHONY: all clean jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lm -lalf -llangen

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

