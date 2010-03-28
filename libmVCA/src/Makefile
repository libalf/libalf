# $Id$
# libmVCA Makefile

.PHONY: all clean install uninstall

PREFIX ?= /usr/local/

LOCALCHANGES := $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION := $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION ?= svn-r${SVNREVISION}${LOCALCHANGES}

CPPFLAGS+=-Wall -I../include/ -I${PREFIX}/include -shared -fpic -ggdb3
CPPFLAGS+=-Wextra
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
#CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -L${PREFIX}/lib

SONAME=libmVCA

OBJECTS=pushdown.o transition_function.o mVCA.o deterministic_mVCA.o nondeterministic_mVCA.o p-automaton.o

ifeq (${OS}, Windows_NT)
  LDFLAGS += -lws2_32
  TARGET=libmVCA.dll
else
  TARGET=libmVCA.so
endif

all:	${TARGET}

libmVCA.so: ${OBJECTS}
	g++ $(OBJECTS) $(LDFLAGS) -o $@ 

libmVCA.dll: ${OBJECTS}
	g++ $(OBJECTS) $(LDFLAGS) -o $@ 

libmVCA.a: ${OBJECTS}
	ar rcs $@ $(OBJECTS)

clean:
	-rm -f *.o libmVCA.so libmVCA.a

install: libmVCA.so
	echo installing libmVCA library to ${PREFIX}/lib/
	mkdir -p ${PREFIX}/lib/
	cp libmVCA.so ${PREFIX}/lib/
	chmod 755 ${PREFIX}/lib/libmVCA.so
	-test `id -u` -eq 0 && /sbin/ldconfig; true

uninstall:
	echo removing libmVCA library from ${PREFIX}/lib/
	rm ${PREFIX}/lib/libmVCA.so
	-test `id -u` -eq 0 && /sbin/ldconfig; true

