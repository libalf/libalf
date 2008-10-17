# $Id$
# ASO testsuite Makefile

.PHONY: all clean run

CPPFLAGS=-Wall -I../../include

LDFLAGS=-L../../src -lalf

all: aso

clean:
	-rm *.o
	-rm aso

run: aso
	LD_LIBRARY_PATH=../../src ./aso

aso: aso.cpp

