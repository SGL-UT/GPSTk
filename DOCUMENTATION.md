DOCUMENTATION
-------------

Contents:
---------

* Doxygen API Documentation - Automated Build
* Doxygen API Documentation - Manual Build
* LaTex User's Reference Manual PDF
* Application Folder Documentation


Doxygen API Documentation - Automated Build:
------------------------------------------------------------------------

   1. Navigate to the root directory of the extract GPSTk file tree, herein called $gpstk_root:

        $ cd $gpstk_root
         
   2a. Execute the build script to build the Core Doxygen API Documentation
        
        $ ./build.sh -d 
   
   2b. Execute the build script to build the Complete (Core/Ext) Doxygen API Documentation & 
       Improved Python Docstrings
        
        $ ./build.sh -de


Doxygen API Documentation - Manual Build:
----------------------------------------------------------------------

   1. Navigate to the root directory of the extract GPSTk file tree:

         $ cd $gpstk_root

         $ doxygen 


LaTex User's Reference Manual PDF:
----------------------------------

To generate a PDF version of the GPSTk User's Guide, the source files must be compiled using a 
combination of perltex and latex. 

   1. Navigate to the root directory of the extract GPSTk file tree, herein called $gpstk_root:

         $ cd $gpstk_root

   2. Navigate to the usersguide/ directory:

         $ cd ref/usersguide
             
   3. Compile the LaTex source to build PDF:

         $ make all

   4. Clean the usersguide directory of any files from compilation:

         $ make clean


Application Folder Documentation:
---------------------------------

Documentation for individual applications can also be found in the source folders for all /core 
applications.  Each /core/app folder contains a README markdown file meant for easy reference of 
application functions and options without the need to generate the LaTex User's Reference Manual PDF.  
The markdown has been designed for easy readability using a web browser or through a web service
such as Gitlab, which will auto render any README.md file found in a source folder.