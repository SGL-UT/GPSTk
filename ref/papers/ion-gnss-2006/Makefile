ARTICLENAME=gpstk-ion-gnss-2006
ARTICLEDEPS=gpstk-ion-gnss-2006.tex toolkitportrait.tex
BIBTEX=gpstk.bib

all: $(ARTICLENAME).pdf 

$(ARTICLENAME).pdf: $(ARTICLENAME).dvi 
	dvipdf $(ARTICLENAME).dvi

# Note latex is call twice. 
# This provides table of contents information, references, etc.
$(ARTICLENAME).dvi: $(ARTICLEDEPS) $(BIBTEX) ion-gps.cls
	latex $(ARTICLENAME).tex
	bibtex $(ARTICLENAME)
	latex $(ARTICLENAME).tex
	latex $(ARTICLENAME).tex
	latex $(ARTICLENAME).tex


clean:
	rm -f *.bbl
	rm -f *.blg
	rm -f *.log
	rm -f *.aux
	rm -f *.toc
	rm -f *.dvi	
	rm -f *.pdf
	rm -f *~



