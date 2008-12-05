# $Id$
# structured query tree testsuite Makefile

.PHONY: all clean jpegs nodata

CPPFLAGS=-Wall -ggdb3  -I../../include

LDFLAGS=-L../../src -lalf -lAMoRE

all: sqt_serialization sqt_learn_regex_angluin

clean: nodata
	-rm *.o
	-rm sqt_serialization
	-rm sqt_learn_regex_angluin

nodata:
	-rm *.dot
	-rm *.jpg
	-rm *.angluin

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

sqt_serialization: sqt_serialization.cpp

sqt_learn_regex_angluin: sqt_learn_regex_angluin.cpp

