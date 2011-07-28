# $Id$
# liblangen (LANguageGENerator) Makefile

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
  TARGET=langen.dll
else
  TARGET=liblangen.so
endif

INSTALL_SHARED_NAME=${TARGET}${LIBVERSIONTAG}
INSTALL_STATIC_NAME=liblangen.a${LIBVERSIONTAG}

CPPFLAGS+=-Wall -I../include/ -I${INCLUDEDIR} -shared -fpic
CPPFLAGS+=-Wextra
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
#CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -L${LIBDIR} -lgmpxx -lgmp

OBJECTS=dfa_enumerator.o dfa_randomgenerator.o nfa_randomgenerator.o prng.o regex_randomgenerator.o langen.o

all:	${TARGET} liblangen.a

liblangen.so: ${OBJECTS}
	${CXX} $(OBJECTS) $(LDFLAGS) -o $@

langen.dll: ${OBJECTS}
	${CXX} $(OBJECTS) $(LDFLAGS) -o $@

liblangen.a: ${OBJECTS}
	${AR} rcs $@ $(OBJECTS)

clean:
	-rm -f *.o liblangen.so langen.dll liblangen.a

install: ${TARGET} liblangen.a
	@echo
	@echo installing liblangen library to ${DESTDIR}${LIBDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${LIBDIR}
	install -T -v -m 755 ${TARGET} ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME}
	install -T -v -m 755 liblangen.a ${DESTDIR}${LIBDIR}/${INSTALL_STATIC_NAME}
	# symlinks
	-rm -f ${DESTDIR}${LIBDIR}/${TARGET}
	ln -s ${LIBDIR}/${INSTALL_SHARED_NAME} ${DESTDIR}${LIBDIR}/${TARGET}
	-rm -f ${DESTDIR}${LIBDIR}/liblangen.a
	ln -s ${LIBDIR}/${INSTALL_STATIC_NAME} ${DESTDIR}${LIBDIR}/liblangen.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

uninstall:
	@echo
	@echo removing liblangen library from ${DESTDIR}${LIBDIR} ...
	@echo
	rm -f ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME}
	rm -f ${DESTDIR}${LIBDIR}/${INSTALL_STATIC_NAME}
	rm -f ${DESTDIR}${LIBDIR}/${TARGET}
	rm -f ${DESTDIR}${LIBDIR}/liblangen.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

