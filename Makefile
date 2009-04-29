# $Id$
# libalf Makefile

.PHONY: install-all-deps all testsuites clean install uninstall

PREFIX ?= /usr/local/

all:
	make -C src all
	make -C dispatcher all

install-all-deps: install
	make -C libAMoRE-1.0 install
	make -C libAMoRE++ install
	make -C LanguageGenerator install
	make -C JNI install

testsuites: install
	make -C testsuites

clean-all-deps: clean
	make -C libAMoRE-1.0 clean
	make -C libAMoRE++ clean
	make -C LanguageGenerator clean
	make -C JNI clean

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

