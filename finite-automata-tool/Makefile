# $Id$
# fat Makefile

.PHONY: all clean jpegs install uninstall

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

VERSIONTAG:=$(shell cat "../version")
ifeq (${VERSIONTAG},)
# subversion branch
LOCALCHANGES:=$(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION:=$(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION?=svn-r${SVNREVISION}${LOCALCHANGES}
LIBVERSIONTAG?=.svn
else
# release branch
VERSION?=v${VERSIONTAG}
LIBVERSIONTAG?=.${VERSIONTAG}
endif


CPPFLAGS+=-Wall -I${INCLUDEDIR} -ggdb3
CPPFLAGS+=-Wextra
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
LDFLAGS+=-L${LIBDIR}

OBJECTS=fat.o input.o transformation.o output.o

all: fat fateq

fat: $(OBJECTS)
	g++ $(OBJECTS) ${LDFLAGS} -llangen -lalf -lAMoRE++ -o $@

fateq: fateq.o
	g++ fateq.o ${LDFLAGS} -lalf -lAMoRE++ -o $@

clean: nodata
	-rm -f fat fateq *.o

nodata:
	-rm -f *.dot *.jpg

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

install: fat fateq
	@echo
	@echo installing fat to ${BINDIR} ...
	@echo
	install -v -m 755 fat fateq ${BINDIR}

uninstall:
	@echo
	@echo removing fat from ${BINDIR} ...
	@echo
	rm -f ${BINDIR}/fat ${BINDIR}/fateq

