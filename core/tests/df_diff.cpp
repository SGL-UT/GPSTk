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

// This program is intended to help differencing data files and not
// failing on small differences in floating point values.


#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#include "BasicFramework.hpp"
#include "StringUtils.hpp"

using namespace std;

class DFDiff : public gpstk::BasicFramework
{
public:
   DFDiff(const string& applName)
      throw()
      : gpstk::BasicFramework(
         applName,
         "Differences two input files while allowing small differences"
         "in floating point values."),
        epsilon(1e-5),
        linesToSkip(0)
   {};

   // While this is in C11, we don't want to work under C03
   double stringToDouble(const string& s, bool& isDouble)
   {
      char *p;
      double v = strtod(s.c_str(), &p);
      if (p == s.c_str())
         isDouble = false;
      else
         isDouble = true;
      return v;
   }

   bool initialize(int argc, char *argv[]) throw()
   {
      gpstk::CommandOptionWithAnyArg
         input1Option('1', "input1", "First file to take the input from.", true),
         input2Option('2', "input2", "Second file to take the input from.", true),
         lineSkipOption('l', "lines", "Number of lines to skip at beginning of file."),
         epsilonOption('e', "epsilon", "Percent allowable difference in floating point values."),
         outputOption('o', "output", "A file to receive the output. The default is stdout.");

      if (!BasicFramework::initialize(argc,argv))
         return false;


      input1Fn = input1Option.getValue()[0];
      input2Fn = input2Option.getValue()[0];

      input1.open(input1Fn.c_str(), ios::in);
      input2.open(input2Fn.c_str(), ios::in);
      
      if (outputOption.getCount())
         outputFn = outputOption.getValue()[0];

      if (outputFn=="-" || outputFn=="")
      {
         output.copyfmt(cout);
         output.clear(cout.rdstate());
         output.ios::rdbuf(cout.rdbuf());
         outputFn = "<stdout>";
      }
      else
      {
         output.open(outputFn.c_str(), ios::out);
      }

      if (!output)
      {
         cerr << "Could not open: " << outputFn << endl;
         exitCode=1;
         return false;
      }

      if (epsilonOption.getCount())
         epsilon = gpstk::StringUtils::asDouble(epsilonOption.getValue()[0]);

      if (lineSkipOption.getCount())
         linesToSkip = gpstk::StringUtils::asInt(lineSkipOption.getValue()[0]);
      
      if (debugLevel)
         output << "First file " << input1Fn << endl
                << "Second file " << input2Fn << endl
                << "Output file " << outputFn << endl
                << "Epsilon " << epsilon << endl
                << "Skipping " << linesToSkip << " lines." << endl;
      
      return true;
   }
   
protected:
   virtual void spinUp()
   {}

   virtual void process()
   {
      try
      {
         for (int i=0; i<linesToSkip; i++)
         {
            char buffer[1024];
            input1.getline(buffer, sizeof(buffer));
            input2.getline(buffer, sizeof(buffer));
            if (bool(input1) != bool(input2))
            {
               exitCode += 1;
               break;
            }
            if (debugLevel>1)
               cout << "Skip" << endl;
         }
      
         do
         {
            string s1, s2;
            input1 >> s1;
            input2 >> s2;
         
            if (bool(input1) != bool(input2))
               exitCode += 1;
         
            if (s1 != s2)
            {
               if (verboseLevel)
                  output << s1 << " .. " << s2;
               
               bool df1,df2;
               double d1 = stringToDouble(s1, df1);
               double d2 = stringToDouble(s2, df2);
               if (df1 && df2)
               {
                  double diff = d1-d2;
                  double err = d2;
                  if (d1 != 0)
                     err = diff/d1;
               
                  if (abs(err) > epsilon)
                     exitCode += 1;
                  
                  if (verboseLevel)
                     output << " err: " << err;
               }
               else
                  exitCode += 1;
               
               if (verboseLevel)
                  output << endl;
            }
         } while (input1 && input2);
      }
      catch (std::exception& e)
      {
         cout << e.what() << endl;
         exitCode += 1;
      }

      if (verboseLevel)
         output << "Total differences: " << exitCode << endl;
   }

   virtual void shutDown()
   {}

   string input1Fn, input2Fn, outputFn;
   ofstream output;
   ifstream input1, input2;
   double epsilon;
public:
   long linesToSkip;
};


int main(int argc, char *argv[])
{
   DFDiff crap(argv[0]);

   if (crap.initialize(argc, argv))
      crap.run();
   
   return crap.exitCode;
}

