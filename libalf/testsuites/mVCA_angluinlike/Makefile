# $Id$
# testsuite Makefile

.PHONY: all clean run jpegs test

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -Wextra -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lalf -lAMoRE++ -lmVCA

all: learn_static

full_test: clean all
	printf "\n\n\n"
	./learn_static

clean: nodata
	-rm -f *.o learn_static 2>&1

nodata:
	-rm -f *.dot *.jpg *.NLstar testlog 2>&1

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_static: learn_static.cpp

