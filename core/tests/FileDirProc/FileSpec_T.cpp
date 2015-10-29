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

#include "FileSpec.hpp"
#include "YDSTime.hpp"
#include "GPSWeekZcount.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <set>

using namespace std;
using namespace gpstk;

class FileSpec_T
{
public: 

      // constructor
   FileSpec_T() { init(); }

      // destructor
   ~FileSpec_T() {}

      // initialize tests
   void init();

      // test conversion of file spec type values to strings and vice-versa.
      // Also test that no duplicate file spec types exist.
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testConvertFileSpecType();

      // test operator++() and operator--() for FileSpecType
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testFileSpecTypeOps();

      // test creation of invalid FileSpec objects
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testInitInvalid();

      // test creation of valid FileSpec objects and test a few methods
      // that get the state of FileSpec objects
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testInitValid();

      // test FileSpec method newSpec()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testNewSpec();

      // test FileSpec method hasField()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testHasField();

      // test FileSpec method getSpecString()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testGetSpecString();

      // test FileSpec method createSearchString()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testCreateSearchString();

      // test FileSpec method extractField()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testExtractField();

      // test FileSpec method extractCommonTime()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testExtractCommonTime();

      // test FileSpec method toString()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testToString();

      // test FileSpec method sortList()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testSortList();

}; // class FileSpec_T


//---------------------------------------------------------------------------
void FileSpec_T :: init()
{
      // empty
}


//---------------------------------------------------------------------------
int FileSpec_T :: testConvertFileSpecType()
{
   TestUtil  tester( "FileSpec", "convertFileSpecType", __FILE__, __LINE__ );

   set<std::string>  fstSet;
   FileSpec::FileSpecType  fst = FileSpec::unknown;
   fst++;  // skip unknown
   for ( ; fst < FileSpec::end; fst++)
   {
      string  fstStr;

         // attempt to convert from a FileSpecType to a string
      try
      {
         fstStr = FileSpec::convertFileSpecType(fst);

         ostringstream  oss;
         oss << "duplicate file spec type string: " << fstStr;
         tester.assert( (fstSet.find(fstStr) == fstSet.end() ), oss.str(), __LINE__ );

            // check for empty file spec type string
         if (fstStr.size() == 0)
         {
            if (fst == FileSpec::fixed)
            {
                  // Special Case : 'fixed' should convert to "" to denote
                  // no future value substitution, don't try to convert
                  // from "" to fixed though
               tester.assert( true, "fixed FileSpecType", __LINE__ );
            }
            else
            {
               ostringstream  oss;
               oss << "empty file spec type string for value: " << fst;
               tester.assert( false, oss.str(), __LINE__ );
            }
            continue;  // don't test with an empty file spec type string
         }

            // check that file spec type string is exactly one character
         {
            ostringstream  oss;
            oss << "file spec type string should be a single character: " << fstStr;
            tester.assert( fstStr.size() == 1, oss.str(), __LINE__ );
         }

            // store the file spec type string so it can be checked for duplication
         switch (fst)
         {
               // Special Case : 'y' and 'Y' should both denote year
            case FileSpec::year:
               if ((fstStr[0] == 'y') || (fstStr[0] == 'Y'))
               {
                  fstSet.insert("y");
                  fstSet.insert("Y");
               }
               else
               {
                  ostringstream  oss;
                  oss << "special case failed for file spec type: y / Y";
                  tester.assert( false, oss.str(), __LINE__ );
               }
               break;

                  // Special Case : 'c' and 'C' should both denote full GPS zcount
            case FileSpec::fullzcount:
               if ((fstStr[0] == 'c') || (fstStr[0] == 'C'))
               {
                  fstSet.insert("c");
                  fstSet.insert("C");
               }
               else
               {
                  ostringstream  oss;
                  oss << "special case failed for file spec type: c / C";
                  tester.assert( false, oss.str(), __LINE__ );
               }
               break;

            default:
               fstSet.insert(fstStr);
         }
      }
      catch (FileSpecException& fse)
      {
         ostringstream  oss;
         oss << "unexpected exception for file spec type value: " << fst;
         tester.assert( false, oss.str(), __LINE__ );
      }

         // attempt to convert from a string to a FileSpecType
      try
      {
         FileSpec::FileSpecType  fstPost = FileSpec::convertFileSpecType(fstStr);

         ostringstream  oss;
         oss << "file spec type value/string/value mismatch: "
             << fst << "/" + fstStr + "/" << fstPost;

         tester.assert( (fst == fstPost), oss.str(), __LINE__ );
      }
      catch (FileSpecException& fse)
      {
         tester.assert( false, "unexpected exception for file spec type string: " + fstStr, __LINE__ );
      }
   }

      // Special Case : check that 'y' and 'Y' both denote year
   try
   {
      FileSpec::FileSpecType  fsty = FileSpec::convertFileSpecType("y");
      FileSpec::FileSpecType  fstY = FileSpec::convertFileSpecType("Y");
      tester.assert( (fsty == fstY), "'y' and 'Y' should be equivalent", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      tester.assert( false, "unexpected exception for file spec type: y / Y", __LINE__ );
   }

      // Special Case : check that 'c' and 'C' both denote full GPS zcount
   try
   {
      FileSpec::FileSpecType  fstc = FileSpec::convertFileSpecType("c");
      FileSpec::FileSpecType  fstC = FileSpec::convertFileSpecType("C");
      tester.assert( (fstc == fstC), "'c' and 'C' should be equivalent", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      tester.assert( false, "unexpected exception for file spec type: c / C", __LINE__ );
   }

      // check for file spec type value underflow
   try
   {
      string  fstStr = FileSpec::convertFileSpecType(FileSpec::unknown);
      tester.assert(false, "exception expected for file spec type: unknown", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      tester.assert( true, "expected exception for invalid FileSpecType", __LINE__ );
   }

      // check for file spec type value overflow
   try
   {
      string  fstStr = FileSpec::convertFileSpecType(FileSpec::end);
      tester.assert( false, "exception expected for file spec type: end", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      tester.assert( true, "expected exception for invalid FileSpecType", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testFileSpecTypeOps()
{
   TestUtil  tester( "FileSpecType", "operators", __FILE__, __LINE__ );

      /// @todo implement testFileSpecTypeOps

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testInitInvalid()
{
   TestUtil  tester( "FileSpec", "init (invalid)", __FILE__, __LINE__ );

   const string  PCT("%");

      // assemble a list of invalid file spec strings
   vector<string>  invalidSpecs;

   invalidSpecs.push_back("%%");

   string  s;
   for (char c = 'A'; c <= 'Z'; ++c)
   {
      s = c;
      try
      {
         FileSpec::FileSpecType  fst = FileSpec::convertFileSpecType(s);

            // this specified is valid, ignore it
      }
      catch (FileSpecException& fse)
      {
            // this is not a valid specifier, add it
         invalidSpecs.push_back(PCT + s);
      }
   }
   for (char c = 'a'; c <= 'z'; ++c)
   {
      s = c;
      try
      {
         FileSpec::FileSpecType  fst = FileSpec::convertFileSpecType(s);

            // this specified is valid, ignore it
      }
      catch (FileSpecException& fse)
      {
            // this is not a valid specifier, add it
         invalidSpecs.push_back(PCT + s);
      }
   }

      // create objects from invalid file spec strings (and hopefully fail)
   vector<string>::const_iterator  specIter = invalidSpecs.begin();
   for ( ; specIter != invalidSpecs.end(); ++specIter)
   {
      try
      {
         FileSpec  fs(*specIter);
         ostringstream  oss;
         oss << "missing expected exception creating FileSpec(\""
             << *specIter << "\")";
         tester.assert( false, oss.str(), __LINE__ );
      }
      catch (FileSpecException& fse)
      {
         tester.assert( true, "expected exception for invalid FileSpec", __LINE__ );
      }
   }   

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testInitValid()
{
   TestUtil  tester( "FileSpec", "init (valid)", __FILE__, __LINE__ );

   try   // create a default object
   {
      FileSpec  fs;
      tester.assert( true, "FileSpec created", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "exception creating default, empty FileSpec: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }


   vector<string>  validSpecs;

   const string  PCT("%");

   validSpecs.push_back("");
   validSpecs.push_back(" ");
      ///@todo implement %% in FileSpec
      //validSpecs.push_back(PCT + PCT);

   FileSpec::FileSpecType  fst = FileSpec::unknown;
   fst++;  // skip unknown
   for ( ; fst < FileSpec::end; fst++)
   {
      string  s = FileSpec::convertFileSpecType(fst);
      if (s.length() > 0)
      {
         validSpecs.push_back(s);
         validSpecs.push_back(PCT + s);
         validSpecs.push_back(PCT + s + PCT + s);
         validSpecs.push_back(" " + PCT + s + " " + PCT + s + " ");
         validSpecs.push_back(PCT + "4" + s);
         validSpecs.push_back(PCT + "04" + s);
         validSpecs.push_back(PCT + "-8" + s);
         validSpecs.push_back(PCT + "16" + s);
         validSpecs.push_back(PCT + "-12" + s);
            ///@todo implement precision support in FileSpec
            //validSpecs.push_back(PCT + "10.4" + s);
         validSpecs.push_back(PCT + "4" + s + PCT + "12" + s);
         validSpecs.push_back(PCT + "-8" + s + PCT + "06" + s);

            // @note - FileSpec does not support precision, for example "%2.4g"

            // @note - FileSpec does not support '+', #', or ' ' as flags,
            //         for example "%+2g", "%#2g", and "% 2g"
      }
   }

      // test all of the specs - they should result in a valid FileSpec
   vector<string>::const_iterator  specIter = validSpecs.begin();
   for ( ; specIter != validSpecs.end(); ++specIter)
   {
      try
      {
         FileSpec  fs(*specIter);
         tester.assert( true, "FileSpec created", __LINE__ );
      }
      catch (FileSpecException& fse)
      {
         ostringstream  oss;
         oss << "exception creating FileSpec(\""
             << *specIter << "\"): " << fse;
         tester.assert( false, oss.str(), __LINE__ );
      }
   }   
   
   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testNewSpec()
{
   TestUtil  tester( "FileSpec", "newSpec", __FILE__, __LINE__ );

   try
   {
      FileSpec  spec;
      tester.assert( (spec.getSpecString().size() == 0),
                     "empty spec string mismatch", __LINE__ );

      string  str1("test-%y-spec");
      spec.newSpec(str1);
      tester.assert( (str1.compare(spec.getSpecString()) == 0),
                     "non-empty spec string mismatch", __LINE__ );

      string  str2("another-%y-one");
      spec.newSpec(str2);
      tester.assert( (str2.compare(spec.getSpecString()) == 0),
                     "non-empty spec string mismatch", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }
   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testHasField()
{
   TestUtil  tester( "FileSpec", "hasField", __FILE__, __LINE__ );

   try   // empty spec
   {
      FileSpec  spec;
      bool  found = spec.hasField(FileSpec::year);
      tester.assert( !found, "spec field not found", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // non-empty spec containing field
   {
      string  str("test-%y-spec");
      FileSpec  spec(str);
      bool  found = spec.hasField(FileSpec::year);
      tester.assert( found, "spec field not found", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // non-empty spec missing field
   {
      string  str("test-%p-spec");
      FileSpec  spec(str);
      bool  found = spec.hasField(FileSpec::year);
      tester.assert( !found, "missing spec field mistakenly found", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }
   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testGetSpecString()
{
   TestUtil  tester( "FileSpec", "getSpecString", __FILE__, __LINE__ );

   try
   {
      FileSpec  spec;
      tester.assert( (spec.getSpecString().size() == 0),
                     "empty spec string mismatch", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try
   {
      string  str("test-%y-spec");
      FileSpec  spec(str);
      tester.assert( (str.compare(spec.getSpecString()) == 0),
                     "non-empty spec string mismatch", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }
   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testCreateSearchString()
{
   TestUtil  tester( "FileSpec", "createSearchString", __FILE__, __LINE__ );

   try   // empty spec
   {
      FileSpec  spec;
      tester.assert( (spec.createSearchString().size() == 0),
                     "empty spec string mismatch", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // fixed spec
   {
      string  str("test-spec");
      FileSpec  spec(str);
      tester.assert( (str.compare(spec.createSearchString()) == 0),
                     "fixed spec string mismatch", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // non-fixed spec, single substitution
   {
      string  str("test-%y-spec");
      string  expect("test-?-spec");
      FileSpec  spec(str);
      tester.assert( (expect.compare(spec.createSearchString()) == 0),
                     "non-fixed spec single substitution string mismatch", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // non-fixed spec, multiple substitution
   {
      string  str("test-%y-spec-%y.%y");
      string  expect("test-?-spec-?.?");
      FileSpec  spec(str);
      tester.assert( (expect.compare(spec.createSearchString()) == 0),
                     "non-fixed spec multiple substituion string mismatch", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }
   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testExtractField()
{
   TestUtil  tester( "FileSpec", "extractField", __FILE__, __LINE__ );

   try   // extract a field that is present (single)
   {
      FileSpec  spec("test-%4y-spec");
         
      string  field = spec.extractField("test-1999-spec", FileSpec::year);
      tester.assert( (field.compare("1999") == 0), "extract field failed", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }
      
   try   // extract a field that is present multiple times
   {
      FileSpec  spec("test-%2y-spec-%2y");
         
      string  field = spec.extractField("test-97-spec-96", FileSpec::year);
      tester.assert( (field.compare("97") == 0), "extract field failed", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }
      
   try   // extract multiple different fields
   {
      FileSpec  spec("test-%4y%03j%05s-spec");
         
      string  yField = spec.extractField("test-200412312345", FileSpec::year);
      tester.assert( (yField.compare("2004") == 0), "extract field failed", __LINE__ );

      string  jField = spec.extractField("test-200412312345", FileSpec::day);
      tester.assert( (jField.compare("123") == 0), "extract field failed", __LINE__ );

      string  sField = spec.extractField("test-200412312345", FileSpec::doysecond);
      tester.assert( (sField.compare("12345") == 0), "extract field failed", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }
      
   try   // extract a field that isn't there
   {
      FileSpec  spec("test-%y-spec");

      string field = spec.extractField("test-1999-spec", FileSpec::station);
      ostringstream  oss;
      oss << "missing expected exception";
      tester.assert( false, oss.str(), __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "received expected exception: " << fse;
      tester.assert( true, oss.str(), __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testExtractCommonTime()
{
   TestUtil  tester( "FileSpec", "extractCommonTime", __FILE__, __LINE__ );

   try   // extract a valid time
   {
      FileSpec  spec("test-%4Y%03j%05s-spec");
         
      CommonTime t = spec.extractCommonTime("test-200412312345-spec");
      YDSTime  ydst(2004, 123, 12345.0);
      tester.assert( (ydst == t), "extract time failed", __LINE__ );

   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // extract an invalid time
   {
      FileSpec  spec("test-%4Y%03j%05s-spec");
         
      CommonTime t = spec.extractCommonTime("test-101043299999-spec");
      tester.assert( false, "expected exception for invalid time", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "expected exception for invalid time: " << fse;
      tester.assert( true, oss.str(), __LINE__ );
   }

/* This test would have worked with the old DayTime implementation,
 * however CommonTime performs no such checks.

   try   // extract an incomplete time
   {
      FileSpec  spec("test-%4Y-%05s-spec");
         
      CommonTime t = spec.extractCommonTime("test-1999-12345-spec");
      tester.assert( false, "expected exception for incomplete time", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "expected exception for incomplete time: " << fse;
      tester.assert( true, oss.str(), __LINE__ );
   }
*/

   try   // extract a missing time
   {
      FileSpec  spec("test-%4Y%03j%05s-spec");
         
      CommonTime t = spec.extractCommonTime("test-spec");
      tester.assert( false, "expected exception for missing time", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "expected exception for missing time: " << fse;
      tester.assert( true, oss.str(), __LINE__ );
   }

/* Really? Was it ever requried that a FileSpec have a time?
   try   // extract from a time-less spec
   {
      FileSpec  spec("test-%2p-%2r-spec");
         
      CommonTime t = spec.extractCommonTime("test-24-01-spec");
      tester.assert( false, "expected exception for missing time in spec", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "expected exception for missing time in spec: " << fse;
      tester.assert( true, oss.str(), __LINE__ );
   }
*/

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testToString()
{
   TestUtil  tester( "FileSpec", "toString", __FILE__, __LINE__ );

   try // default GPSWeekZcount
   {
      FileSpec  spec("test-%04F%06Z-spec");
         
      GPSWeekZcount wz;
      CommonTime t(wz);
      string  str = spec.toString(t);
      tester.assert( (str.compare("test-0000000000-spec") == 0),
                     "toString failed: " + str, __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try // non-default GPSWeekZcount
   {
      FileSpec  spec("test-%04F%06Z-spec");
         
      GPSWeekZcount wz(1234,56789);
      CommonTime t(wz);
      string  str = spec.toString(t);
      tester.assert( (str.compare("test-1234056789-spec") == 0),
                     "toString failed: " + str, __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try // non-default GPSWeekZcount plus missing other stuff
   {
      FileSpec  spec("test-%04F%06Z-%p-%n-%k-spec");
         
      GPSWeekZcount wz(1234,56789);
      CommonTime t(wz);
      string  str = spec.toString(t);
      tester.assert( (str.compare("test-1234056789-%1p-%1n-%1k-spec") == 0),
                     "toString failed: " + str, __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try // non-default GPSWeekZcount plus supplied other stuff
   {
      FileSpec  spec("test-%04F%06Z-%02p-%05n-%02r-%02k-spec");
         
      GPSWeekZcount wz(1234,56789);
      CommonTime t(wz);
      FileSpec::FSTStringMap  stuff;
      stuff[FileSpec::prn] = "12";
      stuff[FileSpec::station] = "96344";
      stuff[FileSpec::receiver] = "1";
      stuff[FileSpec::clock] = "1";
      string  str = spec.toString(t, stuff);
      tester.assert( (str.compare("test-1234056789-12-96344-01-01-spec") == 0),
                     "toString failed: " + str, __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

      ///@todo implement precision support in FileSpec
/*
   try   // default CommonTime
   {
      FileSpec  spec("test-%04Y%03j%05.0s-spec");
         
      CommonTime t;
      string  str = spec.toString(t);
      tester.assert( (str.compare("test-000000000000-spec") == 0),
                     "toString failed: " + str, __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // non-default CommonTime
   {
      FileSpec  spec("test-%04Y%03j%05.0s-spec");
         
      YDSTime  ydst(1991, 234, 23456);
      string  str = spec.toString(ydst);
      tester.assert( (str.compare("test-199123423456-spec") == 0),
                     "toString failed: " + str, __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // non-default CommonTime plus missing other stuff
   {
      FileSpec  spec("test-%04Y%03j%05.0s-%p-%n-%k-spec");
         
      YDSTime  ydst(1991, 234, 23456);
      string  str = spec.toString(ydst);
      tester.assert( (str.compare("test-199123423456-spec") == 0),
                     "toString failed: " + str, __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // non-default CommonTime plus supplied other stuff
   {
      FileSpec  spec("test-%04Y%03j%05.0s-%02p-%05n-%02r-%02k-spec");
         
      YDSTime  ydst(1991, 234, 23456);
      FileSpec::FSTStringMap  stuff;
      stuff[FileSpec::prn] = "12";
      stuff[FileSpec::station] = "96344";
      stuff[FileSpec::receiver] = "1";
      stuff[FileSpec::clock] = "1";
      string  str = spec.toString(ydst, stuff);
      tester.assert( (str.compare("test-199123423456-12-96344-01-01-spec") == 0),
                     "toString failed: " + str, __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }
*/

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileSpec_T :: testSortList()
{
   TestUtil  tester( "FileSpec", "sortList", __FILE__, __LINE__ );

   try   // sort an empty list
   {
      FileSpec  spec("test-%04Y%03j%05s-%p-%n-%r-%k-spec");
      vector<string>  fileList;
      spec.sortList(fileList);
      tester.assert( (fileList.size() == 0),
                     "failed empty list sort", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // sort a list with one element
   {
      FileSpec  spec("test-%04Y%03j%05s-%p-%n-%r-%k-spec");
      vector<string>  fileList;
      fileList.push_back("test-1997020030000-23-96344-1-1-spec");
      spec.sortList(fileList);
      tester.assert(  (  (fileList.size() == 1) 
                         && (fileList[0].compare("test-1997020030000-23-96344-1-1-spec") == 0) ),
                      "failed single element list sort", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // sort a list with several elements differentiated only by time
   {
         // This only ever worked because the prn, station, receiver
         // and clock were all the same.  You really need to specify
         // the length of the fields.
         /**@todo should we make it a requirement that the length is
          * specified, and throw an exception if it isn't? */
         //FileSpec  spec("test-%04Y%03j%05s-%p-%n-%r-%k-spec");
      FileSpec  spec("test-%04Y%03j%05s-%02p-%05n-%1r-%1k-spec");

      vector<string>  sortedFileList;
         //                          YYYYDDDSSSSS PP NNNNN R K
      sortedFileList.push_back("test-199702001000-23-96344-1-1-spec");
      sortedFileList.push_back("test-199702003000-23-96344-1-1-spec");
      sortedFileList.push_back("test-199703003000-23-96344-1-1-spec");
      sortedFileList.push_back("test-199802003000-23-96344-1-1-spec");
      sortedFileList.push_back("test-199803003000-23-96344-1-1-spec");
      sortedFileList.push_back("test-199902003000-23-96344-1-1-spec");
      vector<string>  fileList;
      fileList.push_back(sortedFileList[4]);
      fileList.push_back(sortedFileList[0]);
      fileList.push_back(sortedFileList[2]);
      fileList.push_back(sortedFileList[5]);
      fileList.push_back(sortedFileList[1]);
      fileList.push_back(sortedFileList[3]);
      spec.sortList(fileList);
      tester.assert( equal(fileList.begin(), fileList.end(), sortedFileList.begin() ),
                     "failed multiple element list sort", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   try   // sort a list with several elements differentiated by non-time elements
   {
      FileSpec  spec("test-%04Y%03j%05s-%02p-%05n-%02r-%02k-spec");

      vector<string>  sortedFileList;
         // index
         //                     v    v   v1 v    vv 2v    vv v3 v 
         //                     01234567890123456789012345678901234567
         //                   ("test-199702001000-13-96344-01-01-spec");
         // field sort order and name
         //                     4    6   7  8     3  1     2  5 4
         //                     fixd YYYYDDDSSSSS PP NNNNN RR KKfixed
      sortedFileList.push_back("test-199702001000-13-96344-01-01-spec");
      sortedFileList.push_back("test-199702001000-23-96344-01-01-spec");
      sortedFileList.push_back("test-199702001000-13-96344-02-02-spec");
      sortedFileList.push_back("test-199702001000-23-96344-02-01-spec");
      sortedFileList.push_back("test-199702001000-13-96346-01-01-spec");
      sortedFileList.push_back("test-199702001000-13-96346-01-02-spec");
      sortedFileList.push_back("test-199702001000-13-96347-01-01-spec");
      sortedFileList.push_back("test-199702001000-23-96347-01-01-spec");
      sortedFileList.push_back("test-199702001000-13-96347-02-01-spec");
      vector<string>  fileList;
      fileList.push_back(sortedFileList[8]);
      fileList.push_back(sortedFileList[4]);
      fileList.push_back(sortedFileList[1]);
      fileList.push_back(sortedFileList[6]);
      fileList.push_back(sortedFileList[0]);
      fileList.push_back(sortedFileList[2]);
      fileList.push_back(sortedFileList[5]);
      fileList.push_back(sortedFileList[7]);
      fileList.push_back(sortedFileList[3]);
      spec.sortList(fileList);
      tester.assert( equal(fileList.begin(), fileList.end(),
                           sortedFileList.begin() ),
                     "failed multiple element list sort", __LINE__ );
   }
   catch (FileSpecException& fse)
   {
      ostringstream  oss;
      oss << "unexpected exception: " << fse;
      tester.assert( false, oss.str(), __LINE__ );
   }

   return tester.countFails();
}


/** Run the program.
 *
 * @return Total error count for all tests
 */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   FileSpec_T  testClass;

   errorTotal += testClass.testConvertFileSpecType();
   errorTotal += testClass.testFileSpecTypeOps();
   errorTotal += testClass.testInitInvalid();
   errorTotal += testClass.testInitValid();
   errorTotal += testClass.testNewSpec();
   errorTotal += testClass.testHasField();
   errorTotal += testClass.testGetSpecString();
   errorTotal += testClass.testCreateSearchString();
   errorTotal += testClass.testExtractField();
   errorTotal += testClass.testExtractCommonTime();
   errorTotal += testClass.testToString();
   errorTotal += testClass.testSortList();

   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal;
}
