# $Id$
# libalf Makefile

.PHONY: install all clean

all:	jalf.jar
	+make -C src
	
jalf.jar:
	ant

clean:
	make -C src clean
	rm -f jalf.jar

install: jalf.jar
	+make -C src install

uninstall:
	+make -C src uninstall

