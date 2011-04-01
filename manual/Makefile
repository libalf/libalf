# Makefile for TeX->pdf
#

include config.mk

TEX = /usr/bin/latex
.PHONY : all final clean

TEXTS = $(shell echo *.tex)

BIBS = $(shell echo *.bib)



all: libalf.pdf

clean:
	-rm libalf.out libalf.pdf libalf.aux libalf.log libalf.idx libalf.lof libalf.toc libalf.bbl libalf.blg libalf.lot

libalf.pdf:
	pdflatex libalf.tex
	#-bibtex libalf
	pdflatex libalf.tex

install: libalf.pdf
	@echo
	@echo installing libalf manual to ${DOCDIR} ...
	@echo
	-install -v -m 755 -d ${DOCDIR}
	install -v -m 755 libalf.pdf ${DOCDIR}

uninstall:
	@echo
	@echo removing libalf manual from ${DOCDIR} ...
	@echo
	rm -f ${DOCDIR}/libalf.pdf
	-rmdir ${DOCDIR}

