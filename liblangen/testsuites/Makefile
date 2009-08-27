# $Id$
# liblangen (LANguageGENerator) testsuite Makefile

.PHONY: all clean dfa_enumerator dfa_randomgenerator nfa_randomgenerator regex_randomgenerator

all: dfa_enumerator dfa_randomgenerator nfa_randomgenerator regex_randomgenerator

dfa_enumerator:
	make -C dfa_enumerator

dfa_randomgenerator:
	make -C dfa_randomgenerator

nfa_randomgenerator:
	make -C nfa_randomgenerator

regex_randomgenerator:
	make -C regex_randomgenerator

clean:
	make -C dfa_enumerator clean
	make -C dfa_randomgenerator clean
	make -C nfa_randomgenerator clean
	make -C regex_randomgenerator clean
