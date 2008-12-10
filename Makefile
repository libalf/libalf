# $Id$
# libalf Makefile

.PHONY: all clean install uninstall

PREFIX ?= /usr/local/

all:
	make -C src all
	make -C testsuites all
	make -C dispatcher all

clean:
	make -C src clean
	make -C testsuites clean
	make -C dispatcher clean

install:
	make -C include install
	make -C src install
	make -C dispatcher install

uninstall:
	make -C src uninstall
	make -C include uninstall
	make -C dispatcher uninstall

