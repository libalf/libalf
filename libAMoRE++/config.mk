# $Id$
# vim:syntax=make
#
# directory definitions for Makefile

CFLAGS += -O3
CPPFLAGS += -O3

DESTDIR?=""
PREFIX?=/usr/local

LIBDIR?=${PREFIX}/lib
INCLUDEDIR?=${PREFIX}/include
BINDIR?=${PREFIX}/bin
SHAREDIR?=${PREFIX}/share
DOCDIR?=${SHAREDIR}/doc/libAMoRE++

