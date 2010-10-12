# $Id$
# testsuite for generic angluin L*
# (learning not just finite_automata but moore_machines with generic alphabet)

.PHONY: all clean nodata jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lalf -lAMoRE++ -lreadline

all: learn_generic

clean: nodata
	-rm -f *.o learn_generic

nodata:
	-rm -f *.dot *.jpg *.angluin *.ser

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_generic: learn_generic.cpp

