#
# Build the libvarexp documentation
#
DOCBOOKBASE	= /usr/local/sgml/dsssl/docbook

.SUFFIXES:
.SUFFIXES: .sgml .tex .txt .dvi .ps .pdf .html

.sgml.tex:
	openjade -t tex -d peti.dsl#print -D $(DOCBOOKBASE)/print $<

.sgml.html:
	openjade -t sgml -d peti.dsl#html -D $(DOCBOOKBASE)/html $< >$@

.html.txt:
	w3m -T text/html -dump $< >$@

.tex.dvi:
	jadetex \\nonstopmode\\input $<;
	jadetex \\nonstopmode\\input $<;
	jadetex \\nonstopmode\\input $<;

.dvi.ps:
	dvips $< -o $@

.dvi.pdf:
	dvipdf $< $@

all:	user-manual.html user-manual.pdf readme.txt

user-manual.html user-manual.tex: ../VERSION peti.dsl operations-on-variables.inc
user-manual.html user-manual.tex: arrays-of-variables.inc ebnf.inc looping.inc
user-manual.html user-manual.tex: purpose.inc quoted-pairs.inc variable-expressions.inc

readme.html:	peti.dsl purpose.inc

clean distclean:
	@rm -f *.aux *.dvi *.log *.toc
	@rm -f *.fot *.ps *.xml
	@rm -f *.tex

realclean:	clean
	@rm -f *.pdf *.html *.txt
