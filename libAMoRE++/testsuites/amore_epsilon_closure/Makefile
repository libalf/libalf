# $Id$
# serialization testsuite Makefile

.PHONY: all clean run jpegs test

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lAMoRE++

all: epsilon_closure

clean: nodata
	-rm -f *.o epsilon_closure 2>&1

nodata:
	-rm -f *.dot *.jpg 2>&1

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

epsilon_closure: epsilon_closure.cpp

