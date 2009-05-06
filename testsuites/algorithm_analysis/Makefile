# $Id$
# algorithm analysis Makefile

.PHONY: all clean run jpegs test

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lalf -lAMoRE++ -llangen

all: algorithm_analysis

clean: nodata
	-rm *.o
	-rm algorithm_analysis

nodata:
	-rm *.dot
	-rm statistics
	-rm *.jpg

run:
	LD_LIBRARY_PATH=../../src ./learn_regex

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

algorithm_analysis: algorithm_analysis.cpp

