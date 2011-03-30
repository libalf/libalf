# $Id$
# libmVCA Makefile

.PHONY: install uninstall

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

install:
	@echo
	@echo installing libmVCA headers to ${INCLUDEDIR} ...
	@echo
	-install -v -m 755 -d ${INCLUDEDIR}/libmVCA
	rm -f ${INCLUDEDIR}/libmVCA/*
	install -v -m 644 libmVCA/*.h ${INCLUDEDIR}/libmVCA

uninstall:
	@echo
	@echo removing libmVCA headers from ${INCLUDEDIR} ...
	@echo
	rm -Rf ${INCLUDEDIR}/libmVCA

