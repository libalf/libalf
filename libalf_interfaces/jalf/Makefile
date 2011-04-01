# $Id$
# libalf Makefile

.PHONY: install all clean

include ../config.mk

VERSIONTAG:=$(shell cat "../version")
ifeq (${VERSIONTAG},)
# subversion branch
LOCALCHANGES:=$(shell test "`svn status | wc -c`" != "0" && echo "-LocalChanges")
SVNREVISION:=$(shell LC_ALL=POSIX svn info | awk '/^Revision:/ {print $$2}')
VERSION?=svn-r${SVNREVISION}${LOCALCHANGES}
LIBVERSIONTAG?=.svn
else
# release branch
VERSION?=v${VERSIONTAG}
LIBVERSIONTAG?=.${VERSIONTAG}
endif

INSTALL_JAR_NAME=jalf.jar${LIBVERSIONTAG}



all:	jalf.jar
	+make -C src
	
jalf.jar:
	ant

clean:
	make -C src clean
	rm -f jalf.jar

install: jalf.jar
	+make -C src install
	@echo
	@echo installing jalf.jar to ${JARDIR} ...
	@echo
	-install -v -m 755 -d ${JARDIR}
	install -T -v -m 755 jalf.jar ${JARDIR}/${INSTALL_JAR_NAME}
	# symlinks
	-rm -f ${JARDIR}/jalf.jar
	ln -s ${JARDIR}/${INSTALL_JAR_NAME} ${JARDIR}/jalf.jar

uninstall:
	+make -C src uninstall
	@echo
	@echo removing jalf.jar from ${JARDIR} ...
	@echo
	rm -f ${JARDIR}/${INSTALL_JAR_NAME}
	rm -f ${JARDIR}/jalf.jar

