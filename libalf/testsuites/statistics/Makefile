# $Id$
# stat testsuite Makefile

.PHONY: all clean

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lalf

all: stat

clean:
	-rm -f *.o stat

stat: stat.cpp

