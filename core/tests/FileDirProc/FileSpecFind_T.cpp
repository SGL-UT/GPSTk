//==============================================================================
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
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin.
//  Copyright 2004-2020, The Board of Regents of The University of Texas System
//
//==============================================================================

//==============================================================================
//
//  This software was developed by Applied Research Laboratories at the
//  University of Texas at Austin, under contract to an agency or agencies
//  within the U.S. Department of Defense. The U.S. Government retains all
//  rights to use, duplicate, distribute, disclose, or release this software.
//
//  Pursuant to DoD Directive 523024
//
//  DISTRIBUTION STATEMENT A: This software has been approved for public
//                            release, distribution is unlimited.
//
//==============================================================================

#ifndef WIN32
#include <unistd.h>
#endif
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <regex>
#include "TestUtil.hpp"
#include "YDSTime.hpp"
#include "FileSpecFind.hpp"

/** @note This test is expected to run with the data directory being
 * current working directory.  If it is run anywhere else, it will
 * fail. */


using namespace std;

class FileSpecFind_T
{
public:
   FileSpecFind_T()
         : fs(gpstk::getFileSep())
   {}

      /// test find with absolute paths
   unsigned findTestsAbs();
      /// test find with relative paths, no . or ..
   unsigned findTestsRel();
      /// test find with relative paths that include .
   unsigned findTestsRelDot();
      /// test find with relative paths that include ..
   unsigned findTestsRelDotDot();
      /// test find with a simple file name with no wildcards and no path
   unsigned findSimpleFileName();

private:
      /// generic version of above tests
   unsigned findTests(const std::string& tld, const std::string& testName);
      /// Return true if all paths in files can be opened for read.
   bool openable(const list<string>& files);

      /// File separator, but short.
   std::string fs;
};


bool FileSpecFind_T ::
openable(const list<string>& files)
{
   for (const auto& i : files)
   {
      ifstream f(i.c_str());
      if (!f)
      {
         cerr << "Can't open \"" << i << "\"" << endl;
         return false;
      }
   }
   return true;
}


unsigned FileSpecFind_T ::
findTestsAbs()
{
   string tld = gpstk::getPathData() + fs + "FileSpecFind";
   return findTests(tld, "Abs");
}


unsigned FileSpecFind_T ::
findTestsRel()
{
   string tld = "FileSpecFind";
   return findTests(tld, "Rel");
}


unsigned FileSpecFind_T ::
findTestsRelDot()
{
   string tld = "." + fs + "FileSpecFind";
   return findTests(tld, "RelDot");
}


unsigned FileSpecFind_T ::
findTestsRelDotDot()
{
   string tld = ".." + fs + "data" + fs + "FileSpecFind";
   return findTests(tld, "RelDotDot");
}


unsigned FileSpecFind_T ::
findTests(const std::string& tld, const std::string& testName)
{
   TUDEF("FileSpecFind", "find("+testName+")");

   string searchSpec = tld + fs + "%04Y" + fs + "%05n" + fs + "%03j" + fs +
      "nsh-%x-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml";

   string winTest = gpstk::FileSpecFind::transToken(searchSpec);
   gpstk::FileSpec::FSTStringMap fsts;
   fsts[gpstk::FileSpec::text] = "FOO";
   list<string> files;
   using ListSize = list<string>::size_type;

      // Expected counts in test below are based on the current
      // contents of the directories as of the time of this writing.
      // The directories should be messed with, but if they are it
      // could break this test.

   TUCSM("find(" + testName + ") no results");
   try
   {
      files = gpstk::FileSpecFind::find(
         searchSpec,
         gpstk::YDSTime(2016,211,0),
         gpstk::YDSTime(2016,212,0),
         fsts);
   }
   catch (gpstk::Exception &exc)
   {
      cerr << exc;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception " + std::string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   TUASSERTE(ListSize, 0, files.size());

   TUCSM("find(" + testName + ") single epoch");
   try
   {
      files = gpstk::FileSpecFind::find(
         searchSpec,
         gpstk::YDSTime(2018,211,67500),
         gpstk::YDSTime(2018,211,67500),
         fsts);
   }
   catch (gpstk::Exception &exc)
   {
      cerr << exc;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception " + std::string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   TUASSERTE(ListSize, 20, files.size());
      // make sure we got all valid files
   TUASSERT(openable(files));

   TUCSM("find(" + testName + ") single epoch via span");
   try
   {
      files = gpstk::FileSpecFind::find(
         searchSpec,
         gpstk::YDSTime(2018,211,67500),
         gpstk::YDSTime(2018,211,67800),
         fsts);
   }
   catch (gpstk::Exception &exc)
   {
      cerr << exc;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception " + std::string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   TUASSERTE(ListSize, 20, files.size());
      // make sure we got all valid files
   TUASSERT(openable(files));

   TUCSM("find(" + testName + ") multiple epochs");
   try
   {
      files = gpstk::FileSpecFind::find(
         searchSpec,
         gpstk::YDSTime(2018,211,67500),
         gpstk::YDSTime(2018,211,67801),
         fsts);
   }
   catch (gpstk::Exception &exc)
   {
      cerr << exc;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception " + std::string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   TUASSERTE(ListSize, 40, files.size());
      // make sure we got all valid files
   TUASSERT(openable(files));

      // change the search spec for filter to not have variable length tokens.
   searchSpec = tld + fs + "%04Y" + fs + "%05n" + fs + "%03j" + fs +
      "nsh-%3x-%5n-%1r-%04Y-%03j-%02H%02M%02S.xml";
   TUCSM("find(" + testName + ") multiple epochs with filter");
   try
   {
      gpstk::FileSpecFind::Filter filter;
      filter.insert(gpstk::FileSpecFind::Filter::value_type(
                       gpstk::FileSpec::station, "10000"));
      files = gpstk::FileSpecFind::find(
         searchSpec,
         gpstk::YDSTime(2018,211,67500),
         gpstk::YDSTime(2018,211,67801),
         filter);
   }
   catch (gpstk::Exception &exc)
   {
      cerr << exc;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception " + std::string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   TUASSERTE(ListSize, 5, files.size());
      // make sure we got all valid files
   TUASSERT(openable(files));

   try
   {
      gpstk::FileSpecFind::Filter filter;
      filter.insert(gpstk::FileSpecFind::Filter::value_type(
                       gpstk::FileSpec::station, "10000"));
      filter.insert(gpstk::FileSpecFind::Filter::value_type(
                       gpstk::FileSpec::text, "FOO"));
      files = gpstk::FileSpecFind::find(
         searchSpec,
         gpstk::YDSTime(2018,211,67500),
         gpstk::YDSTime(2018,211,67801),
         filter);
   }
   catch (gpstk::Exception &exc)
   {
      cerr << exc;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception " + std::string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   TUASSERTE(ListSize, 4, files.size());
      // make sure we got all valid files
   TUASSERT(openable(files));

   try
   {
      gpstk::FileSpecFind::Filter filter;
      filter.insert(gpstk::FileSpecFind::Filter::value_type(
                       gpstk::FileSpec::station, "10000"));
      filter.insert(gpstk::FileSpecFind::Filter::value_type(
                       gpstk::FileSpec::text, "BAR"));
      files = gpstk::FileSpecFind::find(
         searchSpec,
         gpstk::YDSTime(2018,211,67500),
         gpstk::YDSTime(2018,211,67801),
         filter);
   }
   catch (gpstk::Exception &exc)
   {
      cerr << exc;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception " + std::string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   TUASSERTE(ListSize, 1, files.size());
      // make sure we got all valid files
   TUASSERT(openable(files));

   try
   {
      gpstk::FileSpecFind::Filter filter;
      filter.insert(gpstk::FileSpecFind::Filter::value_type(
                       gpstk::FileSpec::station, "10000"));
      filter.insert(gpstk::FileSpecFind::Filter::value_type(
                       gpstk::FileSpec::receiver, "2"));
      files = gpstk::FileSpecFind::find(
         searchSpec,
         gpstk::YDSTime(2018,211,67500),
         gpstk::YDSTime(2018,211,67801),
         filter);
   }
   catch (gpstk::Exception &exc)
   {
      cerr << exc;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception " + std::string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   TUASSERTE(ListSize, 2, files.size());
      // make sure we got all valid files
   TUASSERT(openable(files));

   searchSpec = tld + fs + "2018" + fs + "10000" + fs + "211" + fs +
      "nsh-FOO-10000-1-2018-211-184500.xml";
   TUCSM("find(" + testName + ") exact file name");
   try
   {
      gpstk::FileSpecFind::Filter filter;
      files = gpstk::FileSpecFind::find(
         searchSpec,
         gpstk::CommonTime::BEGINNING_OF_TIME,
         gpstk::CommonTime::END_OF_TIME,
         filter);
   }
   catch (gpstk::Exception &exc)
   {
      cerr << exc;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception " + std::string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   TUASSERTE(ListSize, 1, files.size());
      // make sure we got all valid files
   TUASSERT(openable(files));

   TURETURN();
}


unsigned FileSpecFind_T ::
findSimpleFileName()
{
   TUDEF("FileSpecFind", "find(simple file name)");
   list<string> files;
   using ListSize = list<string>::size_type;
   try
   {
         // The spec doesn't really matter too much as long as it
         // refers to a file that exists in the data directory.  This
         // also assumes the test is run in the data directory, but
         // the other tests do as well.
      files = gpstk::FileSpecFind::find(
         "TropModel_Zero.exp",
         gpstk::YDSTime(2016,211,0),
         gpstk::YDSTime(2016,212,0));
   }
   catch (gpstk::Exception &exc)
   {
      cerr << exc;
      TUFAIL("Unexpected exception");
   }
   catch (std::exception& exc)
   {
      TUFAIL("Unexpected exception " + std::string(exc.what()));
   }
   catch (...)
   {
      TUFAIL("Unexpected exception");
   }
   TUASSERTE(ListSize, 1, files.size());
      // make sure we got all valid files
   TUASSERT(openable(files));

   TURETURN();
}


int main(int argc, char *argv[])
{
   unsigned errorTotal = 0;
   FileSpecFind_T testClass;
   errorTotal += testClass.findTestsAbs();
   errorTotal += testClass.findTestsRel();
   errorTotal += testClass.findTestsRelDot();
   errorTotal += testClass.findTestsRelDotDot();
   errorTotal += testClass.findSimpleFileName();
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
   return errorTotal;
}
