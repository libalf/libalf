# $Id$
# Makefile of libalf stamina interface

.PHONY: all clean

include ../config.mk

CPPFLAGS+=-I../include
CPPFLAGS+=-Wall -Wextra -ggdb3
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
CPPFLAGS+=-DVERBOSE

LDFLAGS+=-L../src -lalf

all:	stamina

stamina: stamina.cpp stamina.h
	${CPP} ${CPPFLAGS} ${LDFLAGS} stamina.cpp -o stamina

clean:
	-rm -f *.o stamina

