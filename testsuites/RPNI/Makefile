# $Id$
# RPNI testsuite Makefile

.PHONY: all clean jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lm -lalf -lAMoRE++ -llangen

all: rpni_static rpni_regex

clean:
	-rm *.o
	-rm rpni_regex rpni_static
	-rm *.dot
	-rm *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

rpni_static: rpni_static.cpp

rpni_regex: rpni_regex.cpp

