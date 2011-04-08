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

INSTALL_SO_NAME=liblangen.so${LIBVERSIONTAG}
INSTALL_A_NAME=liblangen.a${LIBVERSIONTAG}

CPPFLAGS+=-Wall -I../include/ -I${INCLUDEDIR} -shared -fpic
CPPFLAGS+=-Wextra
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
#CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -L${LIBDIR} -lgmpxx -lgmp

OBJECTS=dfa_enumerator.o dfa_randomgenerator.o nfa_randomgenerator.o prng.o regex_randomgenerator.o langen.o

all:	liblangen.so liblangen.a

liblangen.so: ${OBJECTS}
	${CXX} $(OBJECTS) $(LDFLAGS) -o $@

liblangen.a: ${OBJECTS}
	${AR} rcs $@ $(OBJECTS)

clean:
	-rm -f *.o liblangen.so liblangen.a

install: liblangen.so liblangen.a
	@echo
	@echo installing liblangen library to ${LIBDIR} ...
	@echo
	-install -v -m 755 -d ${LIBDIR}
	install -T -v -m 755 liblangen.so ${LIBDIR}/${INSTALL_SO_NAME}
	install -T -v -m 755 liblangen.a  ${LIBDIR}/${INSTALL_A_NAME}
	# symlinks
	-rm -f ${LIBDIR}/liblangen.so
	ln -s ${LIBDIR}/${INSTALL_SO_NAME} ${LIBDIR}/liblangen.so
	-rm -f ${LIBDIR}/liblangen.a
	ln -s ${LIBDIR}/${INSTALL_A_NAME}  ${LIBDIR}/liblangen.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

uninstall:
	@echo
	@echo removing liblangen library from ${LIBDIR} ...
	@echo
	rm -f ${LIBDIR}/${INSTALL_SO_NAME}
	rm -f ${LIBDIR}/${INSTALL_A_NAME}
	rm -f ${LIBDIR}/liblangen.so
	rm -f ${LIBDIR}/liblangen.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

