# $Id: Makefile 1161 2010-02-16 02:40:15Z davidpiegdon $
# a libmVCA testsuite Makefile

.PHONY: all clean

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lmVCA

all: intersect

clean:
	-rm -f *.o intersect *.png *.dot

pngs:
	for EACH in *.dot; do dot -Tpng $$EACH > `echo $$EACH | sed 's/dot$$/png/'`; done

intersect: intersect.cpp

