#
#  libAMoRE Makefile
#
#  Copyright (c) 2008, 2009 - David R. Piegdon <david-i2@piegdon.de>
#
############################################################################

PREFIX ?= /usr/local

LIBDIR ?= ${PREFIX}/lib
DOCDIR ?= ${PREFIX}/share/doc
HEADERDIR ?= ${PREFIX}/include

LOCALCHANGES := $(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION := $(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION ?= libalf-svn-r${SVNREVISION}${LOCALCHANGES}

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

docs/html/index.html:
	$(MAKE) doc

doc:
	doxygen docs/Doxyfile-libAMoRE

install: exportHeaders
	+make all
	cp libAMoRE.a $(LIBDIR)
	chmod 644 $(LIBDIR)/libAMoRE.a
	cp libAMoRE.so $(LIBDIR)
	#strip $(LIBDIR)/libAMoRE.so
	chmod 644 $(LIBDIR)/libAMoRE.so
	-test `id -u` -eq 0 && /sbin/ldconfig; true

install-docs: docs/html/index.html
	install -d $(DOCDIR)/libAMoRE/html 
	install docs/html/* $(DOCDIR)/libAMoRE/html

uninstall:
	rm -f $(LIBDIR)/libAMoRE.a
	rm -f $(HEADERDIR)/amore/*.h
	-test `id -u` -eq 0 && /sbin/ldconfig; true

exportHeaders:
	install -d $(HEADERDIR)/amore
	cp include/amore/* $(HEADERDIR)/amore
	chmod 644 ${HEADERDIR}/amore/*

clean:
	-rm -f *~ */*~ $(OBJ) libAMoRE.a libAMoRE.so
	-rm -Rf docs/html docs/latex 

