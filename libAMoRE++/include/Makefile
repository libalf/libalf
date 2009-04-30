# $Id$
# amore++ Makefile

.PHONY: install uninstall

PREFIX ?= /usr/local/

install:
	echo installing libAMoRE++ headers to ${PREFIX}/include/amore++
	mkdir -p ${PREFIX}/include/amore++/
	rm -f ${PREFIX}/include/amore++/*
	cp -Rv amore++/*.h ${PREFIX}/include/amore++/
	chmod 755 ${PREFIX}/include/amore++
	chmod 644 ${PREFIX}/include/amore++/*.h

uninstall:
	echo removing libAMoRE++ headers from ${PREFIX}/include/amore++
	rm -Rf ${PREFIX}/include/amore++

