# $Id$
# antichain testsuite Makefile

.PHONY: all clean run jpegs test

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3 -DANTICHAIN_DEBUG

LDFLAGS+=-L../../src -L${LIBDIR} -lAMoRE++ -lalf

all: subseteq_regex subseteq_file massive_antichain

clean: nodata
	-rm -f *.o subseteq_regex subseteq_file massive_antichain

nodata:
	-rm -f *.dot *.jpg *-info *.ser

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

subseteq_regex: subseteq_regex.cpp

subseteq_file: subseteq_file.cpp

massive_antichain: massive_antichain.cpp
massive_antichain: LDFLAGS += -llangen


