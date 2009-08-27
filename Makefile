# $Id$

.PHONY: all clean install

all:
	make -C libAMoRE
	make -C libAMoRE++
	make -C liblangen
	make -C libalf
	make -C finite-automata-tool

clean:
	make -C libAMoRE clean
	make -C libAMoRE++ clean
	make -C liblangen clean
	make -C libalf clean
	make -C finite-automata-tool clean

install:
	make -C libAMoRE install
	make -C libAMoRE++ install
	make -C liblangen install
	make -C libalf install
	make -C finite-automata-tool install

