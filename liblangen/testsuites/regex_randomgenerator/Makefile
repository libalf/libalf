# $Id$
# regex_randomgenerator testsuite Makefile

.PHONY: all clean run nodata jpegs

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -llangen -lAMoRE++

all: regex_randomgenerator

clean: nodata
	-rm -f *.o regex_randomgenerator

nodata:
	-rm -f *.dot *.jpg

run:
	LD_LIBRARY_PATH=../../src ./regex_randomgenerator

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

