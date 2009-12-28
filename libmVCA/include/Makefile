# $Id$
# libmVCA Makefile

.PHONY: install uninstall

PREFIX ?= /usr/local/

install:
	echo installing libmVCA headers to ${PREFIX}/include/libmVCA...
	mkdir -p ${PREFIX}/include/libmVCA/
	rm -f ${PREFIX}/include/libmVCA/*
	cp -Rv libmVCA/*.h ${PREFIX}/include/libmVCA/
	chmod 755 ${PREFIX}/include/libmVCA
	chmod 644 ${PREFIX}/include/libmVCA/*.h

uninstall:
	echo removing libmVCA headers from ${PREFIX}/include/libmVCA...
	rm -Rf ${PREFIX}/include/libmVCA

