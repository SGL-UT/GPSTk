DOCUMENTATION
-------

Contents:
---------

* Doxygen API Documentation - Automated Build
* Doxygen API Documentation - Manual Build
* LaTex User's Reference Manual


Doxygen API Documentation - Automated Build:
------------------------------------------------------------------------

   1. Navigate to the root directory of the extract GPSTk file tree, herein called $gpstk_root:

        $ cd $gpstk_root
         
   2a. Execute the build script to build the Core Doxygen API Documentation
        
        $ ./build.sh -d 
   
   2b. Execute the build script to build the Complete (Core/Ext) Doxygen API Documentation & Improved Python Docstrings
        
        $ ./build.sh -de


Doxygen API Documentation & Improved Python Docstrings - Manual Build:
----------------------------------------------------------------------

   1. Navigate to the root directory of the extract GPSTk file tree:

         $ cd $gpstk_root

         $ doxygen 


LaTex User's Reference Manual PDF:
------------------------------

To generate a PDF version of the GPSTk User's Guide, the source files must be compiled using a combination of perltex and latex. 

   1. Navigate to the root directory of the extract GPSTk file tree, herein called $gpstk_root:

         $ cd $gpstk_root

   2. Navigate to the usersguide/ directory:

         $ cd ref/usersguide
             
   3. Compile the LaTex source to build PDF:

         $ make all

   4. Clean the usersguide directory of any files from compilation:

         $ make clean
