# $Id$
# libalf Makefile

.PHONY: all clean algorithm_analysis alphabet angluin_col_table angluin_simple_table biermann DeLeTe2 kearns_vazirani NLstar_count_eq_queries NLstar_table normalizer rivest_shapire_table RPNI statistics simple-example

all: algorithm_analysis alphabet angluin_col_table angluin_simple_table biermann DeLeTe2 kearns_vazirani NLstar_count_eq_queries NLstar_table normalizer rivest_shapire_table RPNI conjecture statistics simple-example

algorithm_analysis:
	+make -C algorithm_analysis

alphabet:
	+make -C alphabet

angluin_col_table:
	+make -C angluin_col_table

angluin_simple_table:
	+make -C angluin_simple_table

biermann:
	+make -C biermann

conjecture:
	+make -C conjecture

DeLeTe2:
	+make -C DeLeTe2

kearns_vazirani:
	+make -C kearns_vazirani

NLstar_count_eq_queries:
	+make -C NLstar_count_eq_queries

NLstar_table:
	+make -C NLstar_table

normalizer:
	+make -C normalizer

rivest_shapire_table:
	+make -C rivest_shapire_table

RPNI:
	+make -C RPNI

statistics:
	+make -C statistics

simple-example:
	+make -C simple-example


clean:
	make -C algorithm_analysis clean
	make -C alphabet clean
	make -C angluin_col_table clean
	make -C angluin_simple_table clean
	make -C biermann clean
	make -C DeLeTe2 clean
	make -C kearns_vazirani clean
	make -C NLstar_count_eq_queries clean
	make -C NLstar_table clean
	make -C normalizer clean
	make -C RPNI clean
	make -C conjecture clean
	make -C statistics clean
	make -C simple-example clean

