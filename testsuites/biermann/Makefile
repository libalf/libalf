# $Id$
# biermann testsuite Makefile

.PHONY: all clean

CPPFLAGS=-Wall -ggdb3  -I../../include

LDFLAGS=-L../../src -lalf -lAMoRE -lm

all: biermann_test

clean:
	-rm *.o
	-rm biermann_test

biermann_test: biermann_test.cpp

