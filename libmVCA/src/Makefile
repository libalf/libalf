# $Id$
# libmVCA Makefile

.PHONY: all clean install uninstall

include ../config.mk

VERSIONTAG:=$(shell cat "../version" 2>/dev/null)
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
  TARGET=mVCA.dll
else
  TARGET=libmVCA.so
endif

INSTALL_SHARED_NAME=${TARGET}${LIBVERSIONTAG}
INSTALL_STATIC_NAME=libmVCA.a${LIBVERSIONTAG}


CPPFLAGS+=-Wall -I../include/ -I${INCLUDEDIR} -shared -fpic
CPPFLAGS+=-Wextra
CPPFLAGS+=-DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
#CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -L${LIBDIR}

SONAME=libmVCA

OBJECTS=pushdown.o transition_function.o mVCA.o deterministic_mVCA.o nondeterministic_mVCA.o p-automaton.o

all:	${TARGET}

libmVCA.so: ${OBJECTS}
	${CXX} $(OBJECTS) $(LDFLAGS) -o $@ 

mVCA.dll: ${OBJECTS}
	${CXX} $(OBJECTS) $(LDFLAGS) -o $@ -lws2_32

libmVCA.a: ${OBJECTS}
	${AR} rcs $@ $(OBJECTS)

clean:
	-rm -f *.o libmVCA.so mVCA.dll libmVCA.a 2>&1

install: ${TARGET} libmVCA.a
	@echo
	@echo installing libmVCA library to ${DESTDIR}${LIBDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${LIBDIR}
	install -T -v -m 755 ${TARGET} ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME}
	install -T -v -m 755 libmVCA.a  ${DESTDIR}${LIBDIR}/${INSTALL_STATIC_NAME}
	# symlinks
	-rm -f ${DESTDIR}${LIBDIR}/${TARGET} 2>&1
	ln -s ${LIBDIR}/${INSTALL_SHARED_NAME} ${DESTDIR}${LIBDIR}/${TARGET}
	-rm -f ${DESTDIR}${LIBDIR}/libmVCA.a 2>&1
	ln -s ${LIBDIR}/${INSTALL_STATIC_NAME} ${DESTDIR}${LIBDIR}/libmVCA.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

uninstall:
	@echo
	@echo removing libmVCA library from ${DESTDIR}${LIBDIR} ...
	@echo
	rm -f ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME}
	rm -f ${DESTDIR}${LIBDIR}/${INSTALL_STATIC_NAME}
	rm -f ${DESTDIR}${LIBDIR}/${TARGET}
	rm -f ${DESTDIR}${LIBDIR}/libmVCA.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

