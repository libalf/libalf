# $Id$
# Makefile for libalf interfaces

.PHONY: all clean install uninstall testsuites

all:
	+make -C dispatcher all
	+make -C jalf all

testsuites:
	make -C testsuites

clean:
	make -C dispatcher clean
	make -C jalf clean
	make -C testsuites clean

install:
	+make -C dispatcher install

uninstall:
	+make -C dispatcher uninstall


