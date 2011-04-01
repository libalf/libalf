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

INSTALL_SO_NAME=libAMoRE++.so${LIBVERSIONTAG}
INSTALL_A_NAME=libAMoRE++.a${LIBVERSIONTAG}

CPPFLAGS+=-Wall -I../include/ -I${INCLUDEDIR} -shared -fpic -ggdb3
CPPFLAGS+=-Wextra
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
CPPFLAGS+=-fno-stack-protector
#CPPFLAGS+=-DANTICHAIN_DEBUG
LDFLAGS+=-shared -L${LIBDIR} -lAMoRE


OBJECTS=amore.o finite_automaton.o nondeterministic_finite_automaton.o deterministic_finite_automaton.o

all:	libAMoRE++.so

libAMoRE++.so: ${OBJECTS}
	${CXX} $(OBJECTS) $(LDFLAGS) -o $@

libAMoRE++.a: ${OBJECTS}
	ar crs $@ $?

clean:
	-rm -f *.o libAMoRE++.so libAMoRE++.a

install: libAMoRE++.so libAMoRE++.a
	@echo
	@echo installing libAMoRE++ to ${LIBDIR} ...
	@echo
	-install -v -m 755 -d ${LIBDIR}
	install -T -v -m 755 libAMoRE++.so ${LIBDIR}/${INSTALL_SO_NAME}
	install -T -v -m 755 libAMoRE++.a  ${LIBDIR}/${INSTALL_A_NAME}
	# symlinks
	-rm -f ${LIBDIR}/libAMoRE++.so
	ln -s ${LIBDIR}/${INSTALL_SO_NAME} ${LIBDIR}/libAMoRE++.so
	-rm -f ${LIBDIR}/libAMoRE++.a
	ln -s ${LIBDIR}/${INSTALL_A_NAME}  ${LIBDIR}/libAMoRE++.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

uninstall:
	@echo
	@echo removing libAMoRE++ library from ${LIBDIR} ...
	@echo
	rm -f ${LIBDIR}/${INSTALL_SO_NAME}
	rm -f ${LIBDIR}/${INSTALL_A_NAME}
	rm -f ${LIBDIR}/libAMoRE++.so
	rm -f ${LIBDIR}/libAMoRE++.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

finite_automaton.cpp: set.h

