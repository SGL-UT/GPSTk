all: firstprototype userref

userref: chap2.pdf

chap2.pdf: chap2.tex  
	src/reftex5 chap2.tex src/gpstk.bib
	latex chap2.tex
	latex chap2.tex
	dvipdf chap2.dvi

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
	rm -f *.dvi	
	rm -f *.pdf
	rm -f *~
