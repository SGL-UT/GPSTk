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
//   release, distribution is unlimited.
//
//=============================================================================

#include <iostream>
#include <string>
#include <sstream>
#include <iterator>
#include "StringUtils.hpp"
#include "TestUtil.hpp"

using namespace gpstk::StringUtils;
using namespace std;

namespace std
{
   std::ostream& operator<<(std::ostream& s, const std::vector<std::string>& v)
   {
      s << "[ ";
      std::copy(v.begin(), v.end(),
                std::ostream_iterator<std::string>(s, ", "));
      s << " ]";
      return s;
   }
}


      /// Array of hex values to output for hexToAsciiTest
const char hexDumpArray1[] =
{ 0x00, 0x10, 0x30, 0x33, 0x30, 0x31, 0x30, 0x35,
  0x3A, 0x65, 0x70, 0x68, 0x20, 0x3A, 0x30, 0x3A,
  0x35, 0x32, 0x36, 0x34, 0x30, 0x2E, 0x33, 0x33,
  0x33, 0x34, 0x30, 0x32, 0x37, 0x37, 0x37, 0x37,
  0x37, 0x37, 0x37, 0x38, 0x41, 0x20, 0x52, 0x61,
  0x6e, 0x64, 0x6F, 0x6D, 0x20, 0x4D, 0x65, 0x73,
  0x73, 0x61, 0x67, 0x65, 0x32, 0x35, 0x39, 0x32,
  0x35, 0x39, 0x32, 0x34, 0x20, 0x32, 0x20, 0x35,
  0x32, 0x36, 0x34, 0x30, 0x2E, 0x33, 0x33, 0x38,
  0x38, 0x33, 0x36, 0x33, 0x34, 0x32, 0x35, 0x39,
  0x32, 0x35, 0x39, 0x33, 0x20, 0x7F, 0x20, 0x38,
  0x35, 0x34, 0x31, 0x32, 0x20, 0x31, 0x20, 0x31,
  0x20, 0x30, 0x20, 0x31, 0x20, 0x31, 0x20, 0x34,
  0x20, 0x32, 0x20, 0x34, 0x20, 0x30, 0x20, 0x30,
  0x20, 0x33, 0x20, 0x31, 0x31, 0x39, 0x39, 0x20,
  0x32, 0x32, 0x63, 0x30, 0x61, 0x66, 0x30, 0x33,
  0x20, 0x31, 0x37, 0x37, 0x30, 0x32, 0x39, 0x65,
  0x34, 0x20, 0x61, 0x66, 0x34, 0x30, 0x30, 0x31,
  0x66, 0x20, 0x65, 0x36, 0x31, 0x62, 0x35, 0x38,
  0x61, 0x20, 0x35, 0x35, 0x66, 0x33, 0x37, 0x35,
  0x30, 0x20, 0x34, 0x63, 0x63, 0x61, 0x38, 0x38,
  0x37, 0x20, 0x31, 0x31, 0x35, 0x64, 0x37, 0x63,
  0x66, 0x63, 0x20, 0x33, 0x61, 0x64, 0x32, 0x30,
  0x34, 0x38, 0x61, 0x20, 0x63, 0x65, 0x61, 0x20,
  0x33, 0x36, 0x32, 0x63, 0x34, 0x38, 0x20, 0x31,
  0x31, 0x39, 0x39, 0x20, 0x32, 0x32, 0x63, 0x30,
  0x61, 0x66, 0x30, 0x33, 0x20, 0x31, 0x37, 0x37,
  0x30, 0x34, 0x61, 0x37, 0x34, 0x20, 0x33, 0x61,
  0x63, 0x34, 0x36, 0x36, 0x63, 0x31, 0x20, 0x62,
  0x64, 0x30, 0x63, 0x36, 0x64, 0x63, 0x20, 0x33,
  0x39, 0x31, 0x62, 0x33, 0x37, 0x38, 0x33, 0x20,
  0x33, 0x65, 0x30, 0x34, 0x30 };


      /** Array of hex values to output for hexDumpDataTest,
       * hexDumpDataStreamFlagTest. */
const char hexDumpArray2[] =
{ 0x61, 0x66, 0x30, 0x33, 0x30, 0x31, 0x30, 0x35,
  0x3A, 0x65, 0x70, 0x68, 0x20, 0x3A, 0x30, 0x3A,
  0x35, 0x32, 0x36, 0x34, 0x30, 0x2E, 0x33, 0x33,
  0x33, 0x34, 0x30, 0x32, 0x37, 0x37, 0x37, 0x37,
  0x37, 0x37, 0x37, 0x38, 0x41, 0x20, 0x52, 0x61,
  0x6e, 0x64, 0x6F, 0x6D, 0x20, 0x4D, 0x65, 0x73,
  0x73, 0x61, 0x67, 0x65, 0x32, 0x35, 0x39, 0x32,
  0x35, 0x39, 0x32, 0x34, 0x20, 0x32, 0x20, 0x35,
  0x32, 0x36, 0x34, 0x30, 0x2E, 0x33, 0x33, 0x38,
  0x38, 0x33, 0x36, 0x33, 0x34, 0x32, 0x35, 0x39,
  0x32, 0x35, 0x39, 0x33, 0x20, 0x32, 0x20, 0x38,
  0x35, 0x34, 0x31, 0x32, 0x20, 0x31, 0x20, 0x31,
  0x20, 0x30, 0x20, 0x31, 0x20, 0x31, 0x20, 0x34,
  0x20, 0x32, 0x20, 0x34, 0x20, 0x30, 0x20, 0x30,
  0x20, 0x33, 0x20, 0x31, 0x31, 0x39, 0x39, 0x20,
  0x32, 0x32, 0x63, 0x30, 0x61, 0x66, 0x30, 0x33,
  0x20, 0x31, 0x37, 0x37, 0x30, 0x32, 0x39, 0x65,
  0x34, 0x20, 0x61, 0x66, 0x34, 0x30, 0x30, 0x31,
  0x66, 0x20, 0x65, 0x36, 0x31, 0x62, 0x35, 0x38,
  0x61, 0x20, 0x35, 0x35, 0x66, 0x33, 0x37, 0x35,
  0x30, 0x20, 0x34, 0x63, 0x63, 0x61, 0x38, 0x38,
  0x37, 0x20, 0x31, 0x31, 0x35, 0x64, 0x37, 0x63,
  0x66, 0x63, 0x20, 0x33, 0x61, 0x64, 0x32, 0x30,
  0x34, 0x38, 0x61, 0x20, 0x63, 0x65, 0x61, 0x20,
  0x33, 0x36, 0x32, 0x63, 0x34, 0x38, 0x20, 0x31,
  0x31, 0x39, 0x39, 0x20, 0x32, 0x32, 0x63, 0x30,
  0x61, 0x66, 0x30, 0x33, 0x20, 0x31, 0x37, 0x37,
  0x30, 0x34, 0x61, 0x37, 0x34, 0x20, 0x33, 0x61,
  0x63, 0x34, 0x36, 0x36, 0x63, 0x31, 0x20, 0x62,
  0x64, 0x30, 0x63, 0x36, 0x64, 0x63, 0x20, 0x33,
  0x39, 0x31, 0x62, 0x33, 0x37, 0x38, 0x33, 0x20,
  0x33, 0x65, 0x30, 0x34, 0x30 };


/**
 * This test file will contain a series of scanf checks for each of
 * the directly tested TimeTag classes.
 */
class StringUtils_T
{
public:
   StringUtils_T() {}
   ~StringUtils_T() {}

      /**
       * Get the contents of a file and store it in a string.  Used to
       * get expected text results.
       */
   string getFileContents(const string& fileName)
   {
      static const int BUF_SIZ = 512;
      ifstream s(fileName.c_str());
      std::string rv;
      GPSTK_ASSERT(s);
      char *buffer = new char[BUF_SIZ];
      while (s)
      {
         s.read(buffer, BUF_SIZ);
         rv += string(buffer, s.gcount());
      }
      delete[] buffer;
      return rv;
   }

      /**
       * Tests for the justification methods.
       * Given a baseString and a filler character, the tests will justify the
       * string to a new string of the size of the original string + 6.
       */
   unsigned justificationTest()
   {
      TUDEF("StringUtils", "Justify");
      string baseString= "1234567890"; // base string to manipulate
      string resultString;           // storage for the correct response
      string manipulatedString;      // storage for the manipulated base string
      char fillerChar = '-';

      manipulatedString = baseString;
      resultString = string(6, fillerChar) + baseString;
         // Add six filler characters to the front of the string
      rightJustify(manipulatedString, manipulatedString.length() + 6,
                   fillerChar);
      TUCSM("rightJustify");
      TUASSERTE(std::string, resultString, manipulatedString);

      manipulatedString = baseString;
      resultString = baseString + string(6, fillerChar);
         // Add six filler characters to the end of the string
      leftJustify(manipulatedString, manipulatedString.length()+6, fillerChar);
      TUCSM("leftJustify");
      TUASSERTE(std::string, resultString, manipulatedString);
			
      manipulatedString = baseString;
      resultString = string(3, fillerChar) + baseString +
         string(3, fillerChar);
         // Add three filler characters to both ends of the string
      center(manipulatedString, manipulatedString.length()+6, fillerChar);
      TUCSM("center");
      TUASSERTE(std::string, resultString, manipulatedString);

      TURETURN();
   }



      /**
       * Tests for the stripLeading methods.
       * Given a baseString and a filler character and string, the tests
       * will strip the baseString of the request string or character from
       * the front of the string.
       */
   unsigned stripLeadingTest()
   {
      TUDEF("StringUtils", "stripLeading");
      string baseString= "---1234567890---";  // base string to manipulate
      string resultString;           // storage for the correct response
      string manipulatedString;      // storage for the manipulated base string
      char charToBeRemoved = '-';
      string stringToBeRemoved = "--";

      manipulatedString = baseString;
      resultString = baseString.substr(1,string::npos);
         // Strip one of the character -'s from the beginning of the string
      manipulatedString = stripLeading(manipulatedString, charToBeRemoved, 1);
      TUASSERTE(std::string, resultString, manipulatedString);

      manipulatedString = baseString;
      resultString = baseString.substr(2,string::npos);
         // Strip one of the string --'s from the beginning of the string
      manipulatedString = stripLeading(manipulatedString, stringToBeRemoved,
                                       1);
      TUASSERTE(std::string, resultString, manipulatedString);

      manipulatedString = baseString;
      resultString = baseString.substr(3,string::npos);
         // Strip all of the character -'s from the beginning of the string
      manipulatedString = stripLeading(manipulatedString, charToBeRemoved);
      TUASSERTE(std::string, resultString, manipulatedString);

      manipulatedString = baseString;
      resultString = baseString.substr(2,string::npos);
         // Strip all of the string --'s from the beginning of the string
      manipulatedString = stripLeading(manipulatedString, stringToBeRemoved);
      TUASSERTE(std::string, resultString, manipulatedString);
			
      TURETURN();
   }


      /**
       * Tests for the stripTrailing methods.
       * Given a baseString and a filler character and string, the tests
       * will strip the baseString of the request string or character from
       * the end of the string.
       */
   unsigned stripTrailingTest()
   {
      TUDEF("StringUtils", "stripTrailing");
      string baseString= "---1234567890---"; // base string to manipulate
      string resultString;           // storage for the correct response
      string manipulatedString;      // storage for the manipulated base string
      char charToBeRemoved = '-';
      string stringToBeRemoved = "--";

      manipulatedString = baseString;
      resultString = baseString.substr(0, baseString.length()-1);
         // Strip one of the character -'s from the end of the string
      manipulatedString = stripTrailing(manipulatedString, charToBeRemoved, 1);
      TUASSERTE(std::string, resultString, manipulatedString);

      manipulatedString = baseString;
      resultString = baseString.substr(0, baseString.length()-2);
         // Strip one of the string --'s from the end of the string
      manipulatedString = stripTrailing(manipulatedString, stringToBeRemoved,
                                        1);
      TUASSERTE(std::string, resultString, manipulatedString);

      manipulatedString = baseString;
      resultString = baseString.substr(0, baseString.length()-3);
         // Strip all of the character -'s from the end of the string
      manipulatedString = stripTrailing(manipulatedString, charToBeRemoved);
      TUASSERTE(std::string, resultString, manipulatedString);

      manipulatedString = baseString;
      resultString = baseString.substr(0, baseString.length()-2);
         // Strip all of the string --'s from the end of the string
      manipulatedString = stripTrailing(manipulatedString, stringToBeRemoved);
      TUASSERTE(std::string, resultString, manipulatedString);
			
      TURETURN();
   }


      /**
       * Tests for the strip method.
       * Given a baseString and a filler character and string, the
       * tests will strip the baseString of the request string or
       * character from both ends of the string.
       */
   unsigned stripTest()
   {
      TUDEF("StringUtils", "strip");
      string baseString= "---1234567890---"; // base string to manipulate
      string resultString;           // storage for the correct response
      string manipulatedString;      // storage for the manipulated base string
      char charToBeRemoved = '-';
      string stringToBeRemoved = "--";

      manipulatedString = baseString;
      resultString = baseString.substr(1,baseString.length()-2);
         // Strip one of the character -'s from both ends of the string
      manipulatedString = strip(manipulatedString, charToBeRemoved, 1);
      TUASSERTE(std::string, resultString, manipulatedString);

      manipulatedString = baseString;
      resultString = baseString.substr(2,baseString.length()-4);
         // Strip one of the string --'s from both ends of the string
      manipulatedString = strip(manipulatedString, stringToBeRemoved, 1);
      TUASSERTE(std::string, resultString, manipulatedString);

      manipulatedString = baseString;
      resultString = baseString.substr(3, baseString.length()-6);
         // Strip all of the character -'s from both ends of the string
      manipulatedString = strip(manipulatedString, charToBeRemoved);
      TUASSERTE(std::string, resultString, manipulatedString);

      manipulatedString = baseString;
      resultString = baseString.substr(2, baseString.length()-4);
         // Strip all of the string --'s from both ends of the string
      manipulatedString = strip(manipulatedString, stringToBeRemoved);
      TUASSERTE(std::string, resultString, manipulatedString);
			
      TURETURN();
   }


      /**
       * Tests for the translate method.
       * Given a baseString and a translation strings, the baseString
       * will be "translated" using those translation strings.
       */
   unsigned translateTest()
   {
      TUDEF("StringUtils", "strip");
         // Original String to replace characters on
      string baseString = "Hello World! I am the Translation test String!";
         // storage for manipulated base string
      string manipulatedString;
         // Characters to replace
      string translateFrom = "aeoWTS!";
         // Characters that will replace the ones in translateFrom
      string translateTo1 = "@30M7Q1";
         // Correctly translated base string with translateTo1
      string expResult1 = "H3ll0 M0rld1 I @m th3 7r@nsl@ti0n t3st Qtring1";
         // Characters that will replace the ones in translateFrom
      string translateTo2 = "@30M7";
         // Correctly translated base string with translateTo2
      string expResult2 = "H3ll0 M0rld  I @m th3 7r@nsl@ti0n t3st  tring ";
         // Characters that will replace the ones in translateFrom
      string translateTo3 = "aeoWTS!";
         // Correctly translated base string with translateTo3
      string expResult3 = "Hello World! I am the Translation test String!";

      manipulatedString = translate(baseString, translateFrom, translateTo1);
      TUASSERTE(std::string, expResult1, manipulatedString);

      manipulatedString = translate(baseString, translateFrom, translateTo2);
      TUASSERTE(std::string, expResult2, manipulatedString);

      manipulatedString = translate(baseString, translateFrom, translateTo3);
      TUASSERTE(std::string, expResult3, manipulatedString);

      TURETURN();
   }


      /**
       * Tests for the string to number methods.
       * Given a baseString and a target number type, the methods
       * should generate the appropriate values.
       */
   unsigned stringToNumberTest()
   {
      TUDEF("StringUtils", "asInt");
         // String to convert to a number
      string baseString = "12345.67890";
         // Storage for converted string and the expected result value
      int resultingInt, comparisonInt = 12345;
         // Storage for converted string and the expected result value
      double resultingDouble, comparisonDouble = 12345.67890;
         // Storage for converted string and the expected result value
      float resultingFloat, comparisonFloat = 12345.67890f;
         // Storage for converted string and the expected result value
      long double resultingLongDouble, comparisonLongDouble = 12345.67890L;

      resultingInt = asInt(baseString);
      TUASSERTE(int, comparisonInt, resultingInt);

      TUCSM("asDouble");
      resultingDouble = asDouble(baseString);
      TUASSERTE(double, comparisonDouble, resultingDouble);

      TUCSM("asFloat");
      resultingFloat = asFloat(baseString);
      TUASSERTE(float, comparisonFloat, resultingFloat);

      TUCSM("asLongDouble");
      resultingLongDouble = asLongDouble(baseString);
      TUASSERTE(long double, comparisonLongDouble, resultingLongDouble);

      TURETURN();
   }


      /**
       * Tests for the number to string method.
       * Given numbers of various types, convert them to a string and
       * compare against the correct known value.
       *
       * @note In the previous test the string to number methods are
       * tested.  It is okay to use them here as any failures here may
       * be cross-referenced with the results of the previous test.
       */
   unsigned numberToStringTest()
   {
      TUDEF("StringUtils", "asString");
      string convertedString;                   // Storage for converted number
      int sampleInt = 1234567890;               // Example integer to test
      double sampleDouble = 12345.67890;        // Example double to test
      float sampleFloat = 12345.7f;             // Example float to test
         // Example long double to test
      long double sampleLongDouble = 12345.67890L;

      convertedString = asString(sampleInt);
      TUASSERTE(int, sampleInt, asInt(convertedString));

      convertedString = asString(sampleDouble);
      TUASSERTE(double, sampleDouble, asDouble(convertedString));

      convertedString = asString(sampleFloat);
      TUASSERTE(float, sampleFloat, asFloat(convertedString));

      convertedString = asString(sampleLongDouble);
      TUASSERTE(long double, sampleLongDouble, asLongDouble(convertedString));

      TURETURN();
   }


      /**
       * Tests for the hex converters.
       * Given strings with hex and decimal values, convert the values
       * to the opposite type then compare with expected results.
       */
   unsigned hexConversionTest()
   {
      TUDEF("StringUtils", "x2d");
         // Example string with hex values in it
      string initialHexString = "0x100 foo";
         // Resulting string when the hex -> decimal conversion was made
      string correctConvertedHexString = "256 foo";
         // Example decimal string
      string initialDecimalString ="100 bar";
         // Resulting string when the decimal -> hex conversion was made
      string correctConvertedDecimalString = "64 bar";
         // Storage for manipulated strings
      string manipulatedResult;

      manipulatedResult = x2d(initialHexString);
      TUASSERTE(std::string, correctConvertedHexString, initialHexString);

      TUCSM("d2x");
      manipulatedResult = d2x(initialDecimalString);
      TUASSERTE(std::string, correctConvertedDecimalString,
                initialDecimalString);

      TURETURN();
   }


      /**
       * Tests for the string replace methods.
       * Provided a string, replace substrings within it with other
       * substrings and compare with expected results.
       *
       * @warning Method will perform the replace on the string passed
       * to it as well!
       */
   unsigned stringReplaceTest()
   {
      TUDEF("StringUtils", "replaceAll");
      string baseString = "010101000111010";
      string replaced1s = "0-0-0-000---0-0";
      string replaced1withABC = "0ABC0ABC0ABC000ABCABCABC0ABC0";
      string replaced1with123 = "01230123012300012312312301230";
      string manipulatedString;

      manipulatedString = baseString;
      manipulatedString = replaceAll(manipulatedString, string("1"),
                                     string("-"));
      TUASSERTE(std::string, replaced1s, manipulatedString);

      manipulatedString = baseString;
      manipulatedString = replaceAll(manipulatedString, string("1"),
                                     string("ABC"));
      TUASSERTE(std::string, replaced1withABC, manipulatedString);

      manipulatedString = baseString;
      manipulatedString = replaceAll(manipulatedString, string("1"),
                                     string("123"));
      TUASSERTE(std::string, replaced1with123, manipulatedString);

      TURETURN();
   }


      /**
       * Tests for the word methods.
       * @warning Removal methods will perform the replace on the
       * string passed to it as well!.
       */
   unsigned wordTest()
   {
      TUDEF("StringUtils", "firstWord");
         // Original string to manipulate
      string originalString = "      The  Quick    Brown  Fox     Jumps  Over"
         "    The  Lazy    Dog.    ";
         // Expected result for use of firstWord with the original string
      string firstWordInString = "The";
         // Number of words in the original string
      int numberOfWords = 9;
      int resultInt;
         // Expected result for the fifth word in the original string
      string fifthWordInString = "Jumps";
         // Expected result for removal of the 4th and 5th words
      string removedFourthFifthWords = "      The  Quick    Brown  Over    The"
         "  Lazy    Dog.    ";
         // Expected result for the removal of the 1st word in
         // removedFourthFifthWords
      string removedFirstWord = "Quick    Brown  Over    The  Lazy    "
         "Dog.    ";
         // Expected result of the removal of the sixth word in
         // removedFirstWord
      string removedSixthWord = "Quick    Brown  Over    The  Lazy";
         // Expected result of the reporting of the words from the
         // fourth to the end of removedSixthWord
      string allWordsFromFourthOn = "The  Lazy";
         // String to store resulting data.
      string resultString;

      resultString = firstWord("");
      TUASSERTE(std::string, "", resultString);

      resultString = firstWord(originalString);
      TUASSERTE(std::string, firstWordInString, resultString);

      TUCSM("numWords");
      resultInt = numWords("");
      TUASSERTE(int, 0, resultInt);

      resultInt = numWords(originalString);
      TUASSERTE(int, numberOfWords, resultInt);

      TUCSM("word");
      resultString = word("",2);
      TUASSERTE(std::string, "", resultString);

      resultString = word(originalString,4);
      TUASSERTE(std::string, fifthWordInString, resultString);

      resultString = word(originalString,10);
      TUASSERTE(std::string, "", resultString);

      std::string  empty;

      TUCSM("removeWords");
         // This changes the string passed to the method
      resultString = removeWords(empty, 1, 1);
      TUASSERTE(std::string, "", resultString);

      resultString = removeWords(originalString, 3, 2);
      TUASSERTE(std::string, removedFourthFifthWords, originalString);
      TUASSERTE(std::string, removedFourthFifthWords, resultString);

      TUCSM("stripFirstWord");
         // This changes the string passed to the method
      resultString = stripFirstWord(empty);
      TUASSERTE(std::string, "", resultString);

      resultString = stripFirstWord(originalString);
      TUASSERTE(std::string, removedFirstWord, originalString);
      TUASSERTE(std::string, firstWordInString, resultString);

      TUCSM("removeWords");
         // This changes the string passed to the method
      resultString = removeWords(empty, 2);
      TUASSERTE(std::string, "", resultString);

      resultString = removeWords(originalString, 5);
      TUASSERTE(std::string, removedSixthWord, originalString);
      TUASSERTE(std::string, removedSixthWord, resultString);

      std::string  removeAllOfMe(originalString);
      resultString = removeWords(removeAllOfMe, 0);
      TUASSERTE(std::string, "", removeAllOfMe);
      TUASSERTE(std::string, "", resultString);

      TUCSM("words");
      resultString = words("", 2);
      TUASSERTE(std::string, std::string(), resultString);
      
      resultString = words(originalString, 3);
      TUASSERTE(std::string, allWordsFromFourthOn, resultString);

      resultString = words(originalString, 10);
      TUASSERTE(std::string, std::string(), resultString);

      TURETURN();
   }


      /**
       * Tests for the regular expression methods.
       * Provided a string and a regular expression query, the isLike
       * method will return 0 or 1 if a match is found.  Matches will
       * then provide the found result.
       */
   unsigned regularExpressionsTest()
   {
      TUDEF("StringUtils", "isLike");
         // String to be searched
      string originalString = "      The  Quick    Brown  Fox     Jumps  Over"
         "    The  Lazy    Dog.    ";
         // Storage for isLike result
      int resultInt;
         // A query which will be found
      string testedRegularExpression1 = "Br.*Over.*";
         // A query which will not be found
      string testedRegularExpression2 = "Br.Over.*";
         // Storage for matches method
      string resultString;
         // Expected string to be found
      string correctResult = "Brown  Fox     Jumps  Over    The  Lazy"
         "    Dog.    ";

      resultInt = isLike(originalString, testedRegularExpression1);
      TUASSERTE(int, 1, resultInt);

      resultInt = isLike(originalString, testedRegularExpression2);
      TUASSERTE(int, 0, resultInt);

      TUCSM("matches");
      resultString = matches(originalString, testedRegularExpression1);
      TUASSERTE(std::string, correctResult, resultString);

      TURETURN();
   }


      /**
       * Test for the prettyPrint method.
       * prettyPrint will take a string and split the information into
       * formatted lines.
       */
   unsigned prettyPrintTest()
   {
      TUDEF("StringUtils", "prettyPrint");
      string originalString("This is a very long sentence that will get cut"
                            " up into lines with a maximum length of 40"
                            " characters, each line indented by a few"
                            " spaces.");
      string prettyPrintThis = originalString;
      string prettyPrinted = "       This is a very long sentence that\n"
         "     will get cut up into lines with a\n"
         "     maximum length of 40 characters,\n"
         "     each line indented by a few spaces.\n";

      prettyPrint(prettyPrintThis, "\n", "    ", "      ", 40);
      TUASSERTE(std::string, prettyPrinted, prettyPrintThis);

      prettyPrintThis = "ThisXtestsXprintingXwithXaXwordXdelimiterXotherXthan"
         "XspaceXasXwellXasXlineXdelimetersXotherXthanXnewline";
      prettyPrinted = "mehhhhhhhhXThisXtestsXprintingXwithXaZZZZcruddycrudcrud"
         "XwordXdelimiterXotherZZZZcruddycrudcrudXthanXspaceXasXwellXasZZZZ"
         "cruddycrudcrudXlineXdelimetersXotherZZZZcruddycrudcrudXthanXnewline"
         "ZZZZ";
      prettyPrint(prettyPrintThis, "ZZZZ", "cruddycrudcrud", "mehhhhhhhh", 37,
                  'X');
      TUASSERTE(std::string, prettyPrinted, prettyPrintThis);

         // This tests handling of newlines which is useful for
         // command line option documentation in particular.
      originalString = "Bit-field: inject errors"
         "\nbit 0 - corrupt lengths"
         "\nbit 1 - corrupt addressing"
         "\nbit 2 - corrupt checksum"
         "\nbit 3 - corrupt `format'";
      prettyPrintThis = originalString;
      prettyPrinted = " -j, --error-inj=NUM     Bit-field: inject errors\n"
                      "                         bit 0 - corrupt lengths\n"
                      "                         bit 1 - corrupt addressing\n"
                      "                         bit 2 - corrupt checksum\n"
                      "                         bit 3 - corrupt `format'\n";
      prettyPrint(prettyPrintThis, "\n", "                        ",
                  " -j, --error-inj=NUM    ", 80);
      TUASSERTE(std::string, prettyPrinted, prettyPrintThis);

      TURETURN();
   }


      /**
       * Test for the splitWithQuotes method.
       * splitWithQuotes will split a string into a
       * std::vector<std::string> respecting single and double quoted
       * strings.
       */
   unsigned splitWithQuotesTest()
   {
      TUDEF("StringUtils", "splitWithQuotes");
         // no quotes
      std::string originalString("String with no quotes");
      std::vector<std::string> expectedResult;
      expectedResult.push_back("String");
      expectedResult.push_back("with");
      expectedResult.push_back("no");
      expectedResult.push_back("quotes");
      std::vector<std::string> observedResult =
         splitWithQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // empty string
      originalString = "";
      expectedResult.clear();
      observedResult = splitWithQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // only whitespace
      originalString = " ";
      expectedResult.clear();
      expectedResult.push_back("");
      expectedResult.push_back("");
      observedResult = splitWithQuotes(originalString,' ',false,false);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // double quotes
      originalString = std::string("String with \"double quoted values\"");
      expectedResult.clear();
      expectedResult.push_back("String");
      expectedResult.push_back("with");
      expectedResult.push_back("double quoted values");
      observedResult = splitWithQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // single quotes
      originalString= std::string("String with \'single quoted values\'");
      expectedResult.clear();
      expectedResult.push_back("String");
      expectedResult.push_back("with");
      expectedResult.push_back("single quoted values");
      observedResult = splitWithQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // leading quote
      originalString= std::string("\"First words\" quoted");
      expectedResult.clear();
      expectedResult.push_back("First words");
      expectedResult.push_back("quoted");
      observedResult = splitWithQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // back to back quotes
      originalString= std::string("\"Back to\" \"back quotes\"");
      expectedResult.clear();
      expectedResult.push_back("Back to");
      expectedResult.push_back("back quotes");
      observedResult = splitWithQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // empty, non-whitespace fields
      originalString =
         std::string("Zan.35(KB04-KB01), +90  7  7.73619,,,,-1.52405,no,,,");
      expectedResult.clear();
      expectedResult.push_back("Zan.35(KB04-KB01)");
      expectedResult.push_back("+90  7  7.73619");
      expectedResult.push_back("");
      expectedResult.push_back("");
      expectedResult.push_back("");
      expectedResult.push_back("-1.52405");
      expectedResult.push_back("no");
      expectedResult.push_back("");
      expectedResult.push_back("");
      expectedResult.push_back("");
      observedResult = splitWithQuotes(originalString,',',true,false);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

      TURETURN();
   }

      /**
       * Test for the splitWithDoubleQuotes method.
       * splitWithDoubleQuotes will split a string into a
       * std::vector<std::string> respecting double quoted strings.
       */
   unsigned splitWithDoubleQuotesTest()
   {
      TUDEF("StringUtils", "splitWithDoubleQuotes");
         // no quotes
      std::string originalString("String with no quotes");
      std::vector<std::string> expectedResult;
      expectedResult.push_back("String");
      expectedResult.push_back("with");
      expectedResult.push_back("no");
      expectedResult.push_back("quotes");
      std::vector<std::string> observedResult =
         splitWithDoubleQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // empty string
      originalString = "";
      expectedResult.clear();
      observedResult = splitWithDoubleQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // only whitespace
      originalString = " ";
      expectedResult.clear();
      expectedResult.push_back("");
      expectedResult.push_back("");
      observedResult = splitWithDoubleQuotes(originalString,' ',false,false);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // double quotes
      originalString = std::string("String with \"double quoted values\"");
      expectedResult.clear();
      expectedResult.push_back("String");
      expectedResult.push_back("with");
      expectedResult.push_back("double quoted values");
      observedResult = splitWithDoubleQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // single quotes
      originalString= std::string("String with \'single quoted values\'");
      expectedResult.clear();
      expectedResult.push_back("String");
      expectedResult.push_back("with");
      expectedResult.push_back("\'single");
      expectedResult.push_back("quoted");
      expectedResult.push_back("values\'");
      observedResult = splitWithDoubleQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // leading quote
      originalString= std::string("\"First words\" quoted");
      expectedResult.clear();
      expectedResult.push_back("First words");
      expectedResult.push_back("quoted");
      observedResult = splitWithDoubleQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // back to back quotes
      originalString= std::string("\"Back to\" \"back quotes\"");
      expectedResult.clear();
      expectedResult.push_back("Back to");
      expectedResult.push_back("back quotes");
      observedResult = splitWithDoubleQuotes(originalString);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);

         // empty, non-whitespace fields
      originalString =
         std::string("Zan.35(KB04-KB01), +90  7  7.73619,,,,-1.52405,no,,,");
      expectedResult.clear();
      expectedResult.push_back("Zan.35(KB04-KB01)");
      expectedResult.push_back("+90  7  7.73619");
      expectedResult.push_back("");
      expectedResult.push_back("");
      expectedResult.push_back("");
      expectedResult.push_back("-1.52405");
      expectedResult.push_back("no");
      expectedResult.push_back("");
      expectedResult.push_back("");
      expectedResult.push_back("");
      observedResult = splitWithDoubleQuotes(originalString,',',true,false);
      TUASSERTE(std::vector<std::string>, expectedResult, observedResult);


      TURETURN();
   }


      /**
       * Test for the hexDump methods.
       *
       * This test is intended to verify that the unique ASCII
       * characters are all transformed from hex and output properly.
       *
       * The hexDataDump method is to display hex data in ASCII
       * format. This output may be configured using the
       * configHexDataDump method, or output without the comparison
       * hex code using the printable method.
       */
   unsigned hexToAsciiTest()
   {
      TUDEF("StringUtils", "hexDumpData");
      stringstream hexDumpStream;
      string correctHexDump;
      stringstream configHexDumpStream;
      string correctConfigHexDump;
      string correctPrintableMessage;
      stringstream printableMessageStream;

         // Correct format for hexDumpData
      string refPath = gpstk::getPathData() +
         gpstk::getFileSep() +
         "StringUtils" +
         gpstk::getFileSep() +
         "hexToAsciiTest_";
      correctHexDump = getFileContents(refPath + "hexDump.exp");

         // Correct format for the configured hexDumpData.
      correctConfigHexDump = getFileContents(refPath + "configHexDump.exp");

         // Build the hexDumpString and output it to stringstream
      string hexDumpString(hexDumpArray1, sizeof(hexDumpArray1));
      hexDumpData(hexDumpStream, hexDumpString);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());

         // configure the hexDumpData, dump it to output stringstream
      HexDumpDataConfig hdcfg(true, true, true, 8, 2, 2, 1, 0, 0, 16, true,
                              '\'', 1);
      hexDumpData(configHexDumpStream, hexDumpString, 6, hdcfg);
      TUCSM("HexDumpDataConfig");
      TUASSERTE(std::string, correctConfigHexDump, configHexDumpStream.str());

         // Correct format for the printable
      correctPrintableMessage = getFileContents(refPath +
                                                "printableMessage.exp");
      printableMessageStream << printable(hexDumpString) ;
      TUCSM("printable");
      TUASSERTE(std::string, correctPrintableMessage,
                printableMessageStream.str());

      TURETURN();
   }


      /**
       * Test for the hexDump methods.
       * This test is intended to verify that the output formats are
       * correct.  There are no characters that could be unique to the
       * system configuration in these lists.
       */
   unsigned hexDumpDataTest()
   {
      TUDEF("StringUtils", "hexDumpData");
      stringstream hexDumpStream;
      string correctHexDump;
      stringstream configHexDumpStream;
      string correctConfigHexDump;
      string correctPrintableMessage;
      stringstream printableMessageStream;

         // Correct format for hexDumpData
      string refPath = gpstk::getPathData() +
         gpstk::getFileSep() +
         "StringUtils" +
         gpstk::getFileSep() +
         "hexDumpDataTest_";
      correctHexDump = getFileContents(refPath + "hexDump.exp");

         // Correct format for the configured hexDumpData
      correctConfigHexDump = getFileContents(refPath + "configHexDump.exp");

         // Build the hexDumpString and output it to stringstream
      string hexDumpString(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());

         // configure the hexDumpData, dump it to output stringstream
      HexDumpDataConfig hdcfg(true, true, true, 8, 2, 2, 1, 0, 0, 16, true,
                              '\'', 1);
      hexDumpData(configHexDumpStream, hexDumpString, 6, hdcfg);
      TUCSM("HexDumpDataConfig");
      TUASSERTE(std::string, correctConfigHexDump, configHexDumpStream.str());

         // Correct format for the printable
      correctPrintableMessage = getFileContents(refPath +
                                                "printableMessage.exp");
      printableMessageStream << printable(hexDumpString) ;
      TUCSM("printable");
      TUASSERTE(std::string, correctPrintableMessage,
                printableMessageStream.str());

      TURETURN();
   }


      /**
       * Test stream flags support for hexDumpData, making sure stream
       * state is unaltered on return.
       */
   unsigned hexDumpDataStreamFlagTest()
   {
      TUDEF("StringUtils", "hexDumpData");
      stringstream hexDumpStream;
      string correctHexDump;

         // Correct format for hexDumpData
      string refPath = gpstk::getPathData() +
         gpstk::getFileSep() +
         "StringUtils" +
         gpstk::getFileSep() +
         "hexDumpDataStreamFlagTest_";

      correctHexDump = getFileContents(refPath + "hexDump.exp");
         // set some stream flags that could mess up the output of hexDumpData
      hexDumpStream << std::left << setw(99) << setfill('Z') << boolalpha
                    << showbase << oct << scientific << showpos << uppercase;
         // Build the hexDumpString and output it to stringstream
      string hexDumpString(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString);
      hexDumpStream << "foo" << " " << true << " " << false << " " << 12 << " "
                    << -1.23e12 << " " << hex << 0xdeadbeef << dec << endl;
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());

      TURETURN();
   }


      /**
       * Test various output configurations for hexDumpData.
       */
   unsigned hexDumpDataConfigTest()
   {
      TUDEF("StringUtils", "hexDumpData");
      stringstream hexDumpStream;
      string correctHexDump;
      string hexDumpString;

         // Correct format for hexDumpData
      string refPath = gpstk::getPathData() +
         gpstk::getFileSep() +
         "StringUtils" +
         gpstk::getFileSep() +
         "hexDumpDataConfigTest_";

         // no index
      HexDumpDataConfig cfg1(false, true, false, 4, 1, 1, 1, 8, 2, 16, true,
                             0, 4);
      correctHexDump = getFileContents(refPath + "hexDump_1.exp");
         // Build the hexDumpString and output it to stringstream
      hexDumpString = string(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg1);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg1:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // no index, no ASCII
      HexDumpDataConfig cfg2(false, true, false, 4, 1, 1, 1, 8, 2, 16, false,
                             0, 4);
      correctHexDump = getFileContents(refPath + "hexDump_2.exp");
         // Build the hexDumpString and output it to stringstream
      hexDumpString = string(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg2);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg2:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // no index, no ASCII, no second-level grouping
      HexDumpDataConfig cfg3(false, true, false, 4, 1, 1, 1, 0, 2, 16, false,
                             0, 4);
      correctHexDump = getFileContents(refPath + "hexDump_3.exp");
         // Build the hexDumpString and output it to stringstream
      hexDumpString = string(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg3);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg3:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // no index, no ASCII, no second-level grouping, 8 bytes per line
      HexDumpDataConfig cfg4(false, true, false, 4, 1, 1, 1, 0, 2, 8, false,
                             0, 4);
      correctHexDump = getFileContents(refPath + "hexDump_4.exp");
         // Build the hexDumpString and output it to stringstream
      hexDumpString = string(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg4);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg4:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // above + data base
      HexDumpDataConfig cfg5(false, true, false, 4, 1, 1, 1, 0, 2, 8, false,
                             0, 4, true);
      correctHexDump = getFileContents(refPath + "hexDump_5.exp");
         // Build the hexDumpString and output it to stringstream
      hexDumpString = string(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg5);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg5:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // testing text separators
      HexDumpDataConfig cfg6(true, true, false, 6, ":MEH", 1, "y", 8, "zz",
                             16, true, 't', "FOO", true, true, "", "");
      correctHexDump = getFileContents(refPath + "hexDump_6.exp");
         // Build the hexDumpString and output it to stringstream
      hexDumpString = string(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg6);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg6:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // and this is where I was going with the separator changes:
         // a hexDumpData output that could be relatively easily copy
         // pasted into C/C++ code.
      HexDumpDataConfig cfg7(false, false, false, 4, "", 1, ", ", 0, "zz",
                             8, false, 't', "FOO", true, true, ",", "");
      correctHexDump = getFileContents(refPath + "hexDump_7.exp");
         // Build the hexDumpString and output it to stringstream
      hexDumpString = string(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg7);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg7:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // testing text separators and index bases w/ capitalization
      HexDumpDataConfig cfg8(true, true, true, 6, ":MEH", 1, "y", 8, "zz",
                             16, true, 't', "FOO", true, true, "meep",
                             "shiftthissuckertotherightsome");
      correctHexDump = getFileContents(refPath + "hexDump_8.exp");
         // Build the hexDumpString and output it to stringstream
      hexDumpString = string(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg8);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg8:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // testing text alignment with two-byte words
      HexDumpDataConfig cfg9(true, true, true, 6, ".", 2, "!", 0, "",
                             16, true, '`', "_", true, true, "meep",
                             "shiftthissuckertotherightsome");
      correctHexDump = getFileContents(refPath + "hexDump_9.exp");
         // Build the hexDumpString and output it to stringstream
      hexDumpString = string(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg9);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg9:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // testing where the final line is the same number of bytes
         // as every other line.
      HexDumpDataConfig cfg10(true, true, true, 6, ".", 2, "!", 0, "",
                             16, true, '`', "_", true, true, "meep",
                             "shiftthissuckertotherightsome");
      correctHexDump = getFileContents(refPath + "hexDump_10.exp");
         // Build the hexDumpString and output it to stringstream
      unsigned truesize = sizeof(hexDumpArray2);
      hexDumpString = string(hexDumpArray2, truesize - (truesize % 16));
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg10);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg10:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // testing fill
      HexDumpDataConfig cfg11(false, false, false, 0, "", 1, ", ", 0, "", 8,
                              false, (char)0, "", true, false, ",", "");
      correctHexDump = getFileContents(refPath + "hexDump_11.exp");
         // Build the hexDumpString and output it to stringstream
      unsigned char allChars[256];
      for (unsigned i = 0; i < 256; i++)
         allChars[i] = i;
      hexDumpString = string((char*)allChars, 256);
      hexDumpData(hexDumpStream, hexDumpString, 0, cfg11);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg11:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

         // After updating hexDumpData to support a pre- and
         // post-ascii pair of strings rather than a single character
         // on either side. (e.g. for C-style comments)
      HexDumpDataConfig cfg12(false, false, false, 0, "", 1, ", ", 0, "",
                              8, true, "    /* ", " */", true, false, ",", "",
                              "      ");
      correctHexDump = getFileContents(refPath + "hexDump_12.exp");
         // Build the hexDumpString and output it to stringstream
      hexDumpString = string(hexDumpArray2, sizeof(hexDumpArray2));
      hexDumpData(hexDumpString, hexDumpStream, cfg12);
      TUASSERTE(std::string, correctHexDump, hexDumpStream.str());
         //cout << "cfg12:" << endl << hexDumpStream.str() << endl;
      hexDumpStream.str("");

      TURETURN();
   }
};

int main() // Main function to initialize and run all tests above
{
   unsigned errorTotal = 0;
   StringUtils_T testClass;

   errorTotal += testClass.justificationTest();
   errorTotal += testClass.stripLeadingTest();
   errorTotal += testClass.stripTrailingTest();
   errorTotal += testClass.stripTest();
   errorTotal += testClass.stringToNumberTest();
   errorTotal += testClass.numberToStringTest();
   errorTotal += testClass.hexConversionTest();
   errorTotal += testClass.stringReplaceTest();
   errorTotal += testClass.wordTest();
   errorTotal += testClass.regularExpressionsTest();
   errorTotal += testClass.prettyPrintTest();
   errorTotal += testClass.splitWithQuotesTest();
   errorTotal += testClass.splitWithDoubleQuotesTest();
   errorTotal += testClass.hexDumpDataTest();
   errorTotal += testClass.hexDumpDataStreamFlagTest();
   errorTotal += testClass.hexDumpDataConfigTest();
   errorTotal += testClass.hexToAsciiTest();

   std::cout << "Total Failures for " << __FILE__ << ": " << errorTotal
             << std::endl;

   return errorTotal;
}
