all: firstprototype gpstk-user-reference

gpstk-user-reference: gpstk-user-reference.pdf

gpstk-user-reference.pdf: gpstk-user-reference.tex chap2.tex sec2.tex calgps.tex poscvt.tex timeconvert.tex gpstk.bib
	./makescr
	dvipdf gpstk-user-reference

firstprototype: firstprototype.pdf firstprototype.ps

firstprototype.pdf: firstprototype.dvi
	dvipdf firstprototype.dvi

firstprototype.ps: firstprototype.dvi
	dvips -o firstprototype.ps firstprototype.dvi

# Note latex is call twice. 
# This provides table of contents information, references, etc.
firstprototype.dvi: firstprototype.tex faq.tex gpsoverview.tex \
	firstprototype.tex introduction.tex notices.tex rinexpvt.tex \
	rtashtech.tex titlepg.tex vecsol.tex wheresat.tex
	latex firstprototype.tex
	latex firstprototype.tex

clean:
	rm -f *.log
	rm -f *.aux
	rm -f *.toc
	rm -f *.bbl
	rm -f *.blg
	rm -f *.lgpl
	rm -f *.dvi	
	rm -f *.pdf
	rm -f *.ps
	rm -f *~
	rm -f temp
	rm -f .log
