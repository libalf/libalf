# $Id$
# a libmVCA testsuite Makefile

.PHONY: all clean

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lmVCA -lAMoRE++

all: behaviour_graph

clean:
	-rm -f *.o behaviour_graph *.png *.dot

pngs:
	for EACH in *.dot; do dot -Tpng $$EACH > `echo $$EACH | sed 's/dot$$/png/'`; done

behaviour_graph: behaviour_graph.cpp

