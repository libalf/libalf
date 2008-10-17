# $Id$
# libalf Makefile

.PHONY: all clean install uninstall

CPPFLAGS=-Wall -I../include/ -shared -fpic -ggdb3
LDFLAGS=-shared

SONAME=libalf

OBJECTS=alphabet.o logger.o

all:	libalf.so

libalf.so: ${OBJECTS}
	g++ ${LDFLAGS} -o $@ ${OBJECTS}
	strip $@

clean:
	-rm *.o
	-rm libalf.so

install:

uninstall:

