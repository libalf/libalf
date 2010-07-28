# $Id: Makefile 823 2009-09-04 14:01:27Z davidpiegdon $
# libalf Makefile

.PHONY: install all clean

all:	jalf.jar
	+make -C src all

clean:
	+make -C src clean
	-rm jalf.jar

jalf.jar:
	ant

