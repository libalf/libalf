# $Id$
# normalizer testsuite Makefile

.PHONY: all clean

CPPFLAGS=-Wall -ggdb3  -I../../include

LDFLAGS=-L../../src -lalf

all: normalizer

clean:
	-rm *.o
	-rm normalizer

normalizer: normalizer.cpp

