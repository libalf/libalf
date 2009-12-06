# $Id$
# antichain testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lAMoRE++

all: lang_eq lang_subseteq massive_antichain
#	lang_universal

clean: nodata
	-rm -f *.o lang_subseteq lang_eq lang_universal massive_antichain

nodata:
	-rm -f *.dot *.jpg *-info

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

lang_eq: lang_subseteq.cpp
	g++ ${CPPFLAGS} ${LDFLAGS} lang_subseteq.cpp -o lang_eq

lang_subseteq: lang_subseteq.cpp
lang_subseteq: CPPFLAGS += -DSUBSET

massive_antichain: massive_antichain.cpp
massive_antichain: LDFLAGS += -llangen

#lang_universal: lang_universal.cpp

