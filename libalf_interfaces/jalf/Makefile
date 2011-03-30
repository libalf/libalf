# $Id$
# libalf Makefile

.PHONY: install all clean

all:	jalf.jar
	+make -C src all

clean:
	+make -C src clean
	-rm jalf.jar

jalf.jar:
	ant

