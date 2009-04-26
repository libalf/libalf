# $Id$
# LanguageGenerator Makefile

.PHONY: all clean install uninstall

PREFIX ?= /usr/local/

LOCALCHANGES = $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION = $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION=svn-r${SVNREVISION}${LOCALCHANGES}

CPPFLAGS+=-Wall -I../include/ -I${PREFIX}/include -shared -fpic -ggdb3
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -L${PREFIX}/lib

SONAME=liblangen

OBJECTS=automaton_constructor.o DFAenumerator.o

all:	liblangen.so

liblangen.so: ${OBJECTS}
	g++ $(OBJECTS) $(LDFLAGS) -o $@

clean:
	-rm *.o
	-rm liblangen.so

