############################################################################

#  Makefile         libAMoRE

############################################################################

#include ../conf.mk

LIBDIR?=/usr/lib
DOCDIR?=/usr/share/doc
HEADER_INSTALLDIR?=/usr/include
#-ggdb3
CFLAGS+= -DDEBUG -DJAVAEXCEPT -DLIBAMORE -DUNIX -DLINUX -DBIT32
CFLAGS+= -Wall
CFLAGS+= -Iinclude
CFLAGS+= -fno-stack-protector
CPPFLAGS+= -shared -fpic

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
	ar rs $@ $?

libAMoRE.so: $(OBJ)
	ld -o $@ -shared -fpic $?

docs/html/index.html:
	$(MAKE) doc

doc:
	doxygen docs/Doxyfile-libAMoRE

install: exportHeaders
	cp libAMoRE.a $(LIBDIR)
	chmod 644 $(LIBDIR)/libAMoRE.a
	cp libAMoRE.so $(LIBDIR)
	#strip $(LIBDIR)/libAMoRE.so
	chmod 644 $(LIBDIR)/libAMoRE.so

install-docs: docs/html/index.html
	install -d $(DOCDIR)/libAMoRE/html 
	install docs/html/* $(DOCDIR)/libAMoRE/html

uninstall:
	rm -f $(LIBDIR)/libAMoRE.a
	rm -f $(HEADER_INSTALLDIR)/amore/*.h

exportHeaders:
	install -d $(HEADER_INSTALLDIR)/amore
	cp include/amore/* $(HEADER_INSTALLDIR)/amore
	chmod 644 ${HEADER_INSTALLDIR}/amore/*

clean:
	rm -f *~ */*~ $(OBJ) libAMoRE.a libAMoRE.so
	rm -Rf docs/html docs/latex 
