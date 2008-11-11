# $Id$
# serialization testsuite Makefile

.PHONY: all clean run jpegs test

CPPFLAGS=-Wall -ggdb3  -I../../include

LDFLAGS=-L../../src -lalf -lAMoRE

all: amore_serialization

clean: nodata
	-rm *.o
	-rm amore_serialization

nodata:
	-rm *.dot
	-rm *.jpg
	-rm *.angluin
	-rm testlog

run:
	LD_LIBRARY_PATH=../../src ./learn_regex

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

amore_serialization: amore_serialization.cpp

