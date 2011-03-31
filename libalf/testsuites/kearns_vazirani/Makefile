# $Id$
# ASO testsuite Makefile

.PHONY: all clean run jpegs

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I../../../libAMoRE++/include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L../../../libAMoRE++/src -L${LIBDIR} -lAMoRE++ -lalf

all: learn_regex learn_file

clean: nodata
	-rm -f *.o learn_regex learn_file

nodata:
	-rm -f *.dot *.jpg *.KVtree testlog *.kv *.ser

run:
	LD_LIBRARY_PATH="../../src:../../../libAMoRE:../../../libAMoRE++/src" ./learn_regex

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_regex: learn_regex.cpp

learn_file: learn_file.cpp

