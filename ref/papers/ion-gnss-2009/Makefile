ARTICLENAME=gpstk-ion-gnss-2009
ARTICLEDEPS=gpstk-ion-gnss-2009.tex bio.tex abstract.tex intro.tex   \
	projsummary.tex liboverview.tex librinex3.tex libplot.tex    \
	libprocframe.tex appsoverview.tex appsclock.tex appsswrx.tex \
	appsdataloss.tex future.tex acknowledgements.tex
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



