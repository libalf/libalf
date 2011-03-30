# $Id$
# amore++ Makefile

.PHONY: install uninstall

PREFIX?=/usr/local
LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
DOCDIR?=${PREFIX}/share/doc

install:
	@echo
	@echo installing libAMoRE++ headers to ${INCLUDEDIR} ...
	@echo
	-install -v -m 755 -d ${INCLUDEDIR}/amore++
	rm -f ${INCLUDEDIR}/amore++/*
	install -v -m 644 amore++/*.h ${INCLUDEDIR}/amore++

uninstall:
	@echo
	@echo removing libAMoRE++ headers from ${INCLUDEDIR} ...
	@echo
	rm -Rf ${INCLUDEDIR}/amore++

