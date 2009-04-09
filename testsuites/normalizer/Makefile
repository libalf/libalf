# $Id$
# normalizer testsuite Makefile

.PHONY: all clean

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lalf

all: normalizer

clean:
	-rm *.o
	-rm normalizer

normalizer: normalizer.cpp

