# $Id$
# libalf Makefile

.PHONY: all clean angluin_simple_observationtable biermann normalizer

all: angluin_simple_observationtable biermann normalizer

angluin_simple_observationtable:
	make -C angluin_simple_observationtable

biermann:
	make -C biermann

normalizer:
	make -C normalizer

clean:
	make -C angluin_simple_observationtable clean
	make -C amore_epsilon_closure clean
	make -C amore_serialization clean
	make -C biermann clean
	make -C normalizer clean
