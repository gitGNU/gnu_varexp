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

all:	user-manual.html user-manual.pdf

user-manual.html user-manual.pdf: ../VERSION peti.dsl

clean distclean:
	@rm -f *.aux *.dvi *.log *.toc
	@rm -f *.css *.fot *.ps *.xml
	@rm -f *.tex

realclean:	clean
	@rm -f *.pdf *.html

