# $Id$
# libalf Makefile

.PHONY: all clean angluin_simple_observationtable NLstar_simple_table biermann normalizer

all: angluin_simple_observationtable NLstar_simple_table biermann normalizer

angluin_simple_observationtable:
	make -C angluin_simple_observationtable

NLstar_simple_table:
	make -C NLstar_simple_table

biermann:
	make -C biermann

normalizer:
	make -C normalizer

clean:
	make -C angluin_simple_observationtable clean
	make -C NLstar_simple_table clean
	make -C biermann clean
	make -C normalizer clean
