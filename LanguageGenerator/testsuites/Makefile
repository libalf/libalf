# $Id$
# LanguageGenerator testsuite Makefile

.PHONY: all clean DFAenumerator DFArandomgenerator regex_randomgenerator

all: DFAenumerator DFArandomgenerator regex_randomgenerator

DFAenumerator:
	make -C DFAenumerator

DFArandomgenerator:
	make -C DFArandomgenerator

regex_randomgenerator:
	make -C regex_randomgenerator

clean:
	make -C DFArandomgenerator clean
	make -C regex_randomgenerator clean
