# Makefile for TeX->pdf
#

TEX = /usr/bin/latex
.PHONY : all final clean

TEXTS = $(shell echo *.tex)

BIBS = $(shell echo *.bib)



all: libalf.pdf

clean:
	-rm libalf.out libalf.pdf libalf.aux libalf.log libalf.idx

libalf.pdf:
	pdflatex libalf.tex
	#-bibtex libalf
	pdflatex libalf.tex

