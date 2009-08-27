# $Id$
# liblangen (LANguageGENerator) Makefile

.PHONY: install uninstall

PREFIX ?= /usr/local/

install:
	echo installing liblangen headers to ${PREFIX}/include/liblangen...
	mkdir -p ${PREFIX}/include/liblangen/
	rm -f ${PREFIX}/include/liblangen/*
	cp -Rv liblangen/*.h ${PREFIX}/include/liblangen/
	chmod 755 ${PREFIX}/include/liblangen
	chmod 644 ${PREFIX}/include/liblangen/*.h

uninstall:
	echo removing liblangen headers from ${PREFIX}/include/liblangen...
	rm -Rf ${PREFIX}/include/liblangen

