# $Id$
# libalf Makefile

.PHONY: all clean install uninstall

all:
	make -C src all

clean:
	make -C src clean

install:
	make -C include install
	make -C src install

uninstall:
	make -C src uninstall
	make -C include uninstall

