# $Id$
# libalf Makefile

.PHONY: all clean install uninstall

CPPFLAGS+=-Wall -I../include/ -shared -fpic -ggdb3
CPPFLAGS+=-D__libalf_so_compilation__ -D__cplusplus
LDFLAGS+=-shared -lAMoRE

SONAME=libalf

OBJECTS=alphabet.o automata_amore.o logger.o oracle_automata.o

all:	libalf.so

libalf.so: ${OBJECTS}
	ld ${OBJECTS} ${LDFLAGS} -o $@
	strip $@

clean:
	-rm *.o
	-rm libalf.so

install:

uninstall:

