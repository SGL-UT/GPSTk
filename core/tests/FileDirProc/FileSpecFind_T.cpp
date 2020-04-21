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
 * probably fail. */


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
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;
   return errorTotal;
}
