# $Id$
# a libmVCA testsuite Makefile

.PHONY: all clean

include ../../config.mk

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lmVCA

all: intersect

clean:
	-rm -f *.o intersect *.png *.dot 2>&1

pngs:
	for EACH in *.dot; do dot -Tpng $$EACH > `echo $$EACH | sed 's/dot$$/png/'`; done

intersect: intersect.cpp

