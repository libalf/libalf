# $Id$
# libalf Makefile

.PHONY: install uninstall

include ../config.mk

install:
	@echo
	@echo installing libalf headers to ${INCLUDEDIR} ...
	@echo
	-install -v -m 755 -d ${INCLUDEDIR}/libalf
	rm -f ${INCLUDEDIR}/libalf/*
	install -v -m 644 libalf/*.h ${INCLUDEDIR}/libalf 
	install -v -m 644 amore_alf_glue.h mVCA_alf_glue.h ${INCLUDEDIR}/

uninstall:
	@echo
	@echo removing libalf headers from ${INCLUDEDIR}/libalf...
	@echo
	rm -Rf ${INCLUDEDIR}/libalf
	rm -Rf ${INCLUDEDIR}/amore_alf_glue.h
	rm -Rf ${INCLUDEDIR}/mVCA_alf_glue.h

