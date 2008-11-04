# $Id$
# ASO testsuite Makefile

.PHONY: all clean run jpegs test

CPPFLAGS=-Wall -ggdb3  -I../../include

LDFLAGS=-L../../src -lalf -lAMoRE

all: learn_regex

clean:
	-rm *.o
	-rm learn_regex
	-rm *.dot
	-rm *.jpg
	-rm *.angluin
	-rm testlog

run:
	LD_LIBRARY_PATH=../../src ./learn_regex

jpegs:
	for EACH in *.dot; do dot -Tjpg "$$EACH" -o "$$EACH.jpg"; done;

learn_regex: learn_regex.cpp

