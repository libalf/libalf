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
	-rm statistics stats_a2m0 stats_a2m1 stats_a2m2 stats_a3m0 stats_a3m1 stats_a3m2 stats_a4m0 stats_a4m1 stats_a4m2
	-rm *.jpg

plot:
	gawk '/^[^ ]+ 2 0/ { print $0 }' statistics > stats_a2m0
	gawk '/^[^ ]+ 2 1/ { print $0 }' statistics > stats_a2m1
	gawk '/^[^ ]+ 2 2/ { print $0 }' statistics > stats_a2m2
	gawk '/^[^ ]+ 3 0/ { print $0 }' statistics > stats_a3m0
	gawk '/^[^ ]+ 3 1/ { print $0 }' statistics > stats_a3m1
	gawk '/^[^ ]+ 3 2/ { print $0 }' statistics > stats_a3m2
	gawk '/^[^ ]+ 4 0/ { print $0 }' statistics > stats_a4m0
	gawk '/^[^ ]+ 4 1/ { print $0 }' statistics > stats_a4m1
	gawk '/^[^ ]+ 4 2/ { print $0 }' statistics > stats_a4m2
	gnuplot plot.conf

run:
	LD_LIBRARY_PATH=../../src ./learn_regex

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

algorithm_analysis: algorithm_analysis.cpp

