all: draft01

draft01: draft01.dvi draft01.pdf

draft01.dvi: firstprototype.tex appsoverview.tex faq.tex gpsoverview.tex gpstklogo.eps introduction.tex notices.tex rinexpvt.tex rtashtech.tex titlepg.tex vecsol.tex wheresat.tex
	latex firstprototype
	latex firstprototype

draft01.pdf: draft01.dvi
	dvipdf firstprototype.dvi

clean:
	rm -f *.aux
	rm -f *.bbl
	rm -f *.blg
	rm -f *.log
	rm -f *.pdf
	rm -f *.toc
	rm -f *.dvi
 
