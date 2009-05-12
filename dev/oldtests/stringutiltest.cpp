#pragma ident "$Id$"

//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 2.1 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Copyright 2008, The University of Texas at Austin
//
//============================================================================

#include <iostream>
#include <string>
#include "StringUtils.hpp"

using namespace gpstk::StringUtils;
using namespace std;

/**
 * @file stringutiltest.cpp
 * A test of the gpslib::StringUtils functions
 *
 */

// must be big enough to show hex digits in index
char hexDumpMess[] =
{ 0x00, 0x10, 0x30, 0x33, 0x30, 0x31, 0x30, 0x35,
  0x3A, 0x65, 0x70, 0x68, 0x20, 0x3A, 0x30, 0x3A,
  0x35, 0x32, 0x36, 0x34, 0x30, 0x2E, 0x33, 0x33,
  0x33, 0x34, 0x30, 0x32, 0x37, 0x37, 0x37, 0x37,
  0x37, 0x37, 0x37, 0x38, 0x42, 0x72, 0x69, 0x61,
  0x6e, 0x20, 0x69, 0x73, 0x20, 0x61, 0x20, 0x68,
  0x6f, 0x73, 0x65, 0x72, 0x32, 0x35, 0x39, 0x32,
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
  0x33, 0x65, 0x30, 0x34, 0x30, 0x63, 0x66, 0x85 };

/// This returns 0 if all tests pass.
int main()
{
   try
   {
      int rc = 0;
      string result, ref;
      string a = "1234567890", b = "456";
      string aa = "---1234567890---";
      char c = '-';
      string d = "--";
      string hexDumpString(hexDumpMess, sizeof(hexDumpMess));

         // Someone may want to do this to a string stream and do some
         // comparisons and what-not... for now, I'm only testing to
         // make sure it doesn't crash...
      cout << "Standard hex dump:" << endl;
      hexDumpData(cout, hexDumpString);
      cout << endl << "Alternate hex dump:" << endl;
      HexDumpDataConfig hdcfg(true, true, true, 8, 2, 2, 1, 0, 0, 16, true,
                              '\'', 1);
      hexDumpData(cout, hexDumpString, 6, hdcfg);
      
      cout << endl << "Above using printable():" << endl;
      cout << printable(hexDumpString) << endl;


         //#############################
         // Justification tests
  
      cout << "Original: " << a << endl;
      ref = a;
      result = string(3,c) + a;
      a = rightJustify(a, a.length() + 3, c);
      if(a != result)
      {
         cout  << "rightJustify failed: " << a << endl 
               << "Should be: " << result << endl;
      }
  
      a = ref;
      result = a + string(3,c);
      a = leftJustify(a, a.length()+3, c);
      if(a != result)
      {
         cout << "leftJustify failed: " << a << endl 
              << "Should be: " << result << endl;
      }
  
      a = ref;
      result = string(3,c) + a + string(3,c);
      a = center(a, a.length()+6, c);
      if(a != result)
      {
         cout << "center failed: " << a << endl
              << "Should be: " << result << endl;
      }

      a = ref;

         //#############################
         // Strip Tests

         //######## Leading
      ref = aa;
      result = aa.substr(1,string::npos);
      aa = stripLeading(aa, c, 1);
      if(aa != result)
      {
         cout << "stripLeading(string, char, 1) failed: " << aa << endl
              << "Should be: " << result << endl;
      }

      aa = ref;
      result = aa.substr(2,string::npos);
      aa = stripLeading(aa, d, 1);
      if(aa != result)
      {
         cout << "stripLeading(string, char *, 1) failed: " << aa  << endl
              << "Should be: " << result << endl;
      }

      aa = ref;
      result = aa.substr(3,string::npos);
      aa = stripLeading(aa, c);
      if(aa != result)
      {
         cout << "stripLeading(string, char) failed: " << aa << endl
              << "Should be: " << result << endl;
      }

      aa = ref;
      result = aa.substr(2,string::npos);
      aa = stripLeading(aa, d);
      if(aa != result)
      {
         cout << "stripLeading(string, char *) failed: " << aa  << endl
              << "Should be: " << result << endl;
      }

         //######## Trailing
      aa = ref;
      result = aa.substr(0, aa.length()-1);
      aa = stripTrailing(aa, c, 1);
      if(aa != result)
      {
         cout << "stripTrailing(string, char, 1) failed: " << aa  << endl
              << "Should be: " << result << endl;
      }

      aa = ref;
      result = aa.substr(0, aa.length()-2);
      aa = stripTrailing(aa, d, 1);
      if(aa != result)
      {
         cout << "stripTrailing(string, char *, 1) failed: " << aa  << endl
              << "Should be: " << result << endl;
      }

      aa = ref;
      result = aa.substr(0, aa.length()-3);
      aa = stripTrailing(aa, c);
      if(aa != result)
      {
         cout << "stripTrailing(string, char) failed: " << aa  << endl
              << "Should be: " << result << endl;
      }

      aa = ref;
      result = aa.substr(0, aa.length()-2);
      aa = stripTrailing(aa, d);
      if(aa != result)
      {
         cout << "stripTrailing(string, char *) failed: " << aa  << endl
              << "Should be: " << result << endl;
      }

         //######## Both Leading & Trailing
      aa = ref;
      result = aa.substr(1,aa.length()-2);
      aa = strip(aa, c, 1);
      if(aa != result)
      {
         cout << "strip(string, char, 1) failed: " << aa  << endl
              << "Should be: " << result << endl;
      }
  
      aa = ref;
      result = aa.substr(2,aa.length()-4);
      aa = strip(aa, d, 1);
      if(aa != result)
      {
         cout << "strip(string, char *, 1) failed: " << aa  << endl
              << "Should be: " << result << endl;
      }
  
      aa = ref;
      result = aa.substr(3, aa.length()-6);
      aa = strip(aa,c);
      if(aa != result)
      {
         cout << "strip(string, char) failed: " << aa  << endl
              << "Should be: " << result << endl;
      }
  
      aa = ref;
      result = aa.substr(2, aa.length()-4);
      aa = strip(aa,d);
      if(aa != result)
      {
         cout << "strip(string, char *) failed: " << aa  << endl
              << "Should be: " << result << endl;
      }

         //################################ 
         //translation
      string xlat = "The Hoopy Ford Prefect Boffs the Eccentrica Gallumbits";
      string xlati1 = "sdfgDFG";
      string xlato1 = "!)#%#)%";
      string xlata1 = "The Hoopy )or) Pre#ect Bo##! the Eccentrica %allumbit!";
      string xlati2 = "sdfgDFG";
      string xlato2 = "!)#%";
      string xlata2 = "The Hoopy  or) Pre#ect Bo##! the Eccentrica  allumbit!";
      string xlati3 = "sdfgDFG";
      string xlato3 = "sdfgDFG";
      string xlata3 = "The Hoopy Ford Prefect Boffs the Eccentrica Gallumbits";
      aa = translate(xlat, xlati1, xlato1);
      if (aa != xlata1)
      {
         cout << "translate(const string&,const string&,const string&,char) "
              << "failed: " << aa << endl
              << "should be " << xlata1 << endl;
      }
      aa = translate(xlat, xlati2, xlato2);
      if (aa != xlata2)
      {
         cout << "translate(const string&,const string&,const string&,char) "
              << "failed: " << aa << endl
              << "should be " << xlata2 << endl;
      }
      aa = translate(xlat, xlati3, xlato3);
      if (aa != xlata3)
      {
         cout << "translate(const string&,const string&,const string&,char) "
              << "failed: " << aa << endl
              << "should be " << xlata3 << endl;
      }

         //################################ 
         //String to number
      int res = asInt(a);
      if (1234567890 != res)
      {
         cout << "asInt(string&) failed: " << res << endl
              << "should be 1234567890" << endl;
      }

      a = "12345.67890";
      double res2 = asDouble(a);
      if ( (double)(12345.67890) != res2)
      {
         cout << "asDouble(string&) failed: " << setprecision(15) << res2 << endl
              << "should be 12345.67890 " << endl;
      }

      float res25 = asFloat(a);
      if( (float)(12345.67890) != res25)
      {
         cout << "asFloat(string&) failed: " << setprecision(15) << res25 << endl
              << "should be 12345.67890 " << endl;
      }
  
      long double lda = 12345.67890L;
      long double res3 = asLongDouble(a);
      if (lda != res3)
      {
         cout << "asLongDouble(string&) failed: " << setprecision(21) << res3
              << endl << "should be 12345.67890 " << endl;
      }
  
         //#####################################33
         // Number as string
    
      res2 = 12345.6789; // double
      res3 = 12345.6789L; // long double
      res = 1234567890; // int

      aa = asString(res2);
      if(asDouble(aa) != res2)
      {
         cout << "asString(double) failed: " << aa << endl
              << "should be 12345.6789" << endl;
      }
  
      aa = asString(res3);
      if(asLongDouble(aa) != res3)
      {
         cout << "asString(long double) failed: " << aa << endl
              << "should be 12345.6789" << endl;
      }

      aa = asString(res);
      if(asInt(aa) != res)
      {
         cout << "asString(X) failed: " << aa << endl
              << "should be " << res << endl;
      }
  
         // ##############################3
         // Hex tests

      string q="0x100 bleh";
      string z="100 moo";

      result = x2d(q);
      if (result != string("256 bleh"))
      {
         cout << "x2d failed: " << result << endl
              << "Should be: '256 bleh'"<< endl;
      }

      result = d2x(z);
      if (result != string("64 moo"))
      {
         cout << "x2d failed: " << result << endl
              << "Should be: '64 moo'"<< endl;
      }

         //##############################3
         // String replacement

      a   = "010101000111010";
      aa  = "0-0-0-000---0-0";
      string aaa = "0abc0abc0abc000abcabcabc0abc0";
  
      ref = a;
      a = replaceAll(a, string("1"), string("-"));
      if(a != aa)
      {
         cout << "replaceAll() failed: " << a << endl
              << "should be            " << aa << endl;
      }

      a = ref;
      a = replaceAll(a, string("1"), string("abc"));
      if(a != aaa)
      {
         cout << "replaceAll() failed: " << a << endl
              << "should be            " << aaa << endl;
      }

      a = ref;
      aaa = "01230123012300012312312301230";
      a = replaceAll(a, string("1"), string("123"));
      if(a != aaa)
      {
         cout << "replaceAll() failed: " << a << endl
              << "should be            " << aaa << endl;
      }

         //##############################################
         // Word Tests
      a = "      The  Quick    Brown  Fox     Jumps  Over    The  Lazy    Dog.    ";
  
      aa = firstWord(a);
      if(aa != string("The"))
      {
         cout << "firstWord() failed: " << aa << endl
              << "should be 'The'" << endl;
      }
  
      res = numWords(a);
      if(res != 9)
      {
         cout << "numWords() failed: " << res << endl
              << "should be 9" << endl;
      }
  
      aa = word(a, 4);
      if(aa != string("Jumps"))
      {
         cout << "word() failed: " << aa << endl
              << "should be 'Jumps'" << endl;
      }

      removeWords(a, 3, 2);
      result = "      The  Quick    Brown  Over    The  Lazy    Dog.    ";
      if(a != result)
      {
         cout << "removeWords() failed: " << a << endl
              << "should be '" << result << "'" << endl;
      }

      aa = stripFirstWord(a);
      result = "Quick    Brown  Over    The  Lazy    Dog.    ";
      if ((aa != string("The")) ||
          (a != result))
      {
         cout << "stripFirstWord() failed: " << aa << endl
              << "should be 'The'" << endl
              << "and: " << a << endl
              << "should be '" << result << "'" << endl;
      }

      removeWords(a, 5);
      result = "Quick    Brown  Over    The  Lazy";
      if (a != result)
      {
         cout << "removeWords failed: " << a << endl
              << "should be '" << result << "'" << endl;
      }

      aa = words(a, 3);
      result = "The  Lazy";
      if (aa != result)
      {
         cout << "words failed: \'" << aa << "' ("  << a << ")" << endl
              << "should be '" << result << "'" << endl;
      }

         //####################
         // regular expressions

      rc = isLike(a, "Br.*Over.*");
      if (rc != 1)
      {
         cout << "isLike (Br.*Over.*) failed: " << rc << endl
              << "should be: 1" << endl;
      }

      rc = isLike(a, "Br.Over.*");
      if (rc != 0)
      {
         cout << "isLike (Br.Over.*) failed: " << rc << endl
              << "should be: 0" << endl;
      }

      aa = matches(a, "Br.*Over.*");
      result = "Brown  Over    The  Lazy";
      if (aa != result)
      {
         cout << "matches failed: " << aa << endl
              << "should be: '" << result << "'" << endl;
      }

      string prettyPrintThis("This is a very long sentence that will get cut up into lines with a maximum length of 40 characters, each line indented by a few spaces.");
      prettyPrint(prettyPrintThis, "\n", "    ", "      ", 40);
      cout << prettyPrintThis << endl;

      cout << "All other Tests Passed." << endl; 

      return 0;
   }
   catch (gpstk::Exception& e)
   {
      cout << e << endl;
   }
}
