# $Id$
# libalf Makefile

.PHONY: all clean install uninstall

PREFIX ?= /usr/local/

all:
	make -C src all
	make -C dispatcher all

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

