# $Id$
# libAMoRE Makefile

.PHONY: all clean doc install uninstall installHeaders uninstallHeaders installDocs uninstallDocs

include config.mk

VERSIONTAG:=$(shell cat "version")
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

INSTALL_SO_NAME=libAMoRE.so${LIBVERSIONTAG}
INSTALL_A_NAME=libAMoRE.a${LIBVERSIONTAG}

CFLAGS += -DDEBUG -DJAVAEXCEPT -DLIBAMORE -DUNIX -DLINUX -DBIT32
CFLAGS += -DVERSION="\"${VERSION}\""
CFLAGS += -Wall -Iinclude -fpic
CFLAGS += -fno-stack-protector
CFLAGS += -ggdb3

LDFLAGS += -fpic -shared
LDFLAGS += -ggdb3

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
	atime.o debugPrint.o 

all: libAMoRE.a libAMoRE.so

libtest: libtest.c libAMoRE.so
	$(CC) $(CFLAGS) -o $@ $< -g -L. -lAMoRE

libAMoRE.a: $(OBJ)
	ar crs $@ $?

libAMoRE.so: $(OBJ)
	gcc ${LDFLAGS} -o $@ $?

clean:
	-rm -f *~ */*~ $(OBJ) libAMoRE.a libAMoRE.so libtest
	-rm -Rf docs/html docs/latex 

doc:
	doxygen docs/Doxyfile-libAMoRE


install: installHeaders all
	@echo
	@echo installing libAMoRE library to ${INCLUDEDIR} ...
	@echo
	-install -v -m 755 -d ${LIBDIR}
	install -T -v -m 755 libAMoRE.so ${LIBDIR}/${INSTALL_SO_NAME}
	install -T -v -m 755 libAMoRE.a ${LIBDIR}/${INSTALL_A_NAME}
	# symlinks
	-rm -f ${LIBDIR}/libAMoRE.so
	ln -s ${LIBDIR}/${INSTALL_SO_NAME} ${LIBDIR}/libAMoRE.so
	-rm -f ${LIBDIR}/libAMoRE.a
	ln -s ${LIBDIR}/${INSTALL_A_NAME} ${LIBDIR}/libAMoRE.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true


uninstall: uninstallHeaders
	@echo
	@echo removing libAMoRE library from ${INCLUDEDIR} ...
	@echo
	rm -f ${LIBDIR}/${INSTALL_SO_NAME}
	rm -f ${LIBDIR}/${INSTALL_A_NAME}
	rm -f ${LIBDIR}/libAMoRE.so
	rm -f ${LIBDIR}/libAMoRE.a
	-test `id -u` -eq 0 && /sbin/ldconfig; true


installHeaders:
	@echo
	@echo installing libAMoRE headers to ${INCLUDEDIR} ...
	@echo
	install -v -m 755 -d ${INCLUDEDIR}/amore
	install -v -m 644 include/amore/* ${INCLUDEDIR}/amore
uninstallHeaders:
	@echo
	@echo removing libAMoRE headers from ${INCLUDEDIR} ...
	@echo
	rm -Rf ${INCLUDEDIR}/amore


installDocs: doc
	@echo
	@echo installing libAMoRE doc to ${DOCDIR} ...
	@echo
	install -v -m 755 -d $(DOCDIR)/libAMoRE/html 
	install -v -m 644 docs/html/* $(DOCDIR)/libAMoRE/html
uninstallDocs:
	@echo
	@echo removing libAMoRE doc from ${DOCDIR} ...
	@echo
	rm -Rf ${DOCDIR}/libAMoRE

