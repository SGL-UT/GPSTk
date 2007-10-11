
	testscript.pl is a script accompanied by configfile.txt, which contains a list of test programs and their associated command line options.  testscript.pl runs through every line of the configfile building and extending the proper command line for each test.

>>testscript.pl <development directory>  <configfile>  <output directory>  [<html file and directory path>]

<development directory> - This is the directory path that the GPSTk is installed in. 

<configfile> - The file path and name of the configuration file.

<output directory> - The output directory is the directory that all .dif and .scr files are sent to.  This not only allows for better archiving but also for easier cleanup at the end of testing so as not to waste disc space.  The test script outputs to screen the names of all tests executed and which ones failed in the execution or in differencing.  If a test does fail it can be seen in brief in the testscript.log file located in the output directory. For more details look at the .scr and .dif files.

[<html file and directory path>] - The optional html directory path saves information in a format that can be used in an html script to help create a graphical table of information showing test results, the machine in which the tests were run on and they type of processor.

	As  the tests are executed the stout results are written to a file named by the name of the test program + .scr on the end.  If necessary, the results are differenced against known data in the data directory.  The result of the differencing, even if there it is null, is placed into a file with the .dif extension. The .dif files are also named like the .scr, with the name of the test program + .dif on the end.
SEE configfile help for details on proper syntax for the test script to be able to sync with the configfile.
 
  

