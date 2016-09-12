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

#include "FileHunter.hpp"
#include "YDSTime.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>

// headers for directory searching interface
#ifndef _WIN32
#include <unistd.h>
#else
#include <direct.h>
#include <io.h>
#define PATH_MAX _MAX_PATH
#endif

using namespace std;
using namespace gpstk;

class FileHunter_T
{
public: 

      // constructor
   FileHunter_T() { init(); }

      // destructor
   ~FileHunter_T() { cleanup(); }

      // initialize tests, throws on failure
   void init();

      // remove file system objects created during tests
   void cleanup();

      // test creation of an initialization of FileHunter objects
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testInitialization();

      // test the newHunt() method
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testNewHunt();

      // test the setFilter() method
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testSetFilter();

      // test the find() method
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testFind();

private:

      // @param path Full path of the directory to create
      // @note an exception will be thrown on failure
   void newDir(const string& path);

      // @param path Full path of the directory to create
      // @note an exception will be thrown on failure
   void newFile(const string& path);

      // @return true if the string is in the vector, false otherwise
   bool contains(const vector<string>& vec, const string& str);

      // emit a vector of strings to standard output
      // @param strs vector of strings to emit 
   void dump(const vector<string>& strs);

   string  tempFilePath;  // includes trailing slash

   vector<string>  dirsToRemove;
   vector<string>  filesToRemove;

}; // class FileHunter_T


//---------------------------------------------------------------------------
void FileHunter_T :: init()
{
   TestUtil  tester;

   tempFilePath = gpstk::getPathTestTemp() + getFileSep() + "test_output_filehunter";

      // create directories and files for the find() tests
   newDir(tempFilePath);

   tempFilePath += getFileSep();  // ensure trailling slash

   newFile(tempFilePath + "sample.data");
   newFile(tempFilePath + "prn_08.data");
   newFile(tempFilePath + "prn_16.data");
   newFile(tempFilePath + "2001_123.data");
   newFile(tempFilePath + "2001_234.data");
   newFile(tempFilePath + "2002_123.data");
   newFile(tempFilePath + "2002_234.data");
   newFile(tempFilePath + "2001_123_08.data");
   newFile(tempFilePath + "2001_234_08.data");
   newFile(tempFilePath + "2002_123_16.data");
   newFile(tempFilePath + "2002_234_16.data");
   newDir(tempFilePath + "2003");
   newDir(tempFilePath + "2004");
   newFile(tempFilePath + "2003" + getFileSep() + "123_08.data");
   newFile(tempFilePath + "2003" + getFileSep() + "123_16.data");
   newFile(tempFilePath + "2003" + getFileSep() + "234_08.data");
   newFile(tempFilePath + "2003" + getFileSep() + "234_16.data");
   newFile(tempFilePath + "2004" + getFileSep() + "123_08.data");
   newFile(tempFilePath + "2004" + getFileSep() + "123_16.data");
   newFile(tempFilePath + "2004" + getFileSep() + "234_08.data");
   newFile(tempFilePath + "2004" + getFileSep() + "234_16.data");
   newFile(tempFilePath + "2003" + getFileSep() + "2003_123.data");
   newFile(tempFilePath + "2003" + getFileSep() + "2003_234.data");
   newFile(tempFilePath + "2004" + getFileSep() + "2004_123.data");
   newFile(tempFilePath + "2004" + getFileSep() + "2004_234.data");
}


//---------------------------------------------------------------------------
void FileHunter_T :: newDir(const string& path)
{
   #ifdef WIN32
   if (_mkdir(path.c_str()) != 0)
   {
      if (errno != EEXIST)
      {
         string  exc("failed to create test directory: " + path);
         throw(exc);
      }
   }
   #else
   if (mkdir(path.c_str(), 0755) != 0)
   {
      if (errno != EEXIST)
      {
         string  exc("failed to create test directory: " + path);
         throw(exc);
      }
   }
   #endif


   dirsToRemove.push_back(path);
}


//---------------------------------------------------------------------------
void FileHunter_T :: newFile(const string& path)
{
   ofstream  ofs(path.c_str(), ios::out);
   if (!ofs)
   {
      string  exc("failed to create test file: " + path);
      throw(exc);
   }
   else
   {
      filesToRemove.push_back(path);
   }
}


//---------------------------------------------------------------------------
bool FileHunter_T :: contains(const vector<string>& vec, const string& str)
{
   vector<string>::const_iterator  i = vec.begin();
   for ( ; i != vec.end(); ++i)
   {
      if (0 == str.compare(*i) )
      {
         return true;
      }
   }
   return false;
}


//---------------------------------------------------------------------------
void FileHunter_T :: dump(const vector<string>& strs)
{
   cout << "  FILE DUMP:" << endl;
   vector<string>::const_iterator  strIter = strs.begin();
   for ( ; strIter != strs.end(); ++ strIter)
      cout << "    " << *strIter << endl;
}



//---------------------------------------------------------------------------
void FileHunter_T :: cleanup()
{
      // remove files
   vector<string>::reverse_iterator  fileIter = filesToRemove.rbegin();
   for ( ; fileIter != filesToRemove.rend(); ++fileIter)
   {
      #ifdef WIN32
      _unlink(fileIter->c_str() );
      #else
      unlink(fileIter->c_str() );
      #endif
   }
      // remove directories
   vector<string>::reverse_iterator  dirIter = dirsToRemove.rbegin();
   for ( ; dirIter != dirsToRemove.rend(); ++dirIter)
   {
      #ifdef WIN32
      _rmdir(dirIter->c_str() );
      #else
      rmdir(dirIter->c_str() );
      #endif
   }
}


//---------------------------------------------------------------------------
int FileHunter_T :: testInitialization()
{
   TestUtil  tester( "FileHunter", "initialization", __FILE__, __LINE__ );

   try   // empty file spec
   {
      FileHunter  hunter("");
      tester.assert( false, "expected exception for empty file spec", __LINE__ );
   }
   catch (FileHunterException& fse)
   {
      tester.assert( true, "expected exception for empty file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "expected FileHunterException exception", __LINE__ );
   }

   try   // fixed file spec
   {
      FileHunter  hunter("fixed_spec");
      tester.assert( true, "fixed file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // no path file spec
   {
      FileHunter  hunter("no_path_%p");
      tester.assert( true, "no path file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // relative path file spec
   {
      FileHunter  hunter("dir" + getFileSep() + "spec_%p");
      tester.assert( true, "relative path file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // absolute path file spec
   {
      FileHunter  hunter(tempFilePath + "dir"
                        + getFileSep() + "spec_%p");
      tester.assert( true, "absolute path file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multiple file spec type file spec
   {
      FileHunter  hunter(tempFilePath + "dir"
                        + getFileSep() + "spec_%p_%04Y_%03j_%05s");
      tester.assert( true, "multiple file spec type file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multi-directory file spec
   {
      FileHunter  hunter(tempFilePath + "dir_%n"
                        + getFileSep() + "spec_%p");
      tester.assert( true, "multi-directory file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileHunter_T :: testNewHunt()
{
   TestUtil  tester( "FileHunter", "newHunt", __FILE__, __LINE__ );

   try   // empty file spec
   {
      FileHunter  hunter("fixed_spec");
      hunter.newHunt("");
      tester.assert( false, "expected exception for empty file spec", __LINE__ );
   }
   catch (FileHunterException& fse)
   {
      tester.assert( true, "expected exception for empty file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "expected FileHunterException exception", __LINE__ );
   }

   try   // fixed file spec
   {
      FileHunter  hunter("fixed_spec");
      hunter.newHunt("new_fixed_spec");
      tester.assert( true, "fixed file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // no path file spec
   {
      FileHunter  hunter("fixed_spec");
      hunter.newHunt("no_path_%p");
      tester.assert( true, "no path file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // relative path file spec
   {
      FileHunter  hunter("fixed_spec");
      hunter.newHunt("dir" + getFileSep() + "spec_%p");
      tester.assert( true, "relative path file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // absolute path file spec
   {
      FileHunter  hunter("fixed_spec");
      hunter.newHunt(tempFilePath + "2001"
                    + getFileSep() + "spec_%p");
      tester.assert( true, "absolute path file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multiple file spec type file spec
   {
      FileHunter  hunter("fixed_spec");
      hunter.newHunt(tempFilePath + "2001"
                    + getFileSep() + "spec_%p_%04Y_%03j_%05s");
      tester.assert( true, "multiple file spec type file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multi-directory file spec
   {
      FileHunter  hunter("fixed_spec");
      hunter.newHunt(tempFilePath + "2001_%n"
                    + getFileSep() + "spec_%p");
      tester.assert( true, "multi-directory file spec", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileHunter_T :: testSetFilter()
{
   TestUtil  tester( "FileHunter", "setFilter", __FILE__, __LINE__ );

   try   // unknown file spec type
   {
      FileHunter  hunter("fixed_spec");
      std::vector<string>  toFilter;
      toFilter.push_back("foo");
      hunter.setFilter(FileSpec::unknown, toFilter);
      tester.assert( false, "expected exception for unknown file spec type", __LINE__ );
   }
   catch (FileHunterException& fhe)
   {
      tester.assert( true, "expected exception for unknown file spec type", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "expected FileHunterException exception", __LINE__ );
   }

   try   // missing file spec type
   {
      FileHunter  hunter("fixed_spec");
      std::vector<string>  toFilter;
      toFilter.push_back("24");
      hunter.setFilter(FileSpec::prn, toFilter);
      tester.assert( false, "expected exception for missing file spec type", __LINE__ );
   }
   catch (FileHunterException& fhe)
   {
      tester.assert( true, "expected exception for missing file spec type", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "expected FileHunterException exception", __LINE__ );
   }

   try   // empty filter list
   {
      FileHunter  hunter("fixed_spec_%p");
      std::vector<string>  toFilter;
      hunter.setFilter(FileSpec::prn, toFilter);
      tester.assert( false, "expected exception for empty filter list", __LINE__ );
   }
   catch (FileHunterException& fhe)
   {
      tester.assert( true, "expected exception for empty filter list", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "expected FileHunterException exception", __LINE__ );
   }

   try   // single-entry filter list
   {
      FileHunter  hunter("fixed_spec_%p");
      std::vector<string>  toFilter;
      toFilter.push_back("24");
      hunter.setFilter(FileSpec::prn, toFilter);
      tester.assert( true, "single-entry filter list", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multiple-entry filter list
   {
      FileHunter  hunter("fixed_spec_%p");
      std::vector<string>  toFilter;
      toFilter.push_back("12");
      toFilter.push_back("18");
      toFilter.push_back("24");
      hunter.setFilter(FileSpec::prn, toFilter);
      tester.assert( true, "multiple-entry filter list", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multiple setFilter invocations, same file spec type
   {
      FileHunter  hunter("fixed_spec_%p");
      std::vector<string>  toFilter1;
      toFilter1.push_back("12");
      toFilter1.push_back("18");
      hunter.setFilter(FileSpec::prn, toFilter1);
      tester.assert( true, "multiple setFilter invocations, same file spec type", __LINE__ );
      std::vector<string>  toFilter2;
      toFilter2.push_back("16");
      toFilter2.push_back("24");
      hunter.setFilter(FileSpec::prn, toFilter2);
      tester.assert( true, "multiple setFilter invocations, same file spec type", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multiple setFilter invocations, different file spec type
   {
      FileHunter  hunter("fixed_spec_%p_%n");
      std::vector<string>  toFilter1;
      toFilter1.push_back("12");
      toFilter1.push_back("18");
      hunter.setFilter(FileSpec::prn, toFilter1);
      tester.assert( true, "multiple setFilter invocations, different file spec type", __LINE__ );
      std::vector<string>  toFilter2;
      toFilter2.push_back("85401");
      toFilter2.push_back("85408");
      hunter.setFilter(FileSpec::station, toFilter2);
      tester.assert( true, "multiple setFilter invocations, different file spec type", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileHunter_T :: testFind()
{
   TestUtil  tester( "FileHunter", "find", __FILE__, __LINE__ );

   try   // fixed file spec (present)
   {
      string  filename(tempFilePath + "sample.data");
      FileHunter  hunter(filename);
      vector<string>  files = hunter.find();
      tester.assert( ( (files.size() == 1) && (0 == filename.compare(files[0]) ) ),
                     "fixed file spec (present)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // fixed file spec (absent)
   {
      string  filename(tempFilePath + "missing.data");
      FileHunter  hunter(filename);
      vector<string>  files = hunter.find();
      tester.assert( (files.size() == 0),
                     "fixed file spec (absent)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // single file spec type (present)
   {
      string  filename(tempFilePath + "prn_%02p.data");
      FileHunter  hunter(filename);
      vector<string>  files = hunter.find();
      tester.assert( (  (files.size() == 2)
                     && (contains(files, tempFilePath + "prn_08.data") > 0)
                     && (contains(files, tempFilePath + "prn_16.data") > 0) ),
                     "single file spec type (present)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // single file spec type (absent)
   {
      string  filename(tempFilePath + "prn_%02p.missing");
      FileHunter  hunter(filename);
      vector<string>  files = hunter.find();
      tester.assert( (files.size() == 0),
                     "single file spec type (absent)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   //  multiple file spec types (present)
   {
      string  filename(tempFilePath + "%04Y_%03j.data");
      FileHunter  hunter(filename);
      vector<string>  files = hunter.find();
      tester.assert( (  (files.size() == 4)
                     && (contains(files, tempFilePath + "2001_123.data") > 0)
                     && (contains(files, tempFilePath + "2001_234.data") > 0)
                     && (contains(files, tempFilePath + "2002_123.data") > 0)
                     && (contains(files, tempFilePath + "2002_234.data") > 0) ),
                     "multiple file spec types (present)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   //  multiple file spec types (present)
   {
      string  filename(tempFilePath + "%04Y_%03j_%02p.data");
      FileHunter  hunter(filename);
      vector<string>  files = hunter.find();
      tester.assert( (  (files.size() == 4)
                     && (contains(files, tempFilePath + "2001_123_08.data") > 0)
                     && (contains(files, tempFilePath + "2001_234_08.data") > 0)
                     && (contains(files, tempFilePath + "2002_123_16.data") > 0)
                     && (contains(files, tempFilePath + "2002_234_16.data") > 0) ),
                     "multiple file spec types (present)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multiple file spec types (absent)
   {
      string  filename(tempFilePath + "%02p_%04Y_%03j.data");
      FileHunter  hunter(filename);
      vector<string>  files = hunter.find();
      tester.assert( (files.size() == 0),
                     "multiple file spec types (absent)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   //  multi-directory file spec (present)
   {
      string  filename(tempFilePath + "%04Y"
                      + getFileSep() + "%03j_%02p.data");
      FileHunter  hunter(filename);
      vector<string>  files = hunter.find();
      tester.assert( (  (files.size() == 8)
                     && (contains(files, tempFilePath + "2003" + getFileSep() + "123_08.data") > 0)
                     && (contains(files, tempFilePath + "2003" + getFileSep() + "123_16.data") > 0)
                     && (contains(files, tempFilePath + "2003" + getFileSep() + "234_08.data") > 0)
                     && (contains(files, tempFilePath + "2003" + getFileSep() + "234_16.data") > 0)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "123_08.data") > 0)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "234_16.data") > 0)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "123_08.data") > 0)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "234_16.data") > 0) ),
                     "multi-directory file spec (present)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multi-directory file spec (absent)
   {
      string  filename(tempFilePath + "%04Y"
                      + getFileSep() + "%02p.%03j.data");
      FileHunter  hunter(filename);
      vector<string>  files = hunter.find();
      tester.assert( (files.size() == 0),
                     "multi-directory file spec (absent)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // single-value filtering (file)
   {
      string  filename(tempFilePath + "%04Y"
                      + getFileSep() + "%03j_%02p.data");
      FileHunter  hunter(filename);
      std::vector<string>  toFilter;
      toFilter.push_back("16");
      hunter.setFilter(FileSpec::prn, toFilter);
      vector<string>  files = hunter.find();
      tester.assert( (  (files.size() == 4)
                     && (contains(files, tempFilePath + "2003" + getFileSep() + "123_16.data") > 0)
                     && (contains(files, tempFilePath + "2003" + getFileSep() + "234_16.data") > 0)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "123_16.data") > 0)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "234_16.data") > 0) ),
                     "single-value filtering (file)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // single-value filtering (dir)
   {
      string  filename(tempFilePath + "%04Y"
                      + getFileSep() + "%03j_%02p.data");
      FileHunter  hunter(filename);
      std::vector<string>  toFilter;
      toFilter.push_back("2004");
      hunter.setFilter(FileSpec::year, toFilter);
      vector<string>  files = hunter.find();
      tester.assert( (  (files.size() == 4)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "123_08.data") > 0)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "123_16.data") > 0)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "234_08.data") > 0)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "234_16.data") > 0) ),
                     "single-value filtering (dir)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // single-value filtering (missing)
   {
      string  filename(tempFilePath + "%04Y"
                      + getFileSep() + "%03j_%02p.data");
      FileHunter  hunter(filename);
      std::vector<string>  toFilter;
      toFilter.push_back("2005");
      hunter.setFilter(FileSpec::year, toFilter);
      vector<string>  files = hunter.find();
      tester.assert( (files.size() == 0),
                     "single-value filtering (missing)", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multiple-value filtering
   {
      string  filename(tempFilePath + "%04Y"
                      + getFileSep() + "%03j_%02p.data");
      FileHunter  hunter(filename);
      std::vector<string>  yearFilter;
      yearFilter.push_back("2004");
      hunter.setFilter(FileSpec::year, yearFilter);
      std::vector<string>  prnFilter;
      prnFilter.push_back("16");
      hunter.setFilter(FileSpec::prn, prnFilter);
      vector<string>  files = hunter.find();
      tester.assert( (  (files.size() == 2)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "123_16.data") > 0)
                     && (contains(files, tempFilePath + "2004" + getFileSep() + "234_16.data") > 0) ),
                     "multiple-value filtering", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // time filtering
   {
      string  filename(tempFilePath + "%04Y"
                      + getFileSep() + "%03j_%02p.data");
      FileHunter  hunter(filename);
      vector<string>  files;
      CommonTime  minTime; 
      CommonTime  maxTime; 

      minTime = YDSTime(2001, 1, 0, TimeSystem::Any); 
      maxTime = YDSTime(2002, 1, 0, TimeSystem::Any); 
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 0),
                     "time filtering (all before)", __LINE__ );
      if (files.size() != 0) dump(files);  // @debug

      minTime = YDSTime(2006, 1, 0, TimeSystem::Any); 
      maxTime = YDSTime(2007, 1, 0, TimeSystem::Any); 
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 0),
                     "time filtering (all above)", __LINE__ );
      if (files.size() != 0) dump(files);  // @debug

      minTime = YDSTime(2001, 1, 0, TimeSystem::Any);
      maxTime = YDSTime(2007, 1, 0, TimeSystem::Any);
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 8),
                     "time filtering (all included)", __LINE__ );
      if (files.size() != 8) dump(files);  // @debug

      minTime = YDSTime(2001,   1, 0, TimeSystem::Any);
      maxTime = YDSTime(2004, 150, 0, TimeSystem::Any);
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 6),
                     "time filtering (lower 3/4)", __LINE__ );
      if (files.size() != 6) dump(files);  // @debug

      minTime = YDSTime(2003, 150, 0, TimeSystem::Any);
      maxTime = YDSTime(2007,   1, 0, TimeSystem::Any);
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 6),
                     "time filtering (upper 3/4)", __LINE__ );
      if (files.size() != 6) dump(files);  // @debug

      minTime = YDSTime(2003, 150, 0, TimeSystem::Any);
      maxTime = YDSTime(2004, 150, 0, TimeSystem::Any);
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 4),
                     "time filtering (middle)", __LINE__ );
      if (files.size() != 4) dump(files);  // @debug

   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // multi-dir time filtering
   {
      string  filename(tempFilePath + "%04Y"
                      + getFileSep() + "%03j_%02p.data");
      FileHunter  hunter(filename);
      vector<string>  files;
      CommonTime  minTime; 
      CommonTime  maxTime; 

      minTime = YDSTime(2001, 1, 0, TimeSystem::Any); 
      maxTime = YDSTime(2002, 1, 0, TimeSystem::Any); 
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 0),
                     "multi-dir time filtering (all before)", __LINE__ );
      if (files.size() != 0) dump(files);  // @debug

      minTime = YDSTime(2006, 1, 0, TimeSystem::Any); 
      maxTime = YDSTime(2007, 1, 0, TimeSystem::Any); 
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 0),
                     "multi-dir time filtering (all above)", __LINE__ );
      if (files.size() != 0) dump(files);  // @debug

      minTime = YDSTime(2001, 1, 0, TimeSystem::Any);
      maxTime = YDSTime(2007, 1, 0, TimeSystem::Any);
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 8),
                     "multi-dir time filtering (all included)", __LINE__ );
      if (files.size() != 8) dump(files);  // @debug

      minTime = YDSTime(2001,   1, 0, TimeSystem::Any);
      maxTime = YDSTime(2004, 150, 0, TimeSystem::Any);
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 6),
                     "multi-dir time filtering (lower 3/4)", __LINE__ );
      if (files.size() != 6) dump(files);  // @debug

      minTime = YDSTime(2003, 150, 0, TimeSystem::Any);
      maxTime = YDSTime(2007,   1, 0, TimeSystem::Any);
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 6),
                     "multi-dir time filtering (upper 3/4)", __LINE__ );
      if (files.size() != 6) dump(files);  // @debug

      minTime = YDSTime(2003, 150, 0, TimeSystem::Any);
      maxTime = YDSTime(2004, 150, 0, TimeSystem::Any);
      files = hunter.find(minTime, maxTime);
      tester.assert( (files.size() == 4),
                     "multi-dir time filtering (middle)", __LINE__ );
      if (files.size() != 4) dump(files);  // @debug

   }
   catch (...)
   {
      tester.assert( false, "unexpected exception", __LINE__ );
   }

   return tester.countFails();
}

 
 /** Initialize and run all tests.
  *
  * @return Total error count for all tests
  */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   FileHunter_T  testClass;

   errorTotal += testClass.testInitialization();
   errorTotal += testClass.testNewHunt();
   errorTotal += testClass.testSetFilter();
   errorTotal += testClass.testFind();
   
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal;
}
