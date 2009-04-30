# $Id$
# liblangen Makefile

.PHONY: install uninstall

PREFIX ?= /usr/local/

install:
	echo installing liblangen headers to ${PREFIX}/include/LanguageGenerator...
	mkdir -p ${PREFIX}/include/LanguageGenerator/
	rm -f ${PREFIX}/include/LanguageGenerator/*
	cp -Rv LanguageGenerator/*.h ${PREFIX}/include/LanguageGenerator/
	chmod 755 ${PREFIX}/include/LanguageGenerator
	chmod 644 ${PREFIX}/include/LanguageGenerator/*.h

uninstall:
	echo removing liblangen headers from ${PREFIX}/include/LanguageGenerator...
	rm -Rf ${PREFIX}/include/LanguageGenerator

