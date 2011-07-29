# $Id$
# libAMoRE Makefile

.PHONY: all clean doc install uninstall installHeaders uninstallHeaders installDocs uninstallDocs

include config.mk

VERSIONTAG:=$(shell cat "version" 2>/dev/null)
ifeq (${VERSIONTAG},)
# subversion branch
LOCALCHANGES := $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION := $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION ?= libalf-svn-r${SVNREVISION}${LOCALCHANGES}
LIBVERSIONTAG?=.svn
else
# release branch
VERSION?=v${VERSIONTAG}
LIBVERSIONTAG?=.${VERSIONTAG}
endif

# Choose OS
ifeq (${OS}, Windows_NT)
  LDFLAGS += -lws2_32
  TARGET=AMoRE.dll
else
  TARGET=libAMoRE.so
  CFLAGS += -DUNIX -DLINUX
endif

INSTALL_SHARED_NAME=${TARGET}${LIBVERSIONTAG}
INSTALL_STATIC_NAME=libAMoRE.a${LIBVERSIONTAG}

CFLAGS += -DDEBUG -DJAVAEXCEPT -DLIBAMORE -DBIT32
CFLAGS += -DVERSION="\"${VERSION}\""
CFLAGS += -Wall -Iinclude -fpic
CFLAGS += -fno-stack-protector

LDFLAGS += -fpic -shared

# data objects
LdoOBJ = nfa.o dfa.o regexp.o sfexp.o mon.o language.o rexFromString.o

# transformations
TransfOBJ   = dfa2nfa.o dfamdfa.o enfa2nfa.o nfa2dfa.o nfa2rex.o  \
	nfa2mnfa_sol.o nfa2mnfa_help.o nfa2mnfa.o rex2nfa.o  \
	regexp-infix.o genrex2nfa.o \
	dfa2mon.o  unaryB.o binary.o mon2dcl.o mon2rel.o mon2sfx.o

# tests
TestOBJ = testUnary.o testBinary.o

# all object files
OBJ = global.o buffer.o liberror.o parse.o fileIO.o \
	$(addprefix ldo/,   $(LdoOBJ))       \
	$(addprefix transf/,$(TransfOBJ))    \
	$(addprefix test/,  $(TestOBJ))      \
	debugPrint.o 

all: libAMoRE.a ${TARGET}

libtest: libtest.c libAMoRE.so
	${CC} ${CFLAGS} -o $@ $< -g -L. -lAMoRE

libAMoRE.a: $(OBJ)
	${AR} crs $@ $?

libAMoRE.so: $(OBJ)
	${CC} ${LDFLAGS} -o $@ $?

AMoRE.dll: $(OBJ)
	${CC} ${LDFLAGS} -o $@ $?
	
clean:
	-rm -f *~ */*~ $(OBJ) libAMoRE.a libAMoRE.so AMoRE.dll libtest 2>&1
	-rm -Rf docs/html docs/latex  2>&1

doc:
	doxygen docs/Doxyfile-libAMoRE


install: installHeaders all
	@echo
	@echo installing libAMoRE library to ${DESTDIR}${INCLUDEDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${LIBDIR}
	install -T -v -m 755 ${TARGET} ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME}
	install -T -v -m 755 libAMoRE.a ${DESTDIR}${LIBDIR}/${INSTALL_STATIC_NAME}
	# symlinks
	-rm -f ${DESTDIR}${LIBDIR}/${TARGET} 2>&1
	ln -s ${LIBDIR}/${INSTALL_SHARED_NAME} ${DESTDIR}${LIBDIR}/${TARGET}
	-rm -f ${DESTDIR}${LIBDIR}/libAMoRE.a 2>&1
	ln -s ${LIBDIR}/${INSTALL_STATIC_NAME} ${DESTDIR}${LIBDIR}/libAMoRE.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true


uninstall: uninstallHeaders
	@echo
	@echo removing libAMoRE library from ${INCLUDEDIR} ...
	@echo
	rm -f ${DESTDIR}${LIBDIR}/${INSTALL_SHARED_NAME}
	rm -f ${DESTDIR}${LIBDIR}/${INSTALL_STATIC_NAME}
	rm -f ${DESTDIR}${LIBDIR}/${TARGET}
	rm -f ${DESTDIR}${LIBDIR}/libAMoRE.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true


installHeaders:
	@echo
	@echo installing libAMoRE headers to ${DESTDIR}${INCLUDEDIR} ...
	@echo
	install -v -m 755 -d ${DESTDIR}${INCLUDEDIR}/amore
	install -v -m 644 include/amore/* ${DESTDIR}${INCLUDEDIR}/amore
uninstallHeaders:
	@echo
	@echo removing libAMoRE headers from ${DESTDIR}${INCLUDEDIR} ...
	@echo
	rm -Rf ${DESTDIR}${INCLUDEDIR}/amore


installDocs: doc
	@echo
	@echo installing libAMoRE doc to ${DESTDIR}${DOCDIR} ...
	@echo
	install -v -m 755 -d ${DESTDIR}$(DOCDIR)/libAMoRE/html 
	install -v -m 644 docs/html/* ${DESTDIR}$(DOCDIR)/libAMoRE/html
uninstallDocs:
	@echo
	@echo removing libAMoRE doc from ${DESTDIR}${DOCDIR} ...
	@echo
	rm -Rf ${DESTDIR}${DOCDIR}/libAMoRE

