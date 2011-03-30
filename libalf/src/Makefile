# $Id$
# libalf Makefile

.PHONY: all clean install uninstall

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

INSTALL_SO_NAME=libalf.so${LIBVERSIONTAG}
INSTALL_A_NAME=libalf.a${LIBVERSIONTAG}

CPPFLAGS+=-Wall -I../include/ -I${INCLUDEDIR} -shared -fpic -ggdb3
CPPFLAGS+=-Wextra
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
#CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -L${LIBDIR}

OBJECTS=alf.o alphabet.o conjecture.o basic_string.o learning_algorithm.o logger.o minisat_solver.o normalizer_msc.o statistics.o

ifeq (${OS}, Windows_NT)
  LDFLAGS += -lws2_32
  TARGET=libalf.dll
else
  TARGET=libalf.so
endif

all:	${TARGET}

libalf.so: ${OBJECTS}
	g++ $(OBJECTS) $(LDFLAGS) -o $@ 

libalf.dll: ${OBJECTS}
	g++ $(OBJECTS) $(LDFLAGS) -o $@ 

libalf.a: ${OBJECTS}
	ar rcs $@ $(OBJECTS)

clean:
	-rm -f *.o libalf.so libalf.a

install: libalf.so libalf.a
	@echo
	@echo installing libalf library to ${LIBDIR} ...
	@echo
	-install -v -m 755 -d ${LIBDIR}
	install -T -v -m 755 libalf.so ${LIBDIR}/${INSTALL_SO_NAME}
	install -T -v -m 755 libalf.a  ${LIBDIR}/${INSTALL_A_NAME}
	# symlinks
	-rm -f ${LIBDIR}/libalf.so
	ln -s ${LIBDIR}/${INSTALL_SO_NAME} ${LIBDIR}/libalf.so
	-rm -f ${LIBDIR}/libalf.a
	ln -s ${LIBDIR}/${INSTALL_A_NAME}  ${LIBDIR}/libalf.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

uninstall:
	@echo
	@echo removing libalf library from ${LIBDIR} ...
	@echo
	rm -f ${LIBDIR}/${INSTALL_SO_NAME}
	rm -f ${LIBDIR}/${INSTALL_A_NAME}
	rm -f ${LIBDIR}/libalf.so
	rm -f ${LIBDIR}/libalf.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

