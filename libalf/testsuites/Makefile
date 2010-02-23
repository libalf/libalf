# $Id$
# libalf Makefile

.PHONY: all clean algorithm_analysis alphabet angluin_col_table angluin_simple_table biermann DeLeTe2 kearns_vazirani NLstar_count_eq_queries NLstar_table normalizer RPNI

all: algorithm_analysis alphabet angluin_col_table angluin_simple_table biermann DeLeTe2 NLstar_count_eq_queries NLstar_table normalizer RPNI

algorithm_analysis:
	make -C algorithm_analysis

alphabet:
	make -C alphabet

angluin_col_table:
	make -C angluin_col_table

angluin_simple_table:
	make -C angluin_simple_table

biermann:
	make -C biermann

DeLeTe2:
	make -C DeLeTe2

kearns_vazirani:
	make -C kearns_vazirani

NLstar_count_eq_queries:
	make -C NLstar_count_eq_queries

NLstar_table:
	make -C NLstar_table

normalizer:
	make -C normalizer

RPNI:
	make -C RPNI

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

