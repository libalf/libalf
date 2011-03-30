# $Id$
# ASO testsuite Makefile

.PHONY: all clean run jpegs

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I../../../libAMoRE++/include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L../../../libAMoRE++/src -L${LIBDIR}

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
	g++ ${CPPFLAGS} ${LDFLAGS} learn_regex.cpp -o learn_regex -lAMoRE++ -lalf

learn_file: learn_file.cpp
	g++ ${CPPFLAGS} ${LDFLAGS} learn_file.cpp -o learn_file -lAMoRE++ -lalf
