# $Id$
# libalf Makefile

.PHONY: all clean  algorithm_analysis alphabet angluin_col_table angluin_generic angluin_simple_table biermann biermann_original conjecture DeLeTe2 kearns_vazirani mVCA_angluinlike NLstar_count_eq_queries NLstar_table normalizer online_performance_tester rivest_shapire_table RPNI simple-example statistics

# all: mVCA_angluinlike excluded because algorithm is not ready yet.
all: algorithm_analysis alphabet angluin_col_table angluin_generic angluin_simple_table biermann biermann_original conjecture DeLeTe2 kearns_vazirani NLstar_count_eq_queries NLstar_table normalizer online_performance_tester rivest_shapire_table RPNI simple-example statistics

algorithm_analysis:
	+make -C algorithm_analysis

alphabet:
	+make -C alphabet

angluin_col_table:
	+make -C angluin_col_table

angluin_generic:
	+make -C angluin_generic

angluin_simple_table:
	+make -C angluin_simple_table

biermann:
	+make -C biermann

biermann_original:
	+make -C biermann_original

conjecture:
	+make -C conjecture

DeLeTe2:
	+make -C DeLeTe2

kearns_vazirani:
	+make -C kearns_vazirani

mVCA_angluinlike:
	+make -C mVCA_angluinlike

NLstar_count_eq_queries:
	+make -C NLstar_count_eq_queries

NLstar_table:
	+make -C NLstar_table

normalizer:
	+make -C normalizer

online_performance_tester:
	+make -C online_performance_tester

rivest_shapire_table:
	+make -C rivest_shapire_table

RPNI:
	+make -C RPNI

simple-example:
	+make -C simple-example

statistics:
	+make -C statistics


clean:
	make -C algorithm_analysis clean
	make -C alphabet clean
	make -C angluin_col_table clean
	make -C angluin_generic clean
	make -C angluin_simple_table clean
	make -C biermann clean
	make -C biermann_original clean
	make -C conjecture clean
	make -C DeLeTe2 clean
	make -C kearns_vazirani clean
	make -C mVCA_angluinlike clean
	make -C NLstar_count_eq_queries clean
	make -C NLstar_table clean
	make -C normalizer clean
	make -C online_performance_tester clean
	make -C rivest_shapire_table clean
	make -C RPNI clean
	make -C simple-example clean
	make -C statistics clean

