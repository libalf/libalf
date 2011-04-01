# $Id$
# liblangen (LANguageGENerator) Makefile

.PHONY: install uninstall

include ../config.mk

install:
	@echo
	@echo installing liblangen headers to ${INCLUDEDIR} ...
	@echo
	-install -v -m 755 -d ${INCLUDEDIR}/liblangen
	rm -f ${INCLUDEDIR}/liblangen/*
	install -v -m 644 liblangen/*.h ${INCLUDEDIR}/liblangen

uninstall:
	@echo
	@echo removing liblangen headers from ${INCLUDEDIR} ...
	@echo
	rm -Rf ${INCLUDEDIR}/liblangen

