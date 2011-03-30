# $Id$
# amore++ Makefile

.PHONY: all testsuites clean install uninstall

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

all:
	+make -C src all

testsuites:
	make -C testsuites

clean:
	+make -C src clean
	+make -C testsuites clean

install:
	+make -C include install
	+make -C src install

uninstall:
	make -C include uninstall
	make -C src uninstall

