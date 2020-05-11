# $Id$
# testsuite for generic angluin L*
# (learning not just finite_automata but moore_machines with generic alphabet)

.PHONY: all clean nodata jpegs learn_generic

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lalf

all: offline_DeLeTe2 offline_RPNI

clean: nodata
	-rm -f *.o offline_DeLeTe2 offline_RPNI 2>&1

