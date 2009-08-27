# $Id$
# RPNI testsuite Makefile

.PHONY: all clean jpegs

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lm -lalf -lAMoRE++ -llangen

all: rpni_static rpni_regex rpni_file rpni_angluin_tester

nodata:
	-rm *.dot *.jpg *.bs *.txt

clean: nodata
	-rm *.o
	-rm rpni_regex rpni_static rpni_file rpni_angluin_tester

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

rpni_static: CPPFLAGS+=-DRPNI_DEBUG_EQ_CLASSES
rpni_static: rpni_static.cpp

rpni_regex: rpni_regex.cpp

rpni_angluin_tester: rpni_angluin_tester.cpp

rpni_file: CPPFLAGS+=-DRPNI_DEBUG_EQ_CLASSES
rpni_file: rpni_file.cpp

