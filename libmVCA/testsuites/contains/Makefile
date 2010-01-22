# $Id$
# a libmVCA testsuite Makefile

.PHONY: all clean

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lmVCA

all: contains

clean:
	-rm -f *.o contains

contains: contains.cpp

