# $Id$
# amore++ Makefile

.PHONY: all clean install uninstall

PREFIX ?= /usr/local/

all:
	make -C src all

clean:
	make -C src clean

install:
	make -C include install
	make -C src install

uninstall:
	make -C include uninstall
	make -C src uninstall

