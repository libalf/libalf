# $Id$
# libalf Makefile

.PHONY: all clean install uninstall

PREFIX ?= /usr/local/

LOCALCHANGES = $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION = $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION=svn-r${SVNREVISION}${LOCALCHANGES}

CPPFLAGS+=-Wall -I../include/ -I${PREFIX}/include -shared -fpic -ggdb3
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -lAMoRE -L${PREFIX}/lib

SONAME=libalf

OBJECTS=alf.o alphabet.o answer.o automata.o automata_amore.o basic_string.o logger.o minisat_solver.o normalizer_msc.o oracle_automaton.o statistics.o

all:	libalf.so

libalf.so: ${OBJECTS}
	g++ $(OBJECTS) $(LDFLAGS) -o $@
#	ld ${OBJECTS} ${LDFLAGS} -o $@

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

