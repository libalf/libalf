# $Id$
# libAMoRE++ testsuite Makefile

.PHONY: all clean amore_epsilon_closure amore_serialization antichain

all: amore_epsilon_closure amore_serialization antichain

amore_epsilon_closure:
	make -C amore_epsilon_closure

amore_serialization:
	make -C amore_serialization

antichain:
	make -C antichain

clean:
	make -C amore_epsilon_closure clean
	make -C amore_serialization clean
	make -C antichain clean
