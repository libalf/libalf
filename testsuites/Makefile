# $Id$
# libalf Makefile

.PHONY: all clean angluin_simple_observationtable amore_epsilon_closure amore_serialization structured_query_tree

all: angluin_simple_observationtable amore_serialization amore_epsilon_closure structured_query_tree

angluin_simple_observationtable:
	make -C angluin_simple_observationtable

amore_epsilon_closure:
	make -C amore_epsilon_closure

amore_serialization:
	make -C amore_serialization

structured_query_tree:
	make -C structured_query_tree

clean:
	make -C angluin_simple_observationtable clean
	make -C amore_epsilon_closure clean
	make -C amore_serialization clean
