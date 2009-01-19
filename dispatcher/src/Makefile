# $Id$
# libalf dispatcher Makefile

.PHONY: all clean install uninstall

LOCALCHANGES = $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION = $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION=svn-r${SVNREVISION}${LOCALCHANGES}

CPPFLAGS+=-Wall -I../../include/ -I ../include/ -ggdb3
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
LDFLAGS+=-lalf -lAMoRE

OBJECTS=amore.o main.o servant.o serversocket.o session.o

all:	dispatcher

dispatcher: ${OBJECTS}
	g++ ${OBJECTS} ${LDFLAGS} -o dispatcher

clean:
	-rm *.o
	-rm dispatcher

install: dispatcher

uninstall:

