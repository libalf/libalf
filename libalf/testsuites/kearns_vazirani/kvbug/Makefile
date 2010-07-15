# $Id: Makefile 1281 2010-03-25 03:13:05Z davidpiegdon $
# ASO testsuite Makefile

.PHONY: all clean run jpegs

CPPFLAGS+=-I../../include -I../../../libAMoRE++/include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L../../../libAMoRE++/src -L${PREFIX}/lib -L${PREFIX}/src -lAMoRE++ -lalf

all: kvbug


kvbug: kvbug.cpp

clean:
	rm kvbug

