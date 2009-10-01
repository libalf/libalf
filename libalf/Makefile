# $Id$
# libalf Makefile

.PHONY: install-all-deps all testsuites clean install uninstall

PREFIX ?= /usr/local/

all:
	+make -C src all
	+make -C dispatcher all

testsuites: install
	make -C testsuites

clean:
	+make -C src clean
	+make -C dispatcher clean
	+make -C testsuites clean

install:
	+make -C include install
	+make -C src install
	+make -C dispatcher install

uninstall:
	make -C include uninstall
	make -C src uninstall
	make -C dispatcher uninstall

