# $Id$
# libalf Makefile

.PHONY: install uninstall

include ../config.mk

install:
	@echo
	@echo installing libalf headers to ${DESTDIR}${INCLUDEDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${INCLUDEDIR}/libalf
	rm -f ${DESTDIR}${INCLUDEDIR}/libalf/*
	install -v -m 644 libalf/*.h ${DESTDIR}${INCLUDEDIR}/libalf 
	install -v -m 644 amore_alf_glue.h mVCA_alf_glue.h ${DESTDIR}${INCLUDEDIR}/

uninstall:
	@echo
	@echo removing libalf headers from ${DESTDIR}${INCLUDEDIR}/libalf...
	@echo
	rm -Rf ${DESTDIR}${INCLUDEDIR}/libalf
	rm -Rf ${DESTDIR}${INCLUDEDIR}/amore_alf_glue.h
	rm -Rf ${DESTDIR}${INCLUDEDIR}/mVCA_alf_glue.h

