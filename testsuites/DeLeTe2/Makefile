# $Id$
# DeLeTe2 testsuite Makefile

.PHONY: all clean jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lm -lalf -lAMoRE++ -llangen

all: delete2_static delete2_regex

clean:
	-rm *.o
	-rm delete2_regex delete2_static
	-rm *.dot
	-rm *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

delete2_static: delete2_static.cpp

delete2_regex: delete2_regex.cpp

