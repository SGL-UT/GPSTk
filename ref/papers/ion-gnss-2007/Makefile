ARTICLENAME=gpstk-ion-gnss-2007
ARTICLEDEPS=gpstk-ion-gnss-2007.tex toolkitapps.tex samplePlot.eps gpstk.bib \
	gpstk-directory-structures.eps crossplatformapps.eps
BIBTEX=gpstk.bib

all: $(ARTICLENAME).pdf handouts

handouts: toolkitapps.tex crossplatformapps.tex
	pdflatex crossplatformapps.tex
	pdflatex platformspecificapps.tex	

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
	rm -f $(ARTICLENAME).pdf crossplatformapps.pdf platformspecificapps.pdf
	rm -f *.bbl
	rm -f *.blg
	rm -f *.log
	rm -f *.aux
	rm -f *.toc
	rm -f *.dvi	
	rm -f *~



