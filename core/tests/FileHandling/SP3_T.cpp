//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as
//  published by the Free Software Foundation; either version 3.0 of
//  the License, or any later version.
//
//  The GPSTk is distributed in the hope that it will be useful, but
//  WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//  02110, USA
//
//  Copyright 2015, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
// This software developed by Applied Research Laboratories at the
// University of Texas at Austin, under contract to an agency or
// agencies within the U.S.  Department of Defense. The
// U.S. Government retains all rights to use, duplicate, distribute,
// disclose, or release this software.
//
// Pursuant to DoD Directive 523024
//
// DISTRIBUTION STATEMENT A: This software has been approved for public
//                           release, distribution is unlimited.
//
//=============================================================================
#include "TestUtil.hpp"
#include "SP3Header.hpp"
#include "SP3Data.hpp"
#include "SP3Stream.hpp"
#include <iostream>

class SP3_T
{
public:

      // test the input and output of SP3 files
      //
      // @param ver SP3 version as enumerated in SP3Header::Version
      // @param inFile unique snippet of the test input file name,
      // i.e. FFF in ".../test_input_FFF.sp3"
      //
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
      //
   int doReadWriteTests(gpstk::SP3Header::Version ver, const std::string& inFile)
   {
      gpstk::TestUtil  tester( "SP3Data", "Read/Write (" + inFile + ")", __FILE__, __LINE__ );

      std::string  dataFilePath = gpstk::getPathData();
      std::string  dataFileName = dataFilePath + gpstk::getFileSep() +
         "test_input_" + inFile + ".sp3";
      gpstk::SP3Stream  inStream(dataFileName.c_str(), std::ios::in);

      tester.assert( inStream.good(), "error creating input stream", __LINE__ );

      inStream.exceptions(std::ios_base::failbit);

         // read in the header
      try
      {
         inStream >> inStream.header;
         tester.assert( true, "header read successfully", __LINE__ );
      }
      catch (gpstk::Exception& e)
      {
         std::ostringstream  oss;
         oss << "stream exception reading header: " << e;
         tester.assert( false, oss.str(), __LINE__ );
      }
      catch (...)
      {
         tester.assert( false, "unknown exception reading header", __LINE__ );
      }

         // read in all records
      std::vector<gpstk::SP3Data>  data;
      while (inStream.good() && (EOF != inStream.peek() ) )
      {
         gpstk::SP3Data  record;
         try
         {
            inStream >> record;
            data.push_back(record);
         }
         catch (gpstk::Exception& e)
         {
            std::ostringstream  oss;
            oss << "stream exception reading record: " << e;
            tester.assert( false, oss.str(), __LINE__ );
         }
         catch (...)
         {
            tester.assert( false, "unknown exception reading record", __LINE__ );
         }
      }
      inStream.close();

      std::string  tempFilePath = gpstk::getPathTestTemp();
      std::string  tempFileName = tempFilePath + gpstk::getFileSep() +
         "test_output_" + inFile + "_tmp.sp3";
      gpstk::SP3Stream  outStream(tempFileName.c_str(), std::ios::out);

      tester.assert( outStream.good(), "error creating ouput stream", __LINE__ );

      outStream.exceptions(std::ios_base::failbit | std::ios_base::badbit);

         // write out the header
      outStream.header = inStream.header;
      try
      {
         outStream << outStream.header;
         tester.assert( true, "header written successfully", __LINE__ );
      }
      catch (gpstk::Exception& e)
      {
         std::ostringstream  oss;
         oss << "stream exception writing header: " << e;
         tester.assert( false, oss.str(), __LINE__ );
      }
      catch (...)
      {
         tester.assert( false, "unknown exception writing header", __LINE__ );
      }

         // write all records
      std::vector<gpstk::SP3Data>::iterator  recordIter = data.begin();
      for ( ; recordIter != data.end(); ++recordIter)
      {
         try
         {
            outStream << *recordIter;
            tester.assert( true, "put record", __LINE__ );
         }
         catch (gpstk::Exception& e)
         {
            std::ostringstream  oss;
            oss << "exception writing record: " << e;
            tester.assert( false, oss.str(), __LINE__ );
         }
         catch (...)
         {
            tester.assert( false, "unknown exception writing record", __LINE__ );
         }
      }
      outStream.close();

         // test files for equality
      int diff = compareFiles(dataFileName, tempFileName);
      std::ostringstream  diffoss;
      diffoss << "files '" << dataFileName << "' and '" << tempFileName
              << "' should be equal but differ on"
              << " line " << (diff >> 8)
              << " column " << ((diff & 0xFF) + 1);
      tester.assert( (0 == diff), diffoss.str(), __LINE__ );

      return tester.countFails();
   }

      // compare two SP3 files byte by byte allowing for differing trailing
      // whitespace and differing fixed point notation ( 0.01 vs .01 ).
      // An exception is thrown if the reference file contains a line
      // of invalid length (>80 bytes).
      //
      // @param refFile "truth" file
      // @param checkFile file to compare to "truth" file
      //
      // @return 0 if equal, otherwise return the location in refFile where
      //    it first differs from checkFile.  The location is stored as an
      //    integer equal to (line number << 8) + column, where 0 <= column < 256.
      //    In other words the least significant byte of the return value contains
      //    a column number, and the remaining bytes contain a line number.
      //
   int compareFiles( const std::string& refFile,
                     const std::string& checkFile )
   {
      std::ifstream refStream;
      std::ifstream checkStream;
      std::string   refLine;
      std::string   checkLine;
      int           lineNumber = 0;

      refStream.open( refFile.c_str() );
      checkStream.open( checkFile.c_str() );

         // Compare each line until you reach the end of Ref
      while ( !refStream.eof() )
      {
         ++lineNumber;

            // If we reach the end of Check, but there is
            // more left in Ref, then they are not equal
         if ( checkStream.eof() )
         {
            return (lineNumber << 8);
         }

            // get the next line and compare
         getline( refStream, refLine );
         getline( checkStream, checkLine );

            // ignore trailing spaces
         std::size_t idx = refLine.find_last_not_of(" \t\r\n\f\v");
         if (idx != std::string::npos)
            refLine.erase(idx+1);
         else
            refLine.clear(); // all whitespace
         idx = checkLine.find_last_not_of(" \t\r\n\f\v");
         if (idx != std::string::npos)
            checkLine.erase(idx+1);
         else
            checkLine.clear(); // all whitespace

            // check remaining line length
         if (refLine.size() != checkLine.size())
         {
            return (lineNumber << 8);
         }

            // compare character by character
         int lastPos = refLine.size() - 1;
         if (lastPos > 80)
         {
            std::ostringstream  oss;
            oss << "Line " << lineNumber << " in the reference file exceeeds "
                << "the maximum valid SP3 line length (80)";
            gpstk::Exception exc(oss.str());
            GPSTK_THROW(exc);
         }
         for (int pos = 0; pos <= lastPos; ++pos)
         {
            if (refLine[pos] != checkLine[pos])
            {
               if (pos >= lastPos - 1)
               {
                  return (lineNumber << 8) + pos;
               }

                  // determine if the difference is due to fixed-point format
               switch (refLine[pos])
               {
                  case ' ':  // -.3 vs -0.3  OR  .3 vs 0.3
                     if (('-' == refLine[pos + 1]) && ('.' == refLine[pos + 2]))
                     {
                        if (('-' == checkLine[pos]) && ('0' == checkLine[pos + 1]))
                        {
                           ++pos;  // advance to the decimal
                           continue;  // OK
                        }
                     }
                     else if ('.' == refLine[pos + 1])
                     {
                        if ('0' == checkLine[pos])
                        {
                           continue;  // OK
                        }
                     }
                     break;

                  case '-':  // -0.3 vs -.3
                     if ((' ' == checkLine[pos]) && ('0' == refLine[pos + 1]) && ('.' == refLine[pos + 2]))
                     {
                        continue;  // OK
                     }
                     break;

                  case '0':  // 0.3 vs .3
                     if ((' ' == checkLine[pos]) && ('.' == refLine[pos + 1]))
                     {
                        continue;  // OK
                     }
                     break;

               }  // switch()

               return (lineNumber << 8) + pos;
            }
         }
      }

         // If we reach the end of Ref, but there is
         // more left in Check, then they are not equal
      return ( checkStream.eof() ? 0 : (lineNumber << 8) );
   }

}; // class SP3_T


int main()  //Main function to initialize and run all tests above
{
   int  errorTotal = 0;

   SP3_T  testClass;  // test data is loaded here

   try
   {
      errorTotal += testClass.doReadWriteTests(gpstk::SP3Header::SP3a, "SP3a");
      errorTotal += testClass.doReadWriteTests(gpstk::SP3Header::SP3a, "SP3ae");
      errorTotal += testClass.doReadWriteTests(gpstk::SP3Header::SP3b, "SP3b");
      errorTotal += testClass.doReadWriteTests(gpstk::SP3Header::SP3c, "SP3c");
   }
   catch (gpstk::Exception& e)
   {
      ++errorTotal;
      std::cerr << "unexpected exception executing tests: " << e;
   }
   catch (std::exception& e)
   {
      ++errorTotal;
      std::cerr << "unexpected exception executing tests: " << e.what();
   }
   catch (...)
   {
      ++errorTotal;
      std::cerr << "unexpected exception executing tests";
   }
   return( errorTotal );
}
