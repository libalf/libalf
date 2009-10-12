# $Id$
# libalf Makefile

.PHONY: install uninstall

PREFIX ?= /usr/local/

install:
	echo installing libalf headers to ${PREFIX}/include/libalf...
	mkdir -p ${PREFIX}/include/libalf/
	rm -f ${PREFIX}/include/libalf/*
	cp -Rv libalf/*.h ${PREFIX}/include/libalf/
	cp -Rv amore_alf_glue.h ${PREFIX}/include/amore_alf_glue.h
	chmod 755 ${PREFIX}/include/libalf
	chmod 644 ${PREFIX}/include/libalf/*.h

uninstall:
	echo removing libalf headers from ${PREFIX}/include/libalf...
	rm -Rf ${PREFIX}/include/libalf
	rm -Rf ${PREFIX}/include/amore_alf_glue.h

