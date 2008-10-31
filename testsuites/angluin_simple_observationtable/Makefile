# $Id$
# ASO testsuite Makefile

.PHONY: all clean run

CPPFLAGS=-Wall -ggdb3  -I../../include

LDFLAGS=-L../../src -lalf -lAMoRE

all: aso

clean:
	-rm *.o
	-rm aso
	-rm aso.dot
	-rm aso.jpg

run: aso
	LD_LIBRARY_PATH=../../src ./aso
	dot -Tjpg aso.dot -o aso.jpg

aso: aso.cpp

