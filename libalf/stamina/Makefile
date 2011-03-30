# $Id$
# Makefile of libalf stamina interface

.PHONY: all clean

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

CPPFLAGS+=-I../include
CPPFLAGS+=-Wall -Wextra -ggdb3
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
CPPFLAGS+=-DVERBOSE

LDFLAGS+=-L../src -lalf

all:	stamina

stamina: stamina.cpp stamina.h
	g++ ${CPPFLAGS} ${LDFLAGS} stamina.cpp -o stamina

clean:
	-rm -f *.o stamina

