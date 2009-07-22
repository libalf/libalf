# $Id$
# biermann testsuite Makefile

.PHONY: all clean jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lm -lalf -lAMoRE++ -llangen

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

