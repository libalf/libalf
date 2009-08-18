# $Id$
# fat Makefile

.PHONY: all clean jpegs install uninstall

PREFIX ?= /usr/local/

LOCALCHANGES = $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION = $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION=svn-r${SVNREVISION}${LOCALCHANGES}

CPPFLAGS+=-Wall -I../include/ -I${PREFIX}/include -ggdb3
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
LDFLAGS+=-L${PREFIX}/lib -llangen -lalf -lAMoRE++

OBJECTS=fat.o input.o transformation.o output.o

all: fat fateq

fat: $(OBJECTS)
	g++ $(OBJECTS) $(LDFLAGS) -o $@

fateq: fateq.o
	g++ fateq.o ${LDFLAGS} -o $@

clean:
	-rm fat fateq *.o
	-rm *.dot *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

install: fat
	cp fat ${PREFIX}/bin/fat
	cp fateq ${PREFIX}/bin/fateq

uninstall:
	rm ${PREFIX}/bin/fat
	rm ${PREFIX}/bin/fateq

