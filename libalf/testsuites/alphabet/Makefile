# $Id$
# basic alphabet testsuite Makefile

.PHONY: all clean

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lalf -lAMoRE++ -llangen

all: graded_inc_dec

clean:
	-rm -f *.o graded_inc_dec 2>&1

graded_inc_dec: graded_inc_dec.cpp

