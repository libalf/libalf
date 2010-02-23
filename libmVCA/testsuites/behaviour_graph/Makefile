# $Id: Makefile 1161 2010-02-16 02:40:15Z davidpiegdon $
# a libmVCA testsuite Makefile

.PHONY: all clean

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lmVCA -lAMoRE++

all: behaviour_graph

clean:
	-rm -f *.o behaviour_graph *.png *.dot

pngs:
	for EACH in *.dot; do dot -Tpng $$EACH > `echo $$EACH | sed 's/dot$$/png/'`; done

behaviour_graph: behaviour_graph.cpp

