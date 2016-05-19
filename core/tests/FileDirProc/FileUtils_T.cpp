//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//  
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

#include "TestUtil.hpp"
#include "FileUtils.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>

// headers for directory searching interface
#ifndef _WIN32
#include <unistd.h>
#else
#include <direct.h>
#include <io.h>
// Copied from linux libc sys/stat.h to keep code cross-platform:
#ifndef S_ISDIR
#define S_ISDIR(mode)  (((mode) & S_IFMT) == S_IFDIR)
#endif
#endif

using namespace std;
using namespace gpstk;

class FileUtils_T
{
public: 

      // constructor, set the precision value
   FileUtils_T() { init(); }

      // destructor
   ~FileUtils_T() { cleanup(); }

      // initialize tests
   void init();

      // test makeDir()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testMakeDir();

      // test fileAccessCheck()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testFileAccessCheck();

private:

      // recursively remove a file system object
      // @param path full path of the object to remove
   void cleanup(string path);

      // recursively remove all FileUtil-related objects in tempFilePath
   void cleanup();

   vector<string>  dirsToRemove;
   vector<string>  filesToRemove;

   string  tempFilePath;
   string  testPrefix;

}; // class FileUtils_T


//---------------------------------------------------------------------------
void FileUtils_T :: init()
{
    TestUtil  tester;

    tempFilePath = gpstk::getPathTestTemp();

    testPrefix = "test_output_fileutils_";
}


//---------------------------------------------------------------------------
void FileUtils_T :: cleanup()
{
      // remove files
   vector<string>::reverse_iterator  fileIter = filesToRemove.rbegin();
   for ( ; fileIter != filesToRemove.rend(); ++fileIter)
   {
         #ifdef WIN32
         _chmod(fileIter->c_str(), _S_IWRITE );
         _unlink(fileIter->c_str() );

         #else
         chmod(fileIter->c_str(), 0644);
         unlink(fileIter->c_str() );
         #endif

   }
      // remove directories
   vector<string>::reverse_iterator  dirIter = dirsToRemove.rbegin();
   for ( ; dirIter != dirsToRemove.rend(); ++dirIter)
   {
         #ifdef WIN32
         _chmod(dirIter->c_str(), _S_IWRITE );
         _rmdir(dirIter->c_str() );
         #else
         chmod(dirIter->c_str(), 0755);
         rmdir(dirIter->c_str() );
         #endif
   }
}


//---------------------------------------------------------------------------
int FileUtils_T :: testMakeDir()
{
   TestUtil  tester( "FileUtils", "makeDir", __FILE__, __LINE__ );

   // @note - These tests are kinda odd because makeDir always returns 0
   //         regardless of success or failure.

   string  dir;
   
   try   // empty path
   {
      tester.assert( (0 == FileUtils::makeDir(dir, 0755) ), "empty (return)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   // relative paths are not permitted according to FileUtils::makeDir documentation

   try   // absolute path
   {
      struct stat  statbuf;
      dir = tempFilePath + getFileSep() + testPrefix + "dir2";
      dirsToRemove.push_back(dir);
      tester.assert( (0 == FileUtils::makeDir(dir, 0755) ), "absolute (return)", __LINE__ );
      tester.assert( (0 == stat(dir.c_str(), &statbuf) ),   "absolute (exists)", __LINE__ );
      tester.assert( S_ISDIR(statbuf.st_mode),              "absolute (dir)",    __LINE__ );
      tester.assert( ( (statbuf.st_mode & 0777) == 0755),   "absolute (mode)",   __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   /*
   try   // invalid characters (I couldn't find any yet)
   {
      struct stat  statbuf;
      dir = tempFilePath + getFileSep() + testPrefix + "d|r<";
      dirsToRemove.push_back(dir);  // just in case this works somehow
      tester.assert( (0 == FileUtils::makeDir(dir, 0666) ), "invalid (return)", __LINE__ );
      tester.assert( (0 > stat(dir.c_str(), &statbuf) ),    "invalid (exists)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }
   */

   try   // existing path
   {
      struct stat  statbuf;
      dir = tempFilePath + getFileSep() + testPrefix + "dir2";
      dirsToRemove.push_back(dir);  // just in case
      tester.assert( (0 == FileUtils::makeDir(dir, 0755) ), "existing (return)", __LINE__ );
      tester.assert( (0 == stat(dir.c_str(), &statbuf) ),   "existing (exists)", __LINE__ );
      tester.assert( S_ISDIR(statbuf.st_mode),              "existing (dir)",    __LINE__ );
      tester.assert( ( (statbuf.st_mode & 0777) == 0755),   "existing (mode)",   __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // trailing separator
   {
      struct stat  statbuf;
      dir = tempFilePath + getFileSep() + testPrefix + "dir3" + getFileSep();
      dirsToRemove.push_back(dir);
      tester.assert( (0 == FileUtils::makeDir(dir, 0755) ), "trailing (return)", __LINE__ );
      tester.assert( (0 == stat(dir.c_str(), &statbuf) ),   "trailing (exists)", __LINE__ );
      tester.assert( S_ISDIR(statbuf.st_mode),              "trailing (dir)",    __LINE__ );
      tester.assert( ( (statbuf.st_mode & 0777) == 0755),   "trailing (mode)",   __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // recursion
   {
      struct stat  statbuf;
      dir = tempFilePath + getFileSep() + testPrefix + "dir4";
      dirsToRemove.push_back(dir);
      dir += getFileSep() + "derp";
      dirsToRemove.push_back(dir);
      dir += getFileSep() + "derp";
      dirsToRemove.push_back(dir);
      dir += getFileSep() + "derp";
      dirsToRemove.push_back(dir);
      dir += getFileSep() + "derp";
      dirsToRemove.push_back(dir);
      tester.assert( (0 == FileUtils::makeDir(dir, 0755) ), "recursion (return)", __LINE__ );
      tester.assert( (0 == stat(dir.c_str(), &statbuf) ),   "recursion (exists)", __LINE__ );
      tester.assert( S_ISDIR(statbuf.st_mode),              "recursion (dir)",    __LINE__ );
      tester.assert( ( (statbuf.st_mode & 0777) == 0755),   "recursion (mode)",   __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileUtils_T :: testFileAccessCheck()
{
   TestUtil  tester( "FileUtils", "fileAccessCheck", __FILE__, __LINE__ );

   string  filename;

   try   // missing file
   {
      filename = tempFilePath + getFileSep() + testPrefix + "missing_file";
      filesToRemove.push_back(filename);  // just in case
      tester.assert( !FileUtils::fileAccessCheck(filename),           "expected missing file failure", __LINE__ );
      tester.assert( !FileUtils::fileAccessCheck(filename, ios::in),  "expected missing file failure", __LINE__ );
         // Opening a file for output in a directory the user has
         // write access to will always succeed, so try to open a file
         // in a directory that almost certainly won't exist.
      filename = getFileSep() + "asdfasdflkj" + getFileSep() + "missing_dir";
      tester.assert( !FileUtils::fileAccessCheck(filename, ios::out), "expected missing file failure", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   
   try   // file read
   {
      filename = tempFilePath + getFileSep() + testPrefix + "readable_file";
      ofstream  ofs(filename.c_str() );
      if ( !ofs )
      {
         tester.assert ( false, "test setup error (create)", __LINE__ );
      }
      else
      {
         ofs.flush();
         ofs.close();
         #ifdef WIN32
         if (0 != _chmod(filename.c_str(), _S_IREAD ))
         {
            tester.assert ( false, "test setup error (chmod)", __LINE__ );
         }
         else
         {
            filesToRemove.push_back(filename);
            tester.assert( FileUtils::fileAccessCheck(filename),            "read access failed",         __LINE__ );
            tester.assert( FileUtils::fileAccessCheck(filename, ios::in),   "mode test failed",           __LINE__ );
            tester.assert( !FileUtils::fileAccessCheck(filename, ios::out), "expected mode test failure", __LINE__ );
         }
         #else
         if (0 != chmod(filename.c_str(), 0444) )
         {
            tester.assert ( false, "test setup error (chmod)", __LINE__ );
         }
         else
         {
            filesToRemove.push_back(filename);
            tester.assert( FileUtils::fileAccessCheck(filename),            "read access failed",         __LINE__ );
            tester.assert( FileUtils::fileAccessCheck(filename, ios::in),   "mode test failed",           __LINE__ );
            tester.assert( !FileUtils::fileAccessCheck(filename, ios::out), "expected mode test failure", __LINE__ );
         }
         #endif

      }
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // file write
   {
      filename = tempFilePath + getFileSep() + testPrefix + "writeable_file";
      ofstream  ofs(filename.c_str() );
      if ( !ofs )
      {
         tester.assert ( false, "test setup error (create)", __LINE__ );
      }
      else
      {
         ofs.flush();
         ofs.close();
         #ifdef WIN32
         if (0 != _chmod(filename.c_str(), _S_IWRITE ))

         {
            tester.assert ( false, "test setup error (chmod)", __LINE__ );
         }
         else            
         {
            filesToRemove.push_back(filename);
            tester.assert( FileUtils::fileAccessCheck(filename),           "write access failed", __LINE__ );
            tester.assert( FileUtils::fileAccessCheck(filename, ios::in),  "mode test failed",    __LINE__ );
            tester.assert( FileUtils::fileAccessCheck(filename, ios::out), "mode test failed",    __LINE__ );
         }
         #else
         if (0 != chmod(filename.c_str(), 0666) )
         {
            tester.assert ( false, "test setup error (chmod)", __LINE__ );
         }
         else            
         {
            filesToRemove.push_back(filename);
            tester.assert( FileUtils::fileAccessCheck(filename),           "write access failed", __LINE__ );
            tester.assert( FileUtils::fileAccessCheck(filename, ios::in),  "mode test failed",    __LINE__ );
            tester.assert( FileUtils::fileAccessCheck(filename, ios::out), "mode test failed",    __LINE__ );
         }
         #endif
      }
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   FileUtils_T  testClass;

   errorTotal += testClass.testMakeDir();
   errorTotal += testClass.testFileAccessCheck();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal;
}
