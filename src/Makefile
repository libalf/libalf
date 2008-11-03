# $Id$
# libalf Makefile

.PHONY: all clean install uninstall

CPPFLAGS+=-Wall -I../include/ -shared -fpic -ggdb3
CPPFLAGS+=-D__libalf_so_compilation__ -D__cplusplus
LDFLAGS+=-shared -lAMoRE

SONAME=libalf

OBJECTS=alphabet.o automata.o automata_amore.o logger.o oracle_automaton.o

all:	libalf.so

libalf.so: ${OBJECTS}
	ld ${OBJECTS} ${LDFLAGS} -o $@
	#strip $@

clean:
	-rm *.o
	-rm libalf.so

install: libalf.so
	cp libalf.so /usr/local/lib/
	chmod 755 /usr/local/lib/libalf.so

uninstall:
	rm /usr/local/lib/libalf.so

