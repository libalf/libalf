# $Id$
# biermann testsuite Makefile

.PHONY: all clean jpegs

CPPFLAGS=-Wall -ggdb3  -I../../include

LDFLAGS=-L../../src -lalf -lAMoRE -lm

all: biermann_test

clean:
	-rm *.o
	-rm biermann_test
	-rm *.dot
	-rm *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

biermann_test: biermann_test.cpp

