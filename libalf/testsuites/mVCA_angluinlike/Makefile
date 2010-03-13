# $Id$
# testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lalf -lAMoRE++ -lmVCA

all: learn_static

full_test: clean all
	printf "\n\n\n"
	./learn_static

clean: nodata
	-rm -f *.o learn_static

nodata:
	-rm -f *.dot *.jpg *.NLstar testlog

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_static: learn_static.cpp

