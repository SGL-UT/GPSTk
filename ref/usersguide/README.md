Date Created: July 19, 2012
Author: Preston Herrmann

Generating a PDF document from the Latex source files
-----------------------------------------------------

	To generate a PDF version of the GPSTk User's Guide the source files must be compiled using a combination of perltex and latex. The included makefile in turn calls another included script called 'makescr' which executes the perltex script on the source files to generate the proper Latex files. This perltex command is executed several times within the script to generate sections such as the index and table of contents. After the script outputs the .dvi file the file is then converted to PDF using the 'dvipdf' application. Once the PDF file is generated the script ends. 

Compilation Instructions
------------------------
1. Open a terminal window.
2. Change directories into the /usersguide directory.
3. Run 'make all' from the command line.
4. OPTIONAL: To remove the auxiliary and bibliography files from the usersguide directory, run the command 'make clean'

	If the source files compile without any major errors the gpstk-user-reference.pdf file will be generated. If there are any errors in the source code the make script with prompt the command line for input on how to handle each error. ideally there should be no errors when the makefile script is run. To output the command errors to a text file for easier reference use the command 'make all > makefileOutput.txt'
