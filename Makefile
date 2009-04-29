# $Id$
# libalf Makefile

.PHONY: install-all-deps all testsuites clean install uninstall

PREFIX ?= /usr/local/

all:
	make -C src all
	make -C dispatcher all

install-all-deps:
	make -C libAMoRE-1.0 install
	make -C libAMoRE++ install
	make -C LanguageGenerator install
	make all
	make -C JNI install

testsuites: install
	make -C testsuites

clean:
	make -C src clean
	make -C dispatcher clean
	make -C testsuites clean

install:
	make -C include install
	make -C src install
	make -C dispatcher install

uninstall:
	make -C include uninstall
	make -C src uninstall
	make -C dispatcher uninstall

