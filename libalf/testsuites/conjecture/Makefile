# $Id$
# conjecture testsuite Makefile

.PHONY: all clean run jpegs test

PREFIX ?= /usr/local

CPPFLAGS+=-I../../include -I${PREFIX}/include -Wall -Wextra -ggdb3

LDFLAGS+=-L../../src -L${PREFIX}/lib -lalf

all: simple_moore_machine_from_file

clean: nodata
	-rm -f *.o simple_moore_machine_from_file

nodata:
	-rm -f *.dot *.jpg *.ser

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

simple_moore_machine_from_file: simple_moore_machine_from_file.cpp

