# $Id$
# libalf dispatcher Makefile

.PHONY: all clean install uninstall

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

all:
	+make -C src all

clean:
	make -C src clean

install:
	+make -C include install
	+make -C src install

uninstall:
	make -C src uninstall
	make -C include uninstall

