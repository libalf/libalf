# $Id$
# fat Makefile

.PHONY: all clean jpegs install uninstall

include config.mk

VERSIONTAG:=$(shell cat "version" 2>/dev/null)
ifeq (${VERSIONTAG},)
GITHASH := $(shell git --no-pager rev-parse --short HEAD)
DIRTYFLAG := $(shell git --no-pager status -s | wc -c | sed -e 's/^[1-9][0-9]*$$/-DIRTY/' -e 's/^0$$//')
VERSION := git-${GITHASH}${DIRTYFLAG}
LIBVERSIONTAG := .git
else
# release branch
VERSION := v${VERSIONTAG}
LIBVERSIONTAG := .${VERSIONTAG}
endif


CPPFLAGS+=-Wall -I${INCLUDEDIR}
CPPFLAGS+=-Wextra
CPPFLAGS+=-DVERSION="\"${VERSION}\""
LDFLAGS+=-L${LIBDIR}

OBJECTS=fat.o input.o transformation.o output.o

all: fat fateq

fat: $(OBJECTS)
	${CXX} $(OBJECTS) ${LDFLAGS} -llangen -lalf -lAMoRE++ -lAMoRE -o $@

fateq: fateq.o
	${CXX} fateq.o ${LDFLAGS} -lalf -lAMoRE++ -lAMoRE -o $@

clean: nodata
	-rm -f fat fateq *.o 2>&1

nodata:
	-rm -f *.dot *.jpg 2>&1

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

install: fat fateq
	@echo
	@echo installing fat to ${DESTDIR}${BINDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${BINDIR}
	install -v -m 755 fat fateq ${DESTDIR}${BINDIR}

uninstall:
	@echo
	@echo removing fat from ${DESTDIR}${BINDIR} ...
	@echo
	rm -f ${DESTDIR}${BINDIR}/fat ${DESTDIR}${BINDIR}/fateq

