	configfile.txt accompanies testscript.pl allowing for easy update of new and old tests and a solid infrastructure for adding new test options.  Any line beginning in a “#” or “!” character will be skipped when read by testscript.pl, so that comments can be easily added and the configfile read at a glance.  
	There are two parts for each line(s) containing a test program.  The first part is what would be executed on the command line when running the test program by itself, including all command line options.  Directories should be replaced with variables including:
$datdir - development directory defined by the user + /gpstk/dev/tests/data
$outdir – output directory defined by the user
$exedir – development directory defined by the user + /gpstk/dev/tests

	The second part of the configuration line is separated from the first part by a comma.  After the comma, if a difference is necessary then the two files which need to be differenced should be typed next to one and other with a colon separating them. (for example:  stringutiltest.scr:stringutiltest.ref)  Since the test script automatically creates a file named stringutiltest.scr, and stringutiltest makes its output show in stout this is the proper formation for a difference.  However be aware if a test, for example Xbegweek, creates its own .out file.  That .out file is the one that should be diffed and not the .scr file.
	If a text in the configfile is too long to be viewed on only one line then the “\” backslash character denotes a continuation of the text on the following line.  If the backslash is used then the following line should be indented to align with the space after the end of the test program name on the primary line.  This syntactical design allows for much easier viewing of the configfile. 
(example:
MatrixTest $datdir/anotsym.dat $datdir/cov.dat $datdir/dia.dat $datdir/lt.dat\
                   $datdir/partials.dat $datdir/squ.dat $datdir/sym.dat $datdir/tmatrix.dat\
                   $datdir/ut.dat, MatrixTest.scr:MatrixTest.ref; 
)

