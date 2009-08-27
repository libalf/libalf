# $Id$
# NL* testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lalf -lAMoRE++ -llangen

all: count_eq_queries count_eq_queries_random

clean: nodata
	-rm *.o
	-rm count_eq_queries
	-rm count_eq_queries_random

nodata:
	-rm *.dot
	-rm *.jpg
	-rm *.atm

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

count_eq_queries: count_eq_queries.cpp

count_eq_queries_random: count_eq_queries_random.cpp

