# $Id$
# libAMoRE++ Makefile

.PHONY: all clean install uninstall

include ../config.mk

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

# Choose OS
ifeq (${OS}, Windows_NT)
  LDFLAGS += -lws2_32
  TARGET=AMoRE++.dll
else
  TARGET=libAMoRE++.so
endif

INSTALL_SHARED_NAME=$(TARGET)${LIBVERSIONTAG}
INSTALL_STATIC_NAME=libAMoRE++.a${LIBVERSIONTAG}

CPPFLAGS+=-Wall -I../include/ -I${INCLUDEDIR} -shared -fpic
CPPFLAGS+=-Wextra
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
CPPFLAGS+=-fno-stack-protector
#CPPFLAGS+=-DANTICHAIN_DEBUG
LDFLAGS+=-shared -L${LIBDIR} -lAMoRE


OBJECTS=amore.o finite_automaton.o nondeterministic_finite_automaton.o deterministic_finite_automaton.o monoid.o

all:	${TARGET}

libAMoRE++.so: ${OBJECTS}
	${CXX} $(OBJECTS) $(LDFLAGS) -o $@

AMoRE++.dll: ${OBJECTS}
	${CXX} $(OBJECTS) $(LDFLAGS) -o $@ -lws2_32
	
libAMoRE++.a: ${OBJECTS}
	${AR} crs $@ $?

clean:
	-rm -f *.o AMoRE.dll libAMoRE++.so libAMoRE++.a

install: ${TARGET} libAMoRE++.a
	@echo
	@echo installing ${TARGET} to ${DESTDIR}${LIBDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${LIBDIR}
	install -T -v -m 755 ${TARGET} ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME}
	install -T -v -m 755 libAMoRE++.a ${DESTDIR}${LIBDIR}/${INSTALL_STATIC_NAME}
	# symlinks
	-rm -f ${DESTDIR}${LIBDIR}/${TARGET}
	ln -s ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME} ${DESTDIR}${LIBDIR}/${TARGET}
	-rm -f ${DESTDIR}${LIBDIR}/libAMoRE++.a
	ln -s ${DESTDIR}${LIBDIR}/${INSTALL_STATIC_NAME} ${DESTDIR}${LIBDIR}/libAMoRE++.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

uninstall:
	@echo
	@echo removing libAMoRE++ library from ${DESTDIR}${LIBDIR} ...
	@echo
	rm -f ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME}
	rm -f ${DESTDIR}${LIBDIR}/${INSTALL_STATIC_NAME}
	rm -f ${DESTDIR}${LIBDIR}/${TARGET}
	rm -f ${DESTDIR}${LIBDIR}/libAMoRE++.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

finite_automaton.cpp: set.h

