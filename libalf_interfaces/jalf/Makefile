# $Id$
# libalf Makefile

.PHONY: install all clean

include ../config.mk

VERSIONTAG:=$(shell cat "../version" 2>/dev/null)
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
	@echo installing jalf.jar to ${DESTDIR}${JARDIR} ...
	@echo
	-install -v -m 755 -d ${DESTDIR}${JARDIR}
	install -T -v -m 755 jalf.jar ${DESTDIR}${JARDIR}/${INSTALL_JAR_NAME}
	# symlinks
	-rm -f ${DESTDIR}${JARDIR}/jalf.jar 2>&1
	ln -s ${JARDIR}/${INSTALL_JAR_NAME} ${DESTDIR}${JARDIR}/jalf.jar

uninstall:
	+make -C src uninstall
	@echo
	@echo removing jalf.jar from ${DESTDIR}${JARDIR} ...
	@echo
	rm -f ${DESTDIR}${JARDIR}/${INSTALL_JAR_NAME}
	rm -f ${DESTDIR}${JARDIR}/jalf.jar

