# $Id$
# amore++ Makefile

.PHONY: install uninstall

include ../config.mk

install:
	@echo
	@echo installing libAMoRE++ headers to ${DESTDIR}${INCLUDEDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${INCLUDEDIR}/amore++
	rm -f ${DESTDIR}${INCLUDEDIR}/amore++/*
	install -v -m 644 amore++/*.h ${DESTDIR}${INCLUDEDIR}/amore++

uninstall:
	@echo
	@echo removing libAMoRE++ headers from ${DESTDIR}${INCLUDEDIR} ...
	@echo
	rm -Rf ${DESTDIR}${INCLUDEDIR}/amore++

