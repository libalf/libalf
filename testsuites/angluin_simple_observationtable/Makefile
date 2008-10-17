# $Id$
# ASO testsuite Makefile

.PHONY: all clean run

CPPFLAGS=-Wall -ggdb3  -I../../include -I../../libAMoRE-1.0/

LDFLAGS=-L../../src/ -lalf -L../../libAMoRE-1.0/ -lAMoRE

all: aso

clean:
	-rm *.o
	-rm aso

run: aso
	LD_LIBRARY_PATH=../../src ./aso

aso: aso.cpp

