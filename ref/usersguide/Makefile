all: gpstkusersguide


gpstkusersguide: gpstkusersguide.pdf gpstkusersguide.ps

gpstkusersguide.pdf: gpstkusersguide.dvi
	dvipdf gpstkusersguide.dvi

gpstkusersguide.ps: gpstkusersguide.dvi
	dvips gpstkusersguide.dvi

# Note latex is call twice. 
# This provides table of contents information, references, etc.
gpstkusersguide.dvi: gpstkusersguide.tex faq.tex gpsoverview.tex \
	gpstkusersguide.tex introduction.tex notices.tex rinexpvt.tex \
	rtashtech.tex titlepg.tex vecsol.tex wheresat.tex
	latex gpstkusersguide.tex
	latex gpstkusersguide.tex

clean:
	rm -f *.log
	rm -f *.aux
	rm -f *.toc
	rm -f *.dvi	
	rm -f *.pdf
	rm -f *~