# $Id$
# ASO testsuite Makefile

.PHONY: all clean run jpegs test

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS += -I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lalf -lAMoRE++

all: learn_regex learn_file

clean: nodata
	-rm -f *.o learn_regex learn_static learn_file 2>&1

nodata:
	-rm -f *.dot *.jpg *.rv testlog *.ser 2>&1

run:
	LD_LIBRARY_PATH=../../src ./learn_regex

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_regex: learn_regex.cpp

learn_file: learn_file.cpp
#learn_file: CPPFLAGS += -DDEBUG_RIVEST_SCHAPIRE

