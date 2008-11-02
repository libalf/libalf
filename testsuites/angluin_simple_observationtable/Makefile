# $Id$
# ASO testsuite Makefile

.PHONY: all clean run

CPPFLAGS=-Wall -ggdb3  -I../../include

LDFLAGS=-L../../src -lalf -lAMoRE

all: aso

clean:
	-rm *.o
	-rm aso
	-rm *.dot
	-rm *.jpg
	-rm *.angluin

run:
	LD_LIBRARY_PATH=../../src ./aso

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

aso: aso.cpp

