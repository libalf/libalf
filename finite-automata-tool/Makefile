# $Id$
# fat Makefile

.PHONY:

PREFIX ?= /usr/local/

LOCALCHANGES = $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION = $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION=svn-r${SVNREVISION}${LOCALCHANGES}

CPPFLAGS+=-Wall -I../include/ -I${PREFIX}/include -ggdb3
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
LDFLAGS+=-L${PREFIX}/lib

OBJECTS=fat.o

all: fat

fat: $(OBJECTS)
	g++ $(OBJECTS) $(LDFLAGS) -o $@

clean:
	-rm fat *.o

install: fat
	cp fat ${PREFIX}/bin/fat

uninstall:
	rm ${PREFIX}/bin/fat

