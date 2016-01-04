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

// CppUnit-Tutorial
// file: ftest.cc

#include <cppunit/CompilerOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>

int main (int argc, char* argv[])
{

      // informs test-listener about testresults
   CPPUNIT_NS :: TestResult testresult;

      // register listener for collecting the test-results
   CPPUNIT_NS :: TestResultCollector collectedresults;
   testresult.addListener (&collectedresults);

      // insert test-suite at test-runner by registry
   CPPUNIT_NS :: TestRunner testrunner;
   testrunner.addTest (CPPUNIT_NS :: TestFactoryRegistry ::
                       getRegistry ().makeTest ());
   testrunner.run (testresult);

      // output results in compiler-format
   CPPUNIT_NS :: CompilerOutputter compileroutputter (&collectedresults,
         std::cerr);
   compileroutputter.write ();

      // return 0 if tests were successful
   return collectedresults.wasSuccessful () ? 0 : 1;
}
