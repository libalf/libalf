# $Id$
# libAMoRE++ testsuite Makefile

.PHONY: all clean amore_epsilon_closure  amore_serialization

all: amore_epsilon_closure  amore_serialization

amore_epsilon_closure:
	make -C amore_epsilon_closure

amore_serialization:
	make -C amore_serialization

clean:
	make -C amore_epsilon_closure clean
	make -C amore_serialization clean
	make -C normalizer clean
