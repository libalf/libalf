# $Id$
# liblangen (LANguageGENerator) Makefile

.PHONY: all clean install uninstall

PREFIX ?= /usr/local/

LOCALCHANGES := $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION := $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION ?= svn-r${SVNREVISION}${LOCALCHANGES}

CPPFLAGS+=-Wall -I../include/ -I${PREFIX}/include -shared -fpic -ggdb3
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
#CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -L${PREFIX}/lib -lgmpxx -lgmp

SONAME=liblangen

OBJECTS=dfa_enumerator.o dfa_randomgenerator.o nfa_randomgenerator.o prng.o regex_randomgenerator.o langen.o

all:	liblangen.so

liblangen.so: ${OBJECTS}
	g++ $(OBJECTS) $(LDFLAGS) -o $@

liblangen.a: ${OBJECTS}
	ar rcs $@ $(OBJECTS)

clean:
	-rm -f *.o liblangen.so liblangen.a

install: liblangen.so
	echo installing liblangen library to ${PREFIX}/lib/
	mkdir -p ${PREFIX}/lib/
	cp liblangen.so ${PREFIX}/lib/
	chmod 755 ${PREFIX}/lib/liblangen.so
	-test `id -u` -eq 0 && /sbin/ldconfig; true

uninstall:
	echo removing liblangen library from ${PREFIX}/lib/
	rm ${PREFIX}/lib/liblangen.so
	-test `id -u` -eq 0 && /sbin/ldconfig; true

