# $Id$
# NL* testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lalf -lAMoRE++ -llangen

all: count_eq_queries count_eq_queries_random count_eq_queries_file

clean: nodata
	-rm -f *.o count_eq_queries count_eq_queries_random count_eq_queries_file

nodata:
	-rm -f *.dot *.jpg *.atm

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

count_eq_queries: count_eq_queries.cpp

count_eq_queries_random: count_eq_queries_random.cpp

count_eq_queries_file: count_eq_queries_file.cpp

