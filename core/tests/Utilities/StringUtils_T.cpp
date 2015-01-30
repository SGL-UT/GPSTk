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
			TestUtil testFramework( "StringUtils", "Justify", __FILE__, __LINE__ );
			testFramework.init();

			string baseString= "1234567890";
			string resultString;
			string manipulatedString;
			char fillerChar = '-';

			manipulatedString = baseString;
			resultString = string(6, fillerChar) + baseString;
			rightJustify(manipulatedString, manipulatedString.length() + 6, fillerChar);
//--------------StringUtils_justificationTest_1 - Verify that rightJustify works by appending 6 '-'s to the left
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			manipulatedString = baseString;
			resultString = baseString + string(6, fillerChar);
			leftJustify(manipulatedString, manipulatedString.length()+6, fillerChar);
//--------------StringUtils_justificationTest_2 - Verify that leftJustify works by appending 6 '-'s to the right
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();
			  
			manipulatedString = baseString;
			resultString = string(3, fillerChar) + baseString + string(3, fillerChar);
			center(manipulatedString, manipulatedString.length()+6, fillerChar);
//--------------StringUtils_justificationTest_3 - Verify that leftJustify works by appending 3 '-'s to both sides
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			return testFramework.countFails();
		}



/* =========================================================================================================================
	Tests for the stripLeading methods
	Given a baseString and a filler character and string, the tests will strip the 
	baseString of the request string or character from the front of the string.
========================================================================================================================= */
		int stripLeadingTest( void )
		{
			TestUtil testFramework( "StringUtils", "stripLeading", __FILE__, __LINE__ );
			testFramework.init();

			string baseString= "---1234567890---";
			string resultString;
			string manipulatedString;
			char charToBeRemoved = '-';
			string stringToBeRemoved = "--";

			manipulatedString = baseString;
			resultString = baseString.substr(1,string::npos);
			manipulatedString = stripLeading(manipulatedString, charToBeRemoved, 1);
//--------------StringUtils_stripLeadingTest_1 - Verify that stripLeading works by removing 1 '-' from the beginning of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			manipulatedString = baseString;
			resultString = baseString.substr(2,string::npos);
			manipulatedString = stripLeading(manipulatedString, stringToBeRemoved, 1);
//--------------StringUtils_stripLeadingTest_2 -Verify that stripLeading works by removing 1 '--' from the beginning of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			manipulatedString = baseString;
			resultString = baseString.substr(3,string::npos);
			manipulatedString = stripLeading(manipulatedString, charToBeRemoved);
//--------------StringUtils_stripLeadingTest_3 - Verify that stripLeading works by removing all '-' from the beginning of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			manipulatedString = baseString;
			resultString = baseString.substr(2,string::npos);
			manipulatedString = stripLeading(manipulatedString, stringToBeRemoved);
//--------------StringUtils_stripLeadingTest_4 - Verify that stripLeading works by removing all '--' from the beginning of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();
			
			return testFramework.countFails();
		}


/* =========================================================================================================================
	Tests for the stripTrailing methods
	Given a baseString and a filler character and string, the tests will strip the 
	baseString of the request string or character from the end of the string.
========================================================================================================================= */
		int stripTrailingTest( void )
		{
			TestUtil testFramework( "StringUtils", "stripTrailing", __FILE__, __LINE__ );
			testFramework.init();

			string baseString= "---1234567890---";
			string resultString;
			string manipulatedString;
			char charToBeRemoved = '-';
			string stringToBeRemoved = "--";

			manipulatedString = baseString;
			resultString = baseString.substr(0, baseString.length()-1);
			manipulatedString = stripTrailing(manipulatedString, charToBeRemoved, 1);
//--------------StringUtils_stripTrailingTest_1 - Verify that stripTrailing works by removing 1 '-' from the end of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			manipulatedString = baseString;
			resultString = baseString.substr(0, baseString.length()-2);
			manipulatedString = stripTrailing(manipulatedString, stringToBeRemoved, 1);
//--------------StringUtils_stripTrailingTest_2 - Verify that stripTrailing works by removing 1 '--' from the end of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			manipulatedString = baseString;
			resultString = baseString.substr(0, baseString.length()-3);
			manipulatedString = stripTrailing(manipulatedString, charToBeRemoved);
//--------------StringUtils_stripTrailingTest_3 - Verify that stripTrailing works by removing all '-' from the end of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			manipulatedString = baseString;
			resultString = baseString.substr(0, baseString.length()-2);
			manipulatedString = stripTrailing(manipulatedString, stringToBeRemoved);
//--------------StringUtils_stripTrailingTest_4 - Verify that stripTrailing works by removing all '--' from the end of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();
			
			return testFramework.countFails();
		}


/* =========================================================================================================================
	Tests for the strip method
	Given a baseString and a filler character and string, the tests will strip the 
	baseString of the request string or character from both ends of the string.
========================================================================================================================= */
		int stripTest( void )
		{
			TestUtil testFramework( "StringUtils", "strip", __FILE__, __LINE__ );
			testFramework.init();

			string baseString= "---1234567890---";
			string resultString;
			string manipulatedString;
			char charToBeRemoved = '-';
			string stringToBeRemoved = "--";

			manipulatedString = baseString;
			resultString = baseString.substr(1,baseString.length()-2);
			manipulatedString = strip(manipulatedString, charToBeRemoved, 1);
//--------------StringUtils_stripTest_1 - Verify that strip works by removing 1 '-' from both ends of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			manipulatedString = baseString;
			resultString = baseString.substr(2,baseString.length()-4);
			manipulatedString = strip(manipulatedString, stringToBeRemoved, 1);
//--------------StringUtils_stripTest_2 - Verify that strip works by removing 1 '--' from both ends of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			manipulatedString = baseString;
			resultString = baseString.substr(3, baseString.length()-6);
			manipulatedString = strip(manipulatedString, charToBeRemoved);
//--------------StringUtils_stripTest_3 - Verify that strip works by removing all '-' from both ends of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();

			manipulatedString = baseString;
			resultString = baseString.substr(2, baseString.length()-4);
			manipulatedString = strip(manipulatedString, stringToBeRemoved);
//--------------StringUtils_stripTest_4 - Verify that strip works by removing all '--' from both ends of the string
			testFramework.assert(manipulatedString == resultString);
			testFramework.next();
			
			return testFramework.countFails();
		}


/* =========================================================================================================================
	Tests for the translate method
	Given a baseString and a translation strings, the baseString will be "translated"
        using those translation strings.
========================================================================================================================= */
		int translateTest( void )
		{
			TestUtil testFramework( "StringUtils", "strip", __FILE__, __LINE__ );
			testFramework.init();

			string baseString = "Hello World! I am the Translation test String!";
			string manipulatedString;
			string translateFrom1 = "aeoWTS!";
			string translateTo1 = "@30M7Q1";
			string expectedResult1 = "H3ll0 M0rld1 I @m th3 7r@nsl@ti0n t3st Qtring1";
			string translateFrom2 = "aeoWTS!";
			string translateTo2 = "@30M7";
			string expectedResult2 = "H3ll0 M0rld  I @m th3 7r@nsl@ti0n t3st  tring ";
			string translateFrom3 = "aeoWTS!";
			string translateTo3 = "aeoWTS!";
			string expectedResult3 = "Hello World! I am the Translation test String!";

			manipulatedString = translate(baseString, translateFrom1, translateTo1);
//--------------StringUtils_translateTest_1 - Verify that translate works by changing multiple characters
			testFramework.assert(manipulatedString == expectedResult1);
			testFramework.next();

			manipulatedString = translate(baseString, translateFrom2, translateTo2);
//--------------StringUtils_translateTest_2 - Verify that translate replaces unmatched characters with blanks
			testFramework.assert(manipulatedString == expectedResult2);
			testFramework.next();

			manipulatedString = translate(baseString, translateFrom3, translateTo3);
//--------------StringUtils_translateTest_2 - Verify that translate won't change things when supplied the same set of characters
			testFramework.assert(manipulatedString == expectedResult3);
			testFramework.next();

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Tests for the string to number methods
	Given a baseString and a target number type, the methods should 
	generate the appropriate values.
========================================================================================================================= */
		int stringToNumberTest( void )
		{
			TestUtil testFramework( "StringUtils", "asInt", __FILE__, __LINE__ );
			testFramework.init();

			string baseString = "12345.67890";
			int resultingInt, comparisonInt = 12345;
			double resultingDouble, comparisonDouble = 12345.67890;
			float resultingFloat, comparisonFloat = 12345.67890;
			long double resultingLongDouble, comparisonLongDouble = 12345.67890L;

			resultingInt = asInt(baseString);
//--------------StringUtils_stringToNumberTest_1 - Verify that asInt works as intended
			testFramework.assert(resultingInt == comparisonInt);
			testFramework.next();

			testFramework.changeSourceMethod("asDouble");
			resultingDouble = asDouble(baseString);
//--------------StringUtils_stringToNumberTest_2 - Verify that asDouble works as intended
			testFramework.assert(resultingDouble == comparisonDouble);
			testFramework.next();

			testFramework.changeSourceMethod("asFloat");
			resultingFloat = asFloat(baseString);
//--------------StringUtils_stringToNumberTest_3 - Verify that asFloat works as intended
			testFramework.assert(resultingFloat == comparisonFloat);
			testFramework.next();

			testFramework.changeSourceMethod("asLongDouble");
			resultingLongDouble = asLongDouble(baseString);
//--------------StringUtils_stringToNumberTest_4 - Verify that asLongDouble works as intended
			testFramework.assert(resultingLongDouble == comparisonLongDouble);
			testFramework.next();

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
			TestUtil testFramework( "StringUtils", "asString", __FILE__, __LINE__ );
			testFramework.init();

			string convertedString;
			int sampleInt = 1234567890;
			double sampleDouble = 12345.67890;
			float sampleFloat = 12345.7;
			long double sampleLongDouble = 12345.67890L;

			
			convertedString = asString(sampleInt);
//--------------StringUtils_numberToStringTest_1 - Verify that asString converts ints
			testFramework.assert(asInt(convertedString) == sampleInt);
			testFramework.next();

			convertedString = asString(sampleDouble);
//--------------StringUtils_numberToStringTest_2 - Verify that asString converts doubles
			testFramework.assert(asDouble(convertedString) == sampleDouble);
			testFramework.next();

			convertedString = asString(sampleFloat);
//--------------StringUtils_numberToStringTest_3 - Verify that asString converts floats
			testFramework.assert(asFloat(convertedString) == sampleFloat);
			testFramework.next();

			convertedString = asString(sampleLongDouble);
//--------------StringUtils_numberToStringTest_4 - Verify that asString converts long doubles
			testFramework.assert(asLongDouble(convertedString) == sampleLongDouble);
			testFramework.next();

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Tests for the hex converters
========================================================================================================================= */
		int hexConversionTest( void )
		{
			TestUtil testFramework( "StringUtils", "x2d", __FILE__, __LINE__ );
			testFramework.init();

			string initialHexString = "0x100 foo";
			string correctConvertedHexString = "256 foo";
			string initialDecimalString ="100 bar";
			string correctConvertedDecimalString = "64 bar";
			string manipulatedResult;

			manipulatedResult = x2d(initialHexString);
//--------------StringUtils_hexConversionTest_1 - Verify that x2d works
			testFramework.assert(initialHexString == correctConvertedHexString);
			testFramework.next();

			testFramework.changeSourceMethod("d2x");
			manipulatedResult = d2x(initialDecimalString);
//--------------StringUtils_hexConversionTest_2 - Verify that d2x works
			testFramework.assert(initialDecimalString == correctConvertedDecimalString);
			testFramework.next();

			return testFramework.countFails();
		}

/* =========================================================================================================================
	Tests for the string replace methods
========================================================================================================================= */
		int stringReplaceTest( void )
		{
			TestUtil testFramework( "StringUtils", "replaceAll", __FILE__, __LINE__ );
			testFramework.init();

			string baseString = "010101000111010";
			string replaced1s = "0-0-0-000---0-0";
			string replaced1withABC = "0ABC0ABC0ABC000ABCABCABC0ABC0";
			string replaced1with123 = "01230123012300012312312301230";
			string manipulatedString;

			manipulatedString = baseString;
			manipulatedString = replaceAll(manipulatedString, string("1"), string("-"));
//--------------StringUtils_stringReplaceTest_1 - Verify that replaceAll works with a character swap
			testFramework.assert(manipulatedString == replaced1s);
			testFramework.next();

			manipulatedString = baseString;
			manipulatedString = replaceAll(manipulatedString, string("1"), string("ABC"));
//--------------StringUtils_stringReplaceTest_2 - Verify that replaceAll works with a character swap
			testFramework.assert(manipulatedString == replaced1withABC);
			testFramework.next();

			manipulatedString = baseString;
			manipulatedString = replaceAll(manipulatedString, string("1"), string("123"));
//--------------StringUtils_stringReplaceTest_3 - Verify that replaceAll works with a character swap
			testFramework.assert(manipulatedString == replaced1with123);
			testFramework.next();

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Tests for the word methods
========================================================================================================================= */
		int wordTest( void )
		{
			TestUtil testFramework( "StringUtils", "firstWord", __FILE__, __LINE__ );
			testFramework.init();


			string originalString = "      The  Quick    Brown  Fox     Jumps  Over    The  Lazy    Dog.    ";
			string firstWordInString = "The";
			int numberOfWords = 9;
			int resultInt;
			string fifthWordInString = "Jumps";
			string removedFourthFifthWords = "      The  Quick    Brown  Over    The  Lazy    Dog.    ";
			string removedFirstWord = "Quick    Brown  Over    The  Lazy    Dog.    ";
			string removedSixthWord = "Quick    Brown  Over    The  Lazy"; 
			string allWordsFromFourthOn = "The  Lazy";
			string resultString;


			resultString = firstWord(originalString);
//--------------StringUtils_wordTest_1 - Verify that firstWord returns the correct word in the string
			testFramework.assert(resultString == firstWordInString);
			testFramework.next();	

			testFramework.changeSourceMethod("numWords");
			resultInt = numWords(originalString);
//--------------StringUtils_wordTest_2 - Verify that firstWord returns the correct word in the string
			testFramework.assert(resultInt == numberOfWords);
			testFramework.next();		

			testFramework.changeSourceMethod("word");
			resultString = word(originalString,4);
//--------------StringUtils_wordTest_3 - Verify that word returns the correct word in the string (words are ordered starting with 0)
			testFramework.assert(resultString == fifthWordInString);
			testFramework.next();

			testFramework.changeSourceMethod("removeWords");
			resultString = removeWords(originalString, 3, 2); //This changes the string passed to the method
//--------------StringUtils_wordTest_4 - Verify that removeWords removes the correct set of words
			testFramework.assert(resultString == removedFourthFifthWords);
			testFramework.next();

			testFramework.changeSourceMethod("stripFirstWord");
			resultString = stripFirstWord(originalString); //This changes the string passed to the method
//--------------StringUtils_wordTest_5 - Verify that the first word is stripped from the string
			testFramework.assert(originalString == removedFirstWord);
			testFramework.next();

			testFramework.changeSourceMethod("removeWords");
			resultString = removeWords(originalString,5); //This changes the string passed to the method
//--------------StringUtils_wordTest_6 - Verify that removeWords removes the proper word from the string
			testFramework.assert(resultString == removedSixthWord);
			testFramework.next();

			testFramework.changeSourceMethod("words");
			resultString = words(originalString,3);
//--------------StringUtils_wordTest_7 - Verify that words returns all words from the correct position (4th in this case) onward
			testFramework.assert(resultString == allWordsFromFourthOn);
			testFramework.next();

			return testFramework.countFails();
		}


/* =========================================================================================================================
	Tests for the regular expression methods
========================================================================================================================= */
		int regularExpressionsTest( void )
		{
			TestUtil testFramework( "StringUtils", "isLike", __FILE__, __LINE__ );
			testFramework.init();

			string originalString = "      The  Quick    Brown  Fox     Jumps  Over    The  Lazy    Dog.    ";
			int resultInt;
			string testedRegularExpression1 = "Br.*Over.*";
			string testedRegularExpression2 = "Br.Over.*";
			string resultString;
			string correctResult = "Brown  Fox     Jumps  Over    The  Lazy    Dog.    ";


			resultInt = isLike(originalString, testedRegularExpression1);
//--------------StringUtils_regularExpressionsTest_1 - Verify that isLike returns 1 when the expression is found in the given string
			testFramework.assert(resultInt == 1);
			testFramework.next();	

			resultInt = isLike(originalString, testedRegularExpression2);
//--------------StringUtils_regularExpressionsTest_2 - Verify that isLike returns 0 when the expression is NOT found in the given string
			testFramework.assert(resultInt == 0);
			testFramework.next();	

			testFramework.changeSourceMethod("matches");
			resultString = matches(originalString, testedRegularExpression1);
//--------------StringUtils_regularExpressionsTest_3 - Verify that matches returns the string which matches the regular expression given
			testFramework.assert(resultString == correctResult);
			testFramework.next();	

			return testFramework.countFails();
		}

/* =========================================================================================================================
	Test for the prettyPrint method
========================================================================================================================= */
		int prettyPrintTest( void )
		{
			TestUtil testFramework( "StringUtils", "prettyPrint", __FILE__, __LINE__ );
			testFramework.init();
			string originalString("This is a very long sentence that will get cut up into lines with a maximum length of 40 characters, each line indented by a few spaces.");
			string prettyPrintThis = originalString;
			string prettyPrinted = "       This is a very long sentence that\n     will get cut up into lines with a\n     maximum length of 40 characters,\n     each line indented by a few spaces.\n";

			prettyPrint(prettyPrintThis, "\n", "    ", "      ", 40);
//--------------StringUtils_prettyPrintTest_1 - Verify that prettyPrint works
			testFramework.assert(prettyPrinted == prettyPrintThis);
			testFramework.next();

			return testFramework.countFails();
		}



/* =========================================================================================================================
	Test for the hexDump methods
========================================================================================================================= */
		int hexDumpTest( void )
		{
			TestUtil testFramework( "StringUtils", "hexDumpData", __FILE__, __LINE__ );
			testFramework.init();


			stringstream hexDumpStream;
			stringstream correctHexDumpStream;
			stringstream configHexDumpStream;
			stringstream correctConfigHexDumpStream;
			stringstream correctPrintableMessage;
			stringstream printableMessageStream;

			char hexDumpArray[] =
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

			string hexDumpString(hexDumpArray, sizeof(hexDumpArray));
			hexDumpData(hexDumpStream, hexDumpString);

//--------------StringUtils_hexDumpTest_1 - Verify that prettyPrint works
			testFramework.assert(hexDumpStream.str() == correctHexDumpStream.str());
			testFramework.next();


      			HexDumpDataConfig hdcfg(true, true, true, 8, 2, 2, 1, 0, 0, 16, true,'\'', 1);
			hexDumpData(configHexDumpStream, hexDumpString, 6, hdcfg);
			testFramework.changeSourceMethod("HexDumpDataConfig");
//--------------StringUtils_hexDumpTest_2 - Verify that prettyPrint works
			testFramework.assert(configHexDumpStream.str() == correctConfigHexDumpStream.str());
			testFramework.next();


			correctPrintableMessage << "af030105:eph :0:52640.33340277777778A Random Message25925924"
						<< " 2 52640.338836342592593 2 85412 1 1 0 1 1 4 2 4 0 0 3 1199 22c0af03"
						<< " 177029e4 af4001f e61b58a 55f3750 4cca887 115d7cfc 3ad2048a cea"
						<< " 362c48 1199 22c0af03 17704a74 3ac466c1 bd0c6dc 391b3783 3e040cfe" << endl;
			printableMessageStream << printable(hexDumpString) << endl;
			testFramework.changeSourceMethod("printable");
//--------------StringUtils_hexDumpTest_2 - Verify that prettyPrint works
			testFramework.assert(printableMessageStream.str() == correctPrintableMessage.str());
			testFramework.next();

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

	check = testClass.hexDumpTest();
	errorCounter += check;

	std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

	return errorCounter; //Return the total number of errors
}
