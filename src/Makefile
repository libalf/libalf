# $Id$
# libalf Makefile

CXXFLAGS=-I../include/ -ggdb3
LDFLAGS=

SONAME=libalf

OBJECTS=alphabet.o logger.o

.PHONY: all clean install uninstall

all:	libalf.so

libalf.so: ${OBJECTS}
	ld -shared -soname ${SONAME} -o $@ ${OBJECTS}
	strip $@

clean:
	-rm *.o
	-rm libalf.so

install:

uninstall:

