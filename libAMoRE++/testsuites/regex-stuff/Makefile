# $Id$
# regex testsuite Makefile

.PHONY: all clean

include ../../config.mk
CXXFLAGS+=-Xlinker -rpath=${LIBDIR}

CPPFLAGS+=-I../../include -I${INCLUDEDIR} -Wall -ggdb3

LDFLAGS+=-L../../src -L${LIBDIR} -lAMoRE++

all: regex_stuff

regex_stuff: regex_stuff

clean:
	-rm regex_stuff

