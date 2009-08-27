# $Id$
# basic alphabet testsuite Makefile

.PHONY: all clean

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lalf -lAMoRE++ -llangen

all: graded_inc_dec

clean:
	-rm -f *.o graded_inc_dec

graded_inc_dec: graded_inc_dec.cpp

