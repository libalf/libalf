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
#CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -L${PREFIX}/lib

SONAME=libalf

OBJECTS=alf.o alphabet.o answer.o conjecture.o basic_string.o logger.o minisat_solver.o normalizer_msc.o statistics.o

all:	libalf.so

libalf.so: ${OBJECTS}
	g++ $(OBJECTS) $(LDFLAGS) -o $@ 

libalf.a: ${OBJECTS}
	ar rcs $@ $(OBJECTS)

clean:
	-rm -f *.o libalf.so libalf.a

install: libalf.so
	echo installing libalf library to ${PREFIX}/lib/
	mkdir -p ${PREFIX}/lib/
	cp libalf.so ${PREFIX}/lib/
	chmod 755 ${PREFIX}/lib/libalf.so
	-test `id -u` -eq 0 && /sbin/ldconfig

uninstall:
	echo removing libalf library from ${PREFIX}/lib/
	rm ${PREFIX}/lib/libalf.so
	-test `id -u` -eq 0 && /sbin/ldconfig

