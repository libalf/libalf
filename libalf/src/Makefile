# $Id$
# libalf Makefile

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

CPPFLAGS+=-Wall -I../include/ -I${INCLUDEDIR} -shared -fpic -ggdb3
CPPFLAGS+=-Wextra
CPPFLAGS+=-D__cplusplus -DVERSION="\"${VERSION}\""
# for ubuntu, try disabling stack-smashing due to linker failures:
#CPPFLAGS+=-fno-stack-protector
LDFLAGS+=-shared -L${LIBDIR}

OBJECTS=alf.o alphabet.o conjecture.o basic_string.o learning_algorithm.o logger.o minisat_solver.o normalizer_msc.o statistics.o

# Choose OS
ifeq (${OS}, Windows_NT)
  LDFLAGS += -lws2_32
  TARGET=libalf.dll
else
  TARGET=libalf.so
endif

INSTALL_SHARED_NAME=${TARGET}${LIBVERSIONTAG}
INSTALL_STATIC_NAME=libalf.a${LIBVERSIONTAG}

all:	${TARGET}

libalf.so: ${OBJECTS}
	${CXX} $(OBJECTS) $(LDFLAGS) -o $@ 

libalf.dll: ${OBJECTS}
	${CXX} $(OBJECTS) $(LDFLAGS) -o $@ 

libalf.a: ${OBJECTS}
	${AR} rcs $@ $(OBJECTS)

clean:
	-rm -f *.o alf.dll libalf.so libalf.a

install: ${TARGET} libalf.a
	@echo
	@echo installing libalf library to ${LIBDIR} ...
	@echo
	-install -v -m 755 -d ${LIBDIR}
	install -T -v -m 755 ${TARGET} ${LIBDIR}/${INSTALL_SHARED_NAME}
	install -T -v -m 755 libalf.a  ${LIBDIR}/${INSTALL_STATIC_NAME}
	# symlinks
	-rm -f ${LIBDIR}/${TARGET}
	ln -s ${LIBDIR}/${INSTALL_SHARED_NAME} ${LIBDIR}/${TARGET}
	-rm -f ${LIBDIR}/libalf.a
	ln -s ${LIBDIR}/${INSTALL_STATIC_NAME}  ${LIBDIR}/libalf.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true

uninstall:
	@echo
	@echo removing libalf library from ${LIBDIR} ...
	@echo
	rm -f ${LIBDIR}/${INSTALL_SHARED_NAME}
	rm -f ${LIBDIR}/${INSTALL_STATIC_NAME}
	rm -f ${LIBDIR}/${TARGET}
	rm -f ${LIBDIR}/libalf.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true
