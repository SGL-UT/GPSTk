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
#include "StringUtils.hpp"
#include "TestUtil.hpp"

using namespace gpstk::StringUtils;
using namespace std;

/* =========================================================================================================================
	This test file will contain a series of scanf checks for each of the directly tested TimeTag classes.
========================================================================================================================= */

class StringUtils_T
{
public:
   StringUtils_T() {}
   ~StringUtils_T() {}

/* =========================================================================================================================
   Tests for the justification methods
   Given a baseString and a filler character, the tests will justify the 
   string to a new string of the size of the original string + 6. 
   ========================================================================================================================= */
   int justificationTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "Justify", __FILE__, __LINE__ );
      std::string failMesg;

      string baseString= "1234567890"; //base string to manipulate
      string resultString;             //storage for the correct response
      string manipulatedString;        //storage for the manipulated base string
      char fillerChar = '-';

      manipulatedString = baseString;
      resultString = string(6, fillerChar) + baseString;
      //Add six filler characters to the front of the string 
      rightJustify(manipulatedString, manipulatedString.length() + 6, fillerChar);

      failMesg = "Verify that rightJustify works by appending 6 '-'s to the left";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      manipulatedString = baseString;
      resultString = baseString + string(6, fillerChar);
      //Add six filler characters to the end of the string
      leftJustify(manipulatedString, manipulatedString.length()+6, fillerChar);

      failMesg = "Verify that leftJustify works by appending 6 '-'s to the right";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);
			  
      manipulatedString = baseString;
      resultString = string(3, fillerChar) + baseString + string(3, fillerChar);
      //Add three filler characters to both ends of the string
      center(manipulatedString, manipulatedString.length()+6, fillerChar);

      failMesg = "Verify that leftJustify works by appending 3 '-'s to both sides";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      return testFramework.countFails();
   }



/* =========================================================================================================================
   Tests for the stripLeading methods
   Given a baseString and a filler character and string, the tests will strip the 
   baseString of the request string or character from the front of the string.
   ========================================================================================================================= */
   int stripLeadingTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "stripLeading", __FILE__, __LINE__ );
      std::string failMesg;

      string baseString= "---1234567890---";  //base string to manipulate
      string resultString;                    //storage for the correct response
      string manipulatedString;               //storage for the manipulated base string
      char charToBeRemoved = '-';
      string stringToBeRemoved = "--";

      manipulatedString = baseString;
      resultString = baseString.substr(1,string::npos);
      //Strip one of the character -'s from the beginning of the string
      manipulatedString = stripLeading(manipulatedString, charToBeRemoved, 1);

      failMesg = "Verify that stripLeading works by removing 1 '-' from the beginning of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      manipulatedString = baseString;
      resultString = baseString.substr(2,string::npos);
      //Strip one of the string --'s from the beginning of the string
      manipulatedString = stripLeading(manipulatedString, stringToBeRemoved, 1);

      failMesg = "Verify that stripLeading works by removing 1 '--' from the beginning of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      manipulatedString = baseString;
      resultString = baseString.substr(3,string::npos);
      //Strip all of the character -'s from the beginning of the string
      manipulatedString = stripLeading(manipulatedString, charToBeRemoved);

      failMesg = "Verify that stripLeading works by removing all '-' from the beginning of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      manipulatedString = baseString;
      resultString = baseString.substr(2,string::npos);
      //Strip all of the string --'s from the beginning of the string
      manipulatedString = stripLeading(manipulatedString, stringToBeRemoved);

      failMesg = "Verify that stripLeading works by removing all '--' from the beginning of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);
			
      return testFramework.countFails();
   }


/* =========================================================================================================================
   Tests for the stripTrailing methods
   Given a baseString and a filler character and string, the tests will strip the 
   baseString of the request string or character from the end of the string.
   ========================================================================================================================= */
   int stripTrailingTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "stripTrailing", __FILE__, __LINE__ );
      std::string failMesg;

      string baseString= "---1234567890---"; //base string to manipulate
      string resultString;                   //storage for the correct response
      string manipulatedString;              //storage for the manipulated base string
      char charToBeRemoved = '-';
      string stringToBeRemoved = "--";

      manipulatedString = baseString;
      resultString = baseString.substr(0, baseString.length()-1);
      //Strip one of the character -'s from the end of the string
      manipulatedString = stripTrailing(manipulatedString, charToBeRemoved, 1);

      failMesg = "Verify that stripTrailing works by removing 1 '-' from the end of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      manipulatedString = baseString;
      resultString = baseString.substr(0, baseString.length()-2);
      //Strip one of the string --'s from the end of the string
      manipulatedString = stripTrailing(manipulatedString, stringToBeRemoved, 1);

      failMesg = "Verify that stripTrailing works by removing 1 '--' from the end of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      manipulatedString = baseString;
      resultString = baseString.substr(0, baseString.length()-3);
      //Strip all of the character -'s from the end of the string
      manipulatedString = stripTrailing(manipulatedString, charToBeRemoved);

      failMesg = "Verify that stripTrailing works by removing all '-' from the end of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      manipulatedString = baseString;
      resultString = baseString.substr(0, baseString.length()-2);
      //Strip all of the string --'s from the end of the string
      manipulatedString = stripTrailing(manipulatedString, stringToBeRemoved);

      failMesg = "Verify that stripTrailing works by removing all '--' from the end of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);
			
      return testFramework.countFails();
   }


/* =========================================================================================================================
   Tests for the strip method
   Given a baseString and a filler character and string, the tests will strip the 
   baseString of the request string or character from both ends of the string.
   ========================================================================================================================= */
   int stripTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "strip", __FILE__, __LINE__ );
      std::string failMesg;

      string baseString= "---1234567890---"; //base string to manipulate
      string resultString;                   //storage for the correct response
      string manipulatedString;              //storage for the manipulated base string
      char charToBeRemoved = '-';
      string stringToBeRemoved = "--";

      manipulatedString = baseString;
      resultString = baseString.substr(1,baseString.length()-2);
      //Strip one of the character -'s from both ends of the string
      manipulatedString = strip(manipulatedString, charToBeRemoved, 1);

      failMesg = "Verify that strip works by removing 1 '-' from both ends of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      manipulatedString = baseString;
      resultString = baseString.substr(2,baseString.length()-4);
      //Strip one of the string --'s from both ends of the string
      manipulatedString = strip(manipulatedString, stringToBeRemoved, 1);

      failMesg = "Verify that strip works by removing 1 '--' from both ends of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      manipulatedString = baseString;
      resultString = baseString.substr(3, baseString.length()-6);
      //Strip all of the character -'s from both ends of the string
      manipulatedString = strip(manipulatedString, charToBeRemoved);

      failMesg = "Verify that strip works by removing all '-' from both ends of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);

      manipulatedString = baseString;
      resultString = baseString.substr(2, baseString.length()-4);
      //Strip all of the string --'s from both ends of the string
      manipulatedString = strip(manipulatedString, stringToBeRemoved);

      failMesg = "Verify that strip works by removing all '--' from both ends of the string";
      testFramework.assert(manipulatedString == resultString, failMesg, __LINE__);
			
      return testFramework.countFails();
   }


/* =========================================================================================================================
   Tests for the translate method
   Given a baseString and a translation strings, the baseString will be "translated"
   using those translation strings.
   ========================================================================================================================= */
   int translateTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "strip", __FILE__, __LINE__ );
      std::string failMesg;

      string baseString = "Hello World! I am the Translation test String!";      //Original String to replace characters on
      string manipulatedString;                                                  //storage for manipulated base string
      string translateFrom = "aeoWTS!";                                          //Characters to replace
      string translateTo1 = "@30M7Q1";                                           //Characters that will replace the ones in translateFrom
      string expectedResult1 = "H3ll0 M0rld1 I @m th3 7r@nsl@ti0n t3st Qtring1"; //Correctly translated base string with translateTo1
      string translateTo2 = "@30M7";                                             //Characters that will replace the ones in translateFrom
      string expectedResult2 = "H3ll0 M0rld  I @m th3 7r@nsl@ti0n t3st  tring "; //Correctly translated base string with translateTo2
      string translateTo3 = "aeoWTS!";                                           //Characters that will replace the ones in translateFrom
      string expectedResult3 = "Hello World! I am the Translation test String!"; //Correctly translated base string with translateTo3

      manipulatedString = translate(baseString, translateFrom, translateTo1);
      failMesg = "Verify that translate works by changing multiple characters";
      testFramework.assert(manipulatedString == expectedResult1, failMesg, __LINE__);

      manipulatedString = translate(baseString, translateFrom, translateTo2);
      failMesg = "Verify that translate replaces unmatched characters with blanks";
      testFramework.assert(manipulatedString == expectedResult2, failMesg, __LINE__);

      manipulatedString = translate(baseString, translateFrom, translateTo3);
      failMesg = "Verify that translate won't change things when supplied the same set of characters";
      testFramework.assert(manipulatedString == expectedResult3, failMesg, __LINE__);

      return testFramework.countFails();
   }


/* =========================================================================================================================
   Tests for the string to number methods
   Given a baseString and a target number type, the methods should 
   generate the appropriate values.
   ========================================================================================================================= */
   int stringToNumberTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "asInt", __FILE__, __LINE__ );
      std::string failMesg;

      string baseString = "12345.67890";                                    //String to convert to a number
      int resultingInt, comparisonInt = 12345;                              //Storage for converted string and the expected result value
      double resultingDouble, comparisonDouble = 12345.67890;               //Storage for converted string and the expected result value
      float resultingFloat, comparisonFloat = 12345.67890f;                  //Storage for converted string and the expected result value
      long double resultingLongDouble, comparisonLongDouble = 12345.67890L; //Storage for converted string and the expected result value

      resultingInt = asInt(baseString);
      failMesg = "Verify that asInt works as intended";
      testFramework.assert(resultingInt == comparisonInt, failMesg, __LINE__);

      testFramework.changeSourceMethod("asDouble");
      resultingDouble = asDouble(baseString);
      failMesg = "Verify that asDouble works as intended";
      testFramework.assert(resultingDouble == comparisonDouble, failMesg, __LINE__);

      testFramework.changeSourceMethod("asFloat");
      resultingFloat = asFloat(baseString);
      failMesg = "Verify that asFloat works as intended";
      testFramework.assert(resultingFloat == comparisonFloat, failMesg, __LINE__);

      testFramework.changeSourceMethod("asLongDouble");
      resultingLongDouble = asLongDouble(baseString);
      failMesg = "Verify that asLongDouble works as intended";
      testFramework.assert(resultingLongDouble == comparisonLongDouble, failMesg, __LINE__);

      return testFramework.countFails();
   }

/* =========================================================================================================================
   Tests for the number to string method
   Given numbers of various types, convert them to a string and 
   compare against the correct known value.

   Note that in the previous test the string to number methods are tested.
   It is okay to use them here as any failures here may be cross-referenced with the results
   of the previous test.
   ========================================================================================================================= */
   int numberToStringTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "asString", __FILE__, __LINE__ );
      std::string failMesg;

      string convertedString;                     //Storage for converted number
      int sampleInt = 1234567890;                 //Example integer to test
      double sampleDouble = 12345.67890;          //Example double to test
      float sampleFloat = 12345.7f;                //Example float to test
      long double sampleLongDouble = 12345.67890L;//Example long double to test

			
      convertedString = asString(sampleInt);
      failMesg = "Verify that asString converts ints";
      testFramework.assert(asInt(convertedString) == sampleInt, failMesg, __LINE__);

      convertedString = asString(sampleDouble);
      failMesg = "Verify that asString converts doubles";
      testFramework.assert(asDouble(convertedString) == sampleDouble, failMesg, __LINE__);

      convertedString = asString(sampleFloat);
      failMesg = "Verify that asString converts floats";
      testFramework.assert(asFloat(convertedString) == sampleFloat, failMesg, __LINE__);

      convertedString = asString(sampleLongDouble);
      failMesg = "Verify that asString converts long doubles";
      testFramework.assert(asLongDouble(convertedString) == sampleLongDouble, failMesg, __LINE__);

      return testFramework.countFails();
   }


/* =========================================================================================================================
   Tests for the hex converters

   Given strings with hex and decimal values, convert the values to the opposite type
   then compare with expected results
   ========================================================================================================================= */
   int hexConversionTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "x2d", __FILE__, __LINE__ );
      std::string failMesg;

      string initialHexString = "0x100 foo";           //Example string with hex values in it
      string correctConvertedHexString = "256 foo";    //Resulting string when the hex -> decimal conversion was made
      string initialDecimalString ="100 bar";          //Example decimal string
      string correctConvertedDecimalString = "64 bar"; //Resulting string when the decimal -> hex conversion was made
      string manipulatedResult;                        //Storage for manipulated strings

      manipulatedResult = x2d(initialHexString);
      failMesg = "Verify that x2d works";
      testFramework.assert(initialHexString == correctConvertedHexString, failMesg, __LINE__);

      testFramework.changeSourceMethod("d2x");
      manipulatedResult = d2x(initialDecimalString);
      failMesg = "Verify that d2x works";
      testFramework.assert(initialDecimalString == correctConvertedDecimalString, failMesg, __LINE__);

      return testFramework.countFails();
   }

/* =========================================================================================================================
   Tests for the string replace methods

   Provided a string, replace substrings within it with other substrings and compare with expected results.

   WARNING: Method will perform the replace on the string passed to it as well!
   ========================================================================================================================= */
   int stringReplaceTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "replaceAll", __FILE__, __LINE__ );
      std::string failMesg;

      string baseString = "010101000111010";
      string replaced1s = "0-0-0-000---0-0";
      string replaced1withABC = "0ABC0ABC0ABC000ABCABCABC0ABC0";
      string replaced1with123 = "01230123012300012312312301230";
      string manipulatedString;

      manipulatedString = baseString;
      manipulatedString = replaceAll(manipulatedString, string("1"), string("-"));
      failMesg = "Verify that replaceAll works with a character swap";
      testFramework.assert(manipulatedString == replaced1s, failMesg, __LINE__);

      manipulatedString = baseString;
      manipulatedString = replaceAll(manipulatedString, string("1"), string("ABC"));
      failMesg = "Verify that replaceAll works with a character swap";
      testFramework.assert(manipulatedString == replaced1withABC, failMesg, __LINE__);

      manipulatedString = baseString;
      manipulatedString = replaceAll(manipulatedString, string("1"), string("123"));
      failMesg = "Verify that replaceAll works with a character swap";
      testFramework.assert(manipulatedString == replaced1with123, failMesg, __LINE__);

      return testFramework.countFails();
   }


/* =========================================================================================================================
   Tests for the word methods

   WARNING: Removal methods will perform the replace on the string passed to it as well!
   ========================================================================================================================= */
   int wordTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "firstWord", __FILE__, __LINE__ );
      std::string failMesg;


      string originalString = "      The  Quick    Brown  Fox     Jumps  Over    The  Lazy    Dog.    "; //Original string to manipulate
      string firstWordInString = "The"; //Expected result for use of firstWord with the original string
      int numberOfWords = 9;            //Number of words in the original string
      int resultInt;                    
      string fifthWordInString = "Jumps"; //Expected result for the fifth word in the original string
      string removedFourthFifthWords = "      The  Quick    Brown  Over    The  Lazy    Dog.    "; //Expected result for removal of the 4th and 5th words
      string removedFirstWord = "Quick    Brown  Over    The  Lazy    Dog.    "; //Expected result for the removal of the 1st word in removedFourthFifthWords
      string removedSixthWord = "Quick    Brown  Over    The  Lazy"; //Expected result of the removal of the sixth word in removedFirstWord
      string allWordsFromFourthOn = "The  Lazy"; //Expected result of the reporting of the words from the fourth to the end of removedSixthWord
      string resultString; //String to store resulting data.


      resultString = firstWord(originalString);
      failMesg = "Verify that firstWord returns the correct word in the string";
      testFramework.assert(resultString == firstWordInString, failMesg, __LINE__);
		

      testFramework.changeSourceMethod("numWords");
      resultInt = numWords(originalString);
      failMesg = "Verify that firstWord returns the correct word in the string";
      testFramework.assert(resultInt == numberOfWords, failMesg, __LINE__);
		

      testFramework.changeSourceMethod("word");
      resultString = word(originalString,4);
      failMesg = "Verify that word returns the correct word in the string (words are ordered starting with 0)";
      testFramework.assert(resultString == fifthWordInString, failMesg, __LINE__);

      testFramework.changeSourceMethod("removeWords");
      resultString = removeWords(originalString, 3, 2); //This changes the string passed to the method
      failMesg = "Verify that removeWords removes the correct set of words";
      testFramework.assert(resultString == removedFourthFifthWords, failMesg, __LINE__);

      testFramework.changeSourceMethod("stripFirstWord");
      resultString = stripFirstWord(originalString); //This changes the string passed to the method
      failMesg = "Verify that the first word is stripped from the string";
      testFramework.assert(originalString == removedFirstWord, failMesg, __LINE__);

      testFramework.changeSourceMethod("removeWords");
      resultString = removeWords(originalString,5); //This changes the string passed to the method
      failMesg = "Verify that removeWords removes the proper word from the string";
      testFramework.assert(resultString == removedSixthWord, failMesg, __LINE__);

      testFramework.changeSourceMethod("words");
      resultString = words(originalString,3);
      failMesg = "Verify that words returns all words from the correct position (4th in this case) onward";
      testFramework.assert(resultString == allWordsFromFourthOn, failMesg, __LINE__);

      return testFramework.countFails();
   }


/* =========================================================================================================================
   Tests for the regular expression methods

   Provided a string and a regular expression query, the isLike method will return 0 or 1 if a match is found.
   Matches will then provide the found result.
   ========================================================================================================================= */
   int regularExpressionsTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "isLike", __FILE__, __LINE__ );
      std::string failMesg;

      string originalString = "      The  Quick    Brown  Fox     Jumps  Over    The  Lazy    Dog.    "; //String to be searched
      int resultInt;                                                                                     //Storage for isLike result
      string testedRegularExpression1 = "Br.*Over.*";                                                    //A query which will be found
      string testedRegularExpression2 = "Br.Over.*";                                                     //A query which will not be found
      string resultString;                                                                               //Storage for matches method
      string correctResult = "Brown  Fox     Jumps  Over    The  Lazy    Dog.    ";                      //Expected string to be found


      resultInt = isLike(originalString, testedRegularExpression1);
      failMesg = "Verify that isLike returns 1 when the expression is found in the given string";
      testFramework.assert(resultInt == 1, failMesg, __LINE__);
		

      resultInt = isLike(originalString, testedRegularExpression2);
      failMesg = "Verify that isLike returns 0 when the expression is NOT found in the given string";
      testFramework.assert(resultInt == 0, failMesg, __LINE__);
		

      testFramework.changeSourceMethod("matches");
      resultString = matches(originalString, testedRegularExpression1);
      failMesg = "Verify that matches returns the string which matches the regular expression given";
      testFramework.assert(resultString == correctResult, failMesg, __LINE__);
		

      return testFramework.countFails();
   }

/* =========================================================================================================================
   Test for the prettyPrint method

   prettyPrint will take a string and split the information into formatted lines
   ========================================================================================================================= */
   int prettyPrintTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "prettyPrint", __FILE__, __LINE__ );
      std::string failMesg;
      string originalString("This is a very long sentence that will get cut up into lines with a maximum length of 40 characters, each line indented by a few spaces.");
      string prettyPrintThis = originalString;
      string prettyPrinted = "       This is a very long sentence that\n     will get cut up into lines with a\n     maximum length of 40 characters,\n     each line indented by a few spaces.\n";

      prettyPrint(prettyPrintThis, "\n", "    ", "      ", 40);
      failMesg = "Verify that prettyPrint works";
      testFramework.assert(prettyPrinted == prettyPrintThis, failMesg, __LINE__);

      return testFramework.countFails();
   }

/* =========================================================================================================================
   Test for the splitWithQuotes method

   splitWithQuotes will split a string into a std::vector<std::string> respecting single and double quoted strings
   ========================================================================================================================= */
   int splitWithQuotesTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "splitWithQuotes", __FILE__, __LINE__ );
      std::string failMesg;

      // no quotes
      std::string originalString("String with no quotes");
      std::vector<std::string> expectedResult;
      expectedResult.push_back("String");
      expectedResult.push_back("with");
      expectedResult.push_back("no");
      expectedResult.push_back("quotes");
      std::vector<std::string> observedResult = splitWithQuotes(originalString);

      failMesg = "splitWithQuotes fails with no quotes";
      testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

      // empty string
      originalString = "";
      expectedResult.clear();
      observedResult = splitWithQuotes(originalString);

      failMesg = "splitWithQuotes fails with empty string";
      testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

      // only whitespace
      originalString = " ";
      expectedResult.clear();
      expectedResult.push_back("");
      expectedResult.push_back("");
      observedResult = splitWithQuotes(originalString,' ',false,false);

      failMesg = "splitWithQuotes fails with just whitespace";
      testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

      // double quotes
      originalString = std::string("String with \"double quoted values\"");
      expectedResult.clear();
      expectedResult.push_back("String");
      expectedResult.push_back("with");
      expectedResult.push_back("double quoted values");
      observedResult = splitWithQuotes(originalString);

      failMesg = "splitWithQuotes fails with double quotes";
      testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

      // single quotes
      originalString= std::string("String with \'single quoted values\'");
      expectedResult.clear();
      expectedResult.push_back("String");
      expectedResult.push_back("with");
      expectedResult.push_back("single quoted values");
      observedResult = splitWithQuotes(originalString);

      failMesg = "splitWithQuotes fails with double quotes";
      testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

      // leading quote
      originalString= std::string("\"First words\" quoted");
      expectedResult.clear();
      expectedResult.push_back("First words");
      expectedResult.push_back("quoted");
      observedResult = splitWithQuotes(originalString);

      failMesg = "splitWithQuotes fails with leading quote";
      testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

      // back to back quotes
      originalString= std::string("\"Back to\" \"back quotes\"");
      expectedResult.clear();
      expectedResult.push_back("Back to");
      expectedResult.push_back("back quotes");
      observedResult = splitWithQuotes(originalString);

      failMesg = "splitWithQuotes failed with leading quote";
      testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

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

      failMesg = "splitWithQuotes fails with empty non-white fields";
      testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);


      return testFramework.countFails();
   }

   /* =========================================================================================================================
      Test for the splitWithDoubleQuotes method

      splitWithDoubleQuotes will split a string into a std::vector<std::string> respecting double quoted strings
      ========================================================================================================================= */
      int splitWithDoubleQuotesTest( void )
      {
         gpstk::TestUtil testFramework( "StringUtils", "splitWithDoubleQuotes", __FILE__, __LINE__ );
         std::string failMesg;

         // no quotes
         std::string originalString("String with no quotes");
         std::vector<std::string> expectedResult;
         expectedResult.push_back("String");
         expectedResult.push_back("with");
         expectedResult.push_back("no");
         expectedResult.push_back("quotes");
         std::vector<std::string> observedResult = splitWithDoubleQuotes(originalString);

         failMesg = "splitWithDoubleQuotes fails with no quotes";
         testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

         // empty string
         originalString = "";
         expectedResult.clear();
         observedResult = splitWithDoubleQuotes(originalString);

         failMesg = "splitWithDoubleQuotes fails with empty string";
         testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

         // only whitespace
         originalString = " ";
         expectedResult.clear();
         expectedResult.push_back("");
         expectedResult.push_back("");
         observedResult = splitWithDoubleQuotes(originalString,' ',false,false);

         failMesg = "splitWithDoubleQuotes fails with just whitespace";
         testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

         // double quotes
         originalString = std::string("String with \"double quoted values\"");
         expectedResult.clear();
         expectedResult.push_back("String");
         expectedResult.push_back("with");
         expectedResult.push_back("double quoted values");
         observedResult = splitWithDoubleQuotes(originalString);

         failMesg = "splitWithDoubleQuotes fails with double quotes";
         testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

         // single quotes
         originalString= std::string("String with \'single quoted values\'");
         expectedResult.clear();
         expectedResult.push_back("String");
         expectedResult.push_back("with");
         expectedResult.push_back("\'single");
         expectedResult.push_back("quoted");
         expectedResult.push_back("values\'");
         observedResult = splitWithDoubleQuotes(originalString);

         failMesg = "splitWithDoubleQuotes fails with single quotes";
         testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

         // leading quote
         originalString= std::string("\"First words\" quoted");
         expectedResult.clear();
         expectedResult.push_back("First words");
         expectedResult.push_back("quoted");
         observedResult = splitWithDoubleQuotes(originalString);

         failMesg = "splitWithDoubleQuotes fails with leading quote";
         testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

         // back to back quotes
         originalString= std::string("\"Back to\" \"back quotes\"");
         expectedResult.clear();
         expectedResult.push_back("Back to");
         expectedResult.push_back("back quotes");
         observedResult = splitWithDoubleQuotes(originalString);

         failMesg = "splitWithDoubleQuotes failed with leading quote";
         testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);

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

         failMesg = "splitWithDoubleQuotes fails with empty non-white fields";
         testFramework.assert(expectedResult == observedResult, failMesg, __LINE__);


         return testFramework.countFails();
      }

/* =========================================================================================================================
   Test for the hexDump methods

   This test is intended to verify that the unique ASCII characters 
   are all transformed from hex and output properly.

   The hexDataDump method is to display hex data in ASCII format. This output may be configured using
   the configHexDataDump method, or output without the comparison hex code using the printable method. 

   THIS TEST SHOULD BE REMADE TO BE CROSS PLATFORM
   ========================================================================================================================= */
   int hexToAsciiTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "hexDumpData", __FILE__, __LINE__ );
      std::string failMesg;


      stringstream hexDumpStream;
      stringstream correctHexDumpStream;
      stringstream configHexDumpStream;
      stringstream correctConfigHexDumpStream;
      stringstream correctPrintableMessage;
      stringstream printableMessageStream;

      char hexDumpArray[] = //Array of hex values to output
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
           0x33, 0x65, 0x30, 0x34, 0x30, 0x63, 0x66, (char)0x85 };

      //Correct format for hexDumpData, with Unix-Debain output of the 0x00, 0x10, 0x7F, and 0x85 hex values (first, second, and last values)
      //A NEW METHOD OF CREATING THIS OUTPUT FOR THOSE UNIQUE CHARACTERS SHOULD BE MADE. AS IT IS NOW, THIS TEST WILL FAIL FOR OTHER
      //OPERATING SYSTEMS OR COMPILERS.
      correctHexDumpStream << "0000: 00 10 30 33 30 31 30 35  3a 65 70 68 20 3a 30 3a    ..030105:eph :0:" << endl 
                           << "0010: 35 32 36 34 30 2e 33 33  33 34 30 32 37 37 37 37    52640.3334027777" << endl
                           << "0020: 37 37 37 38 41 20 52 61  6e 64 6f 6d 20 4d 65 73    7778A Random Mes" << endl
                           << "0030: 73 61 67 65 32 35 39 32  35 39 32 34 20 32 20 35    sage25925924 2 5" << endl
                           << "0040: 32 36 34 30 2e 33 33 38  38 33 36 33 34 32 35 39    2640.33883634259" << endl
                           << "0050: 32 35 39 33 20 7f 20 38  35 34 31 32 20 31 20 31    2593 . 85412 1 1" << endl
                           << "0060: 20 30 20 31 20 31 20 34  20 32 20 34 20 30 20 30     0 1 1 4 2 4 0 0" << endl
                           << "0070: 20 33 20 31 31 39 39 20  32 32 63 30 61 66 30 33     3 1199 22c0af03" << endl
                           << "0080: 20 31 37 37 30 32 39 65  34 20 61 66 34 30 30 31     177029e4 af4001" << endl
                           << "0090: 66 20 65 36 31 62 35 38  61 20 35 35 66 33 37 35    f e61b58a 55f375" << endl
                           << "00a0: 30 20 34 63 63 61 38 38  37 20 31 31 35 64 37 63    0 4cca887 115d7c" << endl
                           << "00b0: 66 63 20 33 61 64 32 30  34 38 61 20 63 65 61 20    fc 3ad2048a cea " << endl
                           << "00c0: 33 36 32 63 34 38 20 31  31 39 39 20 32 32 63 30    362c48 1199 22c0" << endl
                           << "00d0: 61 66 30 33 20 31 37 37  30 34 61 37 34 20 33 61    af03 17704a74 3a" << endl
                           << "00e0: 63 34 36 36 63 31 20 62  64 30 63 36 64 63 20 33    c466c1 bd0c6dc 3" << endl
                           << "00f0: 39 31 62 33 37 38 33 20  33 65 30 34 30 63 66 85    91b3783 3e040cf." << endl;

      //Correct format for the configured hexDumpData, with Unix-Debain output of the 0x00, 0x10, 0x7F, and 0x85 hex values (first, second, and last values)
      //A NEW METHOD OF CREATING THIS OUTPUT FOR THOSE UNIQUE CHARACTERS SHOULD BE MADE. AS IT IS NOW, THIS TEST WILL FAIL FOR OTHER
      //OPERATING SYSTEMS OR COMPILERS.
      correctConfigHexDumpStream << "      00000000:  0010 3033 3031 3035 3A65 7068 203A 303A '..030105:eph :0:'\n" 
                                 << "      00000010:  3532 3634 302E 3333 3334 3032 3737 3737 '52640.3334027777'\n" 
                                 << "      00000020:  3737 3738 4120 5261 6E64 6F6D 204D 6573 '7778A Random Mes'\n" 
                                 << "      00000030:  7361 6765 3235 3932 3539 3234 2032 2035 'sage25925924 2 5'\n" 
                                 << "      00000040:  3236 3430 2E33 3338 3833 3633 3432 3539 '2640.33883634259'\n" 
                                 << "      00000050:  3235 3933 207F 2038 3534 3132 2031 2031 '2593 . 85412 1 1'\n" 
                                 << "      00000060:  2030 2031 2031 2034 2032 2034 2030 2030 ' 0 1 1 4 2 4 0 0'\n" 
                                 << "      00000070:  2033 2031 3139 3920 3232 6330 6166 3033 ' 3 1199 22c0af03'\n" 
                                 << "      00000080:  2031 3737 3032 3965 3420 6166 3430 3031 ' 177029e4 af4001'\n" 
                                 << "      00000090:  6620 6536 3162 3538 6120 3535 6633 3735 'f e61b58a 55f375'\n" 
                                 << "      000000A0:  3020 3463 6361 3838 3720 3131 3564 3763 '0 4cca887 115d7c'\n" 
                                 << "      000000B0:  6663 2033 6164 3230 3438 6120 6365 6120 'fc 3ad2048a cea '\n" 
                                 << "      000000C0:  3336 3263 3438 2031 3139 3920 3232 6330 '362c48 1199 22c0'\n"
                                 << "      000000D0:  6166 3033 2031 3737 3034 6137 3420 3361 'af03 17704a74 3a'\n" 
                                 << "      000000E0:  6334 3636 6331 2062 6430 6336 6463 2033 'c466c1 bd0c6dc 3'\n"
                                 << "      000000F0:  3931 6233 3738 3320 3365 3034 3063 6685 '91b3783 3e040cf.'\n";

      //Build the hexDumpString and output it to stringstream
      string hexDumpString(hexDumpArray, sizeof(hexDumpArray));

      hexDumpData(hexDumpStream, hexDumpString);

      failMesg = "Verify that hexDumpData works";
      testFramework.assert(hexDumpStream.str() == correctHexDumpStream.str(), failMesg, __LINE__);

      //configure the hexDumpData, dump it to output stringstream
      HexDumpDataConfig hdcfg(true, true, true, 8, 2, 2, 1, 0, 0, 16, true,'\'', 1);
      hexDumpData(configHexDumpStream, hexDumpString, 6, hdcfg);
      testFramework.changeSourceMethod("HexDumpDataConfig");

      failMesg = "Verify that configuring the hexDumpData works";
      testFramework.assert(configHexDumpStream.str() == correctConfigHexDumpStream.str(), failMesg, __LINE__);

      //Correct format for the printable, with Unix-Debain output of the 0x00, 0x10, 0x7F, and 0x85 hex values (first, second, and last values)
      //A NEW METHOD OF CREATING THIS OUTPUT FOR THOSE UNIQUE CHARACTERS SHOULD BE MADE. AS IT IS NOW, THIS TEST WILL FAIL FOR OTHER
      //OPERATING SYSTEMS OR COMPILERS.
      correctPrintableMessage << "^@^P030105:eph :0:52640.33340277777778A Random Message25925924"
                              << " 2 52640.338836342592593 ^? 85412 1 1 0 1 1 4 2 4 0 0 3 1199 22c0af03"
                              << " 177029e4 af4001f e61b58a 55f3750 4cca887 115d7cfc 3ad2048a cea"
                              << " 362c48 1199 22c0af03 17704a74 3ac466c1 bd0c6dc 391b3783 3e040cf<85>";

      printableMessageStream << printable(hexDumpString) ;
      testFramework.changeSourceMethod("printable");
      failMesg = "Verify that prettyPrint works";
      testFramework.assert(printableMessageStream.str() == correctPrintableMessage.str(), failMesg, __LINE__);

      return testFramework.countFails();
   }

/* =========================================================================================================================
   Test for the hexDump methods

   This test is intended to verify that the output formats are correct.
   There are no characters that could be unique to the system configuration in these lists.
   ========================================================================================================================= */
   int hexDumpDataTest( void )
   {
      gpstk::TestUtil testFramework( "StringUtils", "hexDumpData", __FILE__, __LINE__ );
      std::string failMesg;


      stringstream hexDumpStream;
      stringstream correctHexDumpStream;
      stringstream configHexDumpStream;
      stringstream correctConfigHexDumpStream;
      stringstream correctPrintableMessage;
      stringstream printableMessageStream;

      char hexDumpArray[] = //Array of hex values to output
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
           0x33, 0x65, 0x30, 0x34, 0x30, 0x63, 0x66, 0x65 };

      //Correct format for hexDumpData
      correctHexDumpStream << "0000: 61 66 30 33 30 31 30 35  3a 65 70 68 20 3a 30 3a    af030105:eph :0:" << endl 
                           << "0010: 35 32 36 34 30 2e 33 33  33 34 30 32 37 37 37 37    52640.3334027777" << endl
                           << "0020: 37 37 37 38 41 20 52 61  6e 64 6f 6d 20 4d 65 73    7778A Random Mes" << endl
                           << "0030: 73 61 67 65 32 35 39 32  35 39 32 34 20 32 20 35    sage25925924 2 5" << endl
                           << "0040: 32 36 34 30 2e 33 33 38  38 33 36 33 34 32 35 39    2640.33883634259" << endl
                           << "0050: 32 35 39 33 20 32 20 38  35 34 31 32 20 31 20 31    2593 2 85412 1 1" << endl
                           << "0060: 20 30 20 31 20 31 20 34  20 32 20 34 20 30 20 30     0 1 1 4 2 4 0 0" << endl
                           << "0070: 20 33 20 31 31 39 39 20  32 32 63 30 61 66 30 33     3 1199 22c0af03" << endl
                           << "0080: 20 31 37 37 30 32 39 65  34 20 61 66 34 30 30 31     177029e4 af4001" << endl
                           << "0090: 66 20 65 36 31 62 35 38  61 20 35 35 66 33 37 35    f e61b58a 55f375" << endl
                           << "00a0: 30 20 34 63 63 61 38 38  37 20 31 31 35 64 37 63    0 4cca887 115d7c" << endl
                           << "00b0: 66 63 20 33 61 64 32 30  34 38 61 20 63 65 61 20    fc 3ad2048a cea " << endl
                           << "00c0: 33 36 32 63 34 38 20 31  31 39 39 20 32 32 63 30    362c48 1199 22c0" << endl
                           << "00d0: 61 66 30 33 20 31 37 37  30 34 61 37 34 20 33 61    af03 17704a74 3a" << endl
                           << "00e0: 63 34 36 36 63 31 20 62  64 30 63 36 64 63 20 33    c466c1 bd0c6dc 3" << endl
                           << "00f0: 39 31 62 33 37 38 33 20  33 65 30 34 30 63 66 65    91b3783 3e040cfe" << endl;

      //Correct format for the configured hexDumpData
      correctConfigHexDumpStream << "      00000000:  6166 3033 3031 3035 3A65 7068 203A 303A 'af030105:eph :0:'\n" 
                                 << "      00000010:  3532 3634 302E 3333 3334 3032 3737 3737 '52640.3334027777'\n" 
                                 << "      00000020:  3737 3738 4120 5261 6E64 6F6D 204D 6573 '7778A Random Mes'\n" 
                                 << "      00000030:  7361 6765 3235 3932 3539 3234 2032 2035 'sage25925924 2 5'\n" 
                                 << "      00000040:  3236 3430 2E33 3338 3833 3633 3432 3539 '2640.33883634259'\n" 
                                 << "      00000050:  3235 3933 2032 2038 3534 3132 2031 2031 '2593 2 85412 1 1'\n" 
                                 << "      00000060:  2030 2031 2031 2034 2032 2034 2030 2030 ' 0 1 1 4 2 4 0 0'\n" 
                                 << "      00000070:  2033 2031 3139 3920 3232 6330 6166 3033 ' 3 1199 22c0af03'\n" 
                                 << "      00000080:  2031 3737 3032 3965 3420 6166 3430 3031 ' 177029e4 af4001'\n" 
                                 << "      00000090:  6620 6536 3162 3538 6120 3535 6633 3735 'f e61b58a 55f375'\n" 
                                 << "      000000A0:  3020 3463 6361 3838 3720 3131 3564 3763 '0 4cca887 115d7c'\n" 
                                 << "      000000B0:  6663 2033 6164 3230 3438 6120 6365 6120 'fc 3ad2048a cea '\n" 
                                 << "      000000C0:  3336 3263 3438 2031 3139 3920 3232 6330 '362c48 1199 22c0'\n"
                                 << "      000000D0:  6166 3033 2031 3737 3034 6137 3420 3361 'af03 17704a74 3a'\n" 
                                 << "      000000E0:  6334 3636 6331 2062 6430 6336 6463 2033 'c466c1 bd0c6dc 3'\n"
                                 << "      000000F0:  3931 6233 3738 3320 3365 3034 3063 6665 '91b3783 3e040cfe'\n";

      //Build the hexDumpString and output it to stringstream
      string hexDumpString(hexDumpArray, sizeof(hexDumpArray));
      hexDumpData(hexDumpStream, hexDumpString);

      failMesg = "Verify that hexDumpData works";
      testFramework.assert(hexDumpStream.str() == correctHexDumpStream.str(), failMesg, __LINE__);

      //configure the hexDumpData, dump it to output stringstream
      HexDumpDataConfig hdcfg(true, true, true, 8, 2, 2, 1, 0, 0, 16, true,'\'', 1);
      hexDumpData(configHexDumpStream, hexDumpString, 6, hdcfg);
      testFramework.changeSourceMethod("HexDumpDataConfig");

      failMesg = "Verify that configuring the hexDumpData works";
      testFramework.assert(configHexDumpStream.str() == correctConfigHexDumpStream.str(), failMesg, __LINE__);

      //Correct format for the printable
      correctPrintableMessage << "af030105:eph :0:52640.33340277777778A Random Message25925924"
                              << " 2 52640.338836342592593 2 85412 1 1 0 1 1 4 2 4 0 0 3 1199 22c0af03"
                              << " 177029e4 af4001f e61b58a 55f3750 4cca887 115d7cfc 3ad2048a cea"
                              << " 362c48 1199 22c0af03 17704a74 3ac466c1 bd0c6dc 391b3783 3e040cfe";

      printableMessageStream << printable(hexDumpString) ;
      testFramework.changeSourceMethod("printable");

      failMesg = "Verify that prettyPrint works";
      testFramework.assert(printableMessageStream.str() == correctPrintableMessage.str(), failMesg, __LINE__);

      return testFramework.countFails();
   }
};

int main() //Main function to initialize and run all tests above
{
   int check, errorCounter = 0;
   StringUtils_T testClass;

   check = testClass.justificationTest();
   errorCounter += check;

   check = testClass.stripLeadingTest();
   errorCounter += check;

   check = testClass.stripTrailingTest();
   errorCounter += check;

   check = testClass.stripTest();
   errorCounter += check;

   check = testClass.stringToNumberTest();
   errorCounter += check;

   check = testClass.numberToStringTest();
   errorCounter += check;

   check = testClass.hexConversionTest();
   errorCounter += check;

   check = testClass.stringReplaceTest();
   errorCounter += check;

   check = testClass.wordTest();
   errorCounter += check;

   check = testClass.regularExpressionsTest();
   errorCounter += check;

   check = testClass.prettyPrintTest();
   errorCounter += check;

   check = testClass.splitWithQuotesTest();
   errorCounter += check;

   check = testClass.splitWithDoubleQuotesTest();
   errorCounter += check;

   check = testClass.hexDumpDataTest();
   errorCounter += check;

   check = testClass.hexToAsciiTest();
   errorCounter += check;

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

   return errorCounter; //Return the total number of errors
}
