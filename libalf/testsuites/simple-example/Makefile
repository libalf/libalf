# $Id$
# ASO testsuite Makefile

.PHONY: all clean run jpegs test

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lalf -lAMoRE++

all: learn_regex

clean: nodata
	-rm -f *.o learn_regex

nodata:
	-rm -f *.dot *.jpg

run:
	LD_LIBRARY_PATH=../../src ./learn_regex

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_regex: learn_regex.cpp

