# $Id$
# amore++ Makefile

.PHONY: install uninstall

include ../config.mk

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

