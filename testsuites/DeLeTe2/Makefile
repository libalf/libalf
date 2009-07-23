# $Id$
# DeLeTe2 testsuite Makefile

.PHONY: all clean jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lm -lalf -lAMoRE++ -llangen

all: delete2_static delete2_regex

nodata:
	-rm *.dot *.jpg

clean: nodata
	-rm *.o
	-rm rpni_regex rpni_static

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

delete2_static: delete2_static.cpp

delete2_regex: delete2_regex.cpp

