# $Id$
# structured query tree testsuite Makefile

.PHONY: all clean jpegs nodata

CPPFLAGS=-Wall -ggdb3  -I../../include

LDFLAGS=-L../../src -lalf -lAMoRE

all: sqt_serialization

clean: nodata
	-rm *.o
	-rm sqt_serialization

nodata:
	-rm *.dot
	-rm *.jpg
	-rm *.angluin

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

sqt_serialization: sqt_serialization.cpp

