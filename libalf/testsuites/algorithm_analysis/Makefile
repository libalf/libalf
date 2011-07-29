# $Id$
# algorithm analysis Makefile

.PHONY: all clean run jpegs test

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

# -lrt needed for clock_gettime()
LDFLAGS+=-L../../src -L${LIBDIR} -lalf -lAMoRE++ -llangen -lrt

all: algorithm_analysis

clean: nodata
	-rm -f *.o algorithm_analysis 2>&1

nodata: noresults
	-rm -f *.dot statistics 2>&1

noresults:
	-rm -f *.jpg split_data_a2m0 split_data_a2m1 split_data_a2m2 split_data_a3m0 split_data_a3m1 split_data_a3m2 split_data_a4m0 split_data_a4m1 split_data_a4m2 2>&1

split_data:
	gawk '/^[^ ]+ 2 0/ { print $0 }' statistics > split_data_a2m0
	gawk '/^[^ ]+ 2 1/ { print $0 }' statistics > split_data_a2m1
	gawk '/^[^ ]+ 2 2/ { print $0 }' statistics > split_data_a2m2
	gawk '/^[^ ]+ 3 0/ { print $0 }' statistics > split_data_a3m0
	gawk '/^[^ ]+ 3 1/ { print $0 }' statistics > split_data_a3m1
	gawk '/^[^ ]+ 3 2/ { print $0 }' statistics > split_data_a3m2
	gawk '/^[^ ]+ 4 0/ { print $0 }' statistics > split_data_a4m0
	gawk '/^[^ ]+ 4 1/ { print $0 }' statistics > split_data_a4m1
	gawk '/^[^ ]+ 4 2/ { print $0 }' statistics > split_data_a4m2

plot: split_data
	gnuplot plot.conf

run:
	LD_LIBRARY_PATH=../../src ./learn_regex

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

algorithm_analysis: algorithm_analysis.cpp

