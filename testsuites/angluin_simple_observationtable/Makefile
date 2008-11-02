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

run: aso.jpg
	LD_LIBRARY_PATH=../../src ./aso

aso.jpg: aso
	dot -Tjpg aso.dot -o aso.jpg

aso: aso.cpp

