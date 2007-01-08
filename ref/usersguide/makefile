all: gpstk-user-reference

gpstk-user-reference: gpstk-user-reference.dvi gpstk-user-reference.pdf

gpstk-user-reference.dvi: gpstk-user-reference.tex sec2.tex calgps.tex poscvt.tex timeconvert.tex gpstk.bib src/cmd.tex RinexDump.tex navdmp.tex vecsol.tex DiscFix.tex
	./makescr

gpstk-user-reference.pdf: gpstk-user-reference.dvi
	dvipdf gpstk-user-reference

clean:
	rm -f *.log
	rm -f *.aux
	rm -f *.toc
	rm -f *.bbl
	rm -f *.blg
	rm -f *.lgpl
	rm -f *.dvi	
	rm -f *.ps
	rm -f *~
	rm -f temp
	rm -f .log
	rm -f *.dfpl
	rm -f *.frpl
	rm -f *.topl
	rm -f *.tfpl
	rm -f *.ind
	rm -f *.ilg
	rm -f *.idx
