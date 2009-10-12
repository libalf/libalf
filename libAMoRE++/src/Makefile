# $Id$
# libalf Makefile

.PHONY: all clean install uninstall

PREFIX ?= /usr/local/

#LOCALCHANGES := $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
#SVNREVISION := $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
#VERSION ?= svn-r${SVNREVISION}${LOCALCHANGES}
VERSION := v0.1

CPPFLAGS+=-Wall -I../include/ -I${PREFIX}/include -shared -fpic -ggdb3
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -L${PREFIX}/lib -lAMoRE

SONAME=libalf

OBJECTS=amore.o finite_automaton.o nondeterministic_finite_automaton.o deterministic_finite_automaton.o

all:	libAMoRE++.so

libAMoRE++.so: ${OBJECTS}
	g++ $(OBJECTS) $(LDFLAGS) -o $@

clean:
	-rm -f *.o libAMoRE++.so

install: libAMoRE++.so
	echo installing libAMoRE++ to ${PREFIX}/lib/
	mkdir -p ${PREFIX}/lib
	cp libAMoRE++.so ${PREFIX}/lib
	chmod 755 ${PREFIX}/lib/libAMoRE++.so
	-test `id -u` -eq 0 && /sbin/ldconfig

uninstall:
	echo removing libAMoRE++.so from ${PREFIX}/lib/
	rm ${PREFIX}/lib/libAMoRE++.so
	-test `id -u` -eq 0 && /sbin/ldconfig


