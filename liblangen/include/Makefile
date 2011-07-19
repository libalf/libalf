# $Id$
# liblangen (LANguageGENerator) Makefile

.PHONY: install uninstall

include ../config.mk

install:
	@echo
	@echo installing liblangen headers to ${DESTDIR}${INCLUDEDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${INCLUDEDIR}/liblangen
	rm -f ${DESTDIR}${INCLUDEDIR}/liblangen/*
	install -v -m 644 liblangen/*.h ${DESTDIR}${INCLUDEDIR}/liblangen

uninstall:
	@echo
	@echo removing liblangen headers from ${DESTDIR}${INCLUDEDIR} ...
	@echo
	rm -Rf ${DESTDIR}${INCLUDEDIR}/liblangen

