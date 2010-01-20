ARTICLENAME=gpstk-ion-gnss-2009
ARTICLEDEPS=gpstk-ion-gnss-2009.tex bio.tex abstract.tex intro.tex   \
	prior.tex projsummary.tex liboverview.tex librinex3.tex libplot.tex \
	libprocframe.tex appsclock.tex \
	appsdataloss.tex future.tex acknowledgements.tex UML0709.eps \
	topoavail-nga-kor.eps deps.eps calgps.eps clockapps.eps
BIBTEX=gpstk.bib

all: $(ARTICLENAME).pdf 

$(ARTICLENAME).pdf: $(ARTICLENAME).dvi 
	dvips -t letter -Ppdf $(ARTICLENAME).dvi
	ps2pdf14 $(ARTICLENAME).ps

# Note multiple calls to latex are intentional.  
# This provides table of contents information, references, etc.
$(ARTICLENAME).dvi: $(ARTICLEDEPS) $(BIBTEX) ion-gps.cls
	latex $(ARTICLENAME).tex
	bibtex $(ARTICLENAME)
	latex $(ARTICLENAME).tex
	latex $(ARTICLENAME).tex
	latex $(ARTICLENAME).tex


clean:
	rm -f $(ARTICLENAME).pdf 
	rm -f *.bbl
	rm -f *.blg
	rm -f *.log
	rm -f *.aux
	rm -f *.toc
	rm -f *.dvi	
	rm -f *~
	rm -f $(ARTICLENAME).ps



