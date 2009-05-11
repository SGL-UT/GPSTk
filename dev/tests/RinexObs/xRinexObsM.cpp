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
//============================================================================

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
	testrunner.addTest (CPPUNIT_NS :: TestFactoryRegistry :: getRegistry ().makeTest ());
	testrunner.run (testresult);

	// output results in compiler-format
	CPPUNIT_NS :: CompilerOutputter compileroutputter (&collectedresults, std::cerr);
	compileroutputter.write ();

	// return 0 if tests were successful
	return collectedresults.wasSuccessful () ? 0 : 1;
}
