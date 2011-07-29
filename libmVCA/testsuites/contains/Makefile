# $Id$
# a libmVCA testsuite Makefile

.PHONY: all clean

include ../../config.mk

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lmVCA

all: contains

clean:
	-rm -f *.o contains *.png *.dot 2>&1

contains: contains.cpp

