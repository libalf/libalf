# $Id$
# libalf Makefile

.PHONY: all clean install uninstall

PREFIX ?= /usr/local/

CPPFLAGS+=-Wall -I../include/ -shared -fpic -ggdb3
CPPFLAGS+=-D__cplusplus -DLIBALF_LIBRARY_COMPILATION
LDFLAGS+=-shared -lAMoRE

SONAME=libalf

OBJECTS=alphabet.o answer.o automata.o automata_amore.o basic_string.o logger.o normalizer_msc.o oracle_automaton.o statistics.o

all:	libalf.so

libalf.so: ${OBJECTS}
	ld ${OBJECTS} ${LDFLAGS} -o $@
	strip $@

clean:
	-rm *.o
	-rm libalf.so

install: libalf.so
	echo installing libalf library to ${PREFIX}/lib/
	mkdir -p ${PREFIX}/lib/
	cp libalf.so ${PREFIX}/lib/
	chmod 755 ${PREFIX}/lib/libalf.so

uninstall:
	echo removing libalf library from ${PREFIX}/lib/
	rm ${PREFIX}/lib/libalf.so

