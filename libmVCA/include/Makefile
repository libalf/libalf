# $Id$
# libmVCA Makefile

.PHONY: install uninstall

include ../config.mk

install:
	@echo
	@echo installing libmVCA headers to ${DESTDIR}${INCLUDEDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${INCLUDEDIR}/libmVCA
	rm -f ${DESTDIR}${INCLUDEDIR}/libmVCA/*
	install -v -m 644 libmVCA/*.h ${DESTDIR}${INCLUDEDIR}/libmVCA

uninstall:
	@echo
	@echo removing libmVCA headers from ${DESTDIR}${INCLUDEDIR} ...
	@echo
	rm -Rf ${DESTDIR}${INCLUDEDIR}/libmVCA

