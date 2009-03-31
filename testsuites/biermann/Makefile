# $Id$
# biermann testsuite Makefile

.PHONY: all clean jpegs

CPPFLAGS=-Wall -ggdb3  -I../../include

LDFLAGS=-L../../src -lalf -lAMoRE -lm

all: biermann_static biermann_regex

clean:
	-rm *.o
	-rm biermann_regex biermann_static
	-rm *.dot
	-rm *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

biermann_static: biermann_static.cpp

biermann_regex: biermann_regex.cpp

