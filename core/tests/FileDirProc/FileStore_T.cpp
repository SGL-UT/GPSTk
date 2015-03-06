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

#include "FileStore.hpp"
#include "TestUtil.hpp"
#include <iostream>

using namespace std;
using namespace gpstk;

class FileStore_T
{
public: 

      // constructor
   FileStore_T() { }

      // destructor
   ~FileStore_T() { }

      // run all tests
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testEverything();

private:

   struct TestHeaderType
   {
      TestHeaderType(int i = 0) : value(i) {}
      bool operator==(const TestHeaderType& other) const { return (other.value == value); }
      void dump(std::ostream& os = std::cout) const { os << value << endl; }
      int  value;
   };

      // emit a vector of strings to standard output
      // @param strs vector of strings to emit 
   void dump(const vector<string>& strs);

}; // class FileStore_T


//---------------------------------------------------------------------------
int FileStore_T :: testEverything()
{
   TestUtil  initTester(   "FileStore", "initialization", __FILE__, __LINE__ );
   TestUtil  sizeTester(   "FileStore", "size",           __FILE__, __LINE__ );
   TestUtil  nfilesTester( "FileStore", "nfiles",         __FILE__, __LINE__ );
   TestUtil  clearTester(  "FileStore", "clear",          __FILE__, __LINE__ );
   TestUtil  getTester(    "FileStore", "getFilenames",   __FILE__, __LINE__ );
   TestUtil  addTester(    "FileStore", "addFile",        __FILE__, __LINE__ );
   TestUtil  headerTester( "FileStore", "getHeader",      __FILE__, __LINE__ );

   try   // default initialization
   {
      FileStore<TestHeaderType>  store;
      initTester.assert( true, "unexpected exception", __LINE__ );
   }
   catch (...)
   {
      initTester.assert( false, "unexpected exception", __LINE__ );
   }

   FileStore<TestHeaderType>  store;  // Use this for the remaining tests

   try   // empty store (size)
   {      
      sizeTester.assert( (0 == store.size() ), "empty store expected", __LINE__ );
   }
   catch (...)
   {
      sizeTester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // empty store (nfiles)
   {
      nfilesTester.assert( (0 == store.nfiles() ), "empty store expected", __LINE__ );
   }
   catch (...)
   {
      nfilesTester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // empty store (getFilenames)
   {
      getTester.assert( (0 == store.getFileNames().size() ), "empty store expected", __LINE__ );
   }
   catch (...)
   {
      getTester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // empty store (clear)
   {
      store.clear();
      clearTester.assert( true, "unexpected exception", __LINE__ );
   }
   catch (...)
   {
      clearTester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // empty store (getHeader)
   {
      const TestHeaderType&  header = store.getHeader("filename");
      headerTester.assert( false, "expected exception", __LINE__ );
   }
   catch (InvalidRequest& ire)
   {
      headerTester.assert( true, "expected exception", __LINE__ );
   }
   catch (...)
   {
      headerTester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // empty store (addFile)
   {
      TestHeaderType  header(1);
      store.addFile("testfile1", header);

      addTester.assert( true, "unexpected exception", __LINE__ );
      sizeTester.assert( (1 == store.size() ), "single file expected", __LINE__ );
      nfilesTester.assert( (1 == store.nfiles() ), "single file expected", __LINE__ );

      std::vector<std::string>  filenames = store.getFileNames();
      getTester.assert( (1 == filenames.size() ), "single file expected", __LINE__ );
      if (filenames.size() > 0)
      {
         getTester.assert( (0 == filenames[0].compare("testfile1") ), "unexpected filename", __LINE__ );
      }
   }
   catch (...)
   {
      addTester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // non-empty store (getHeader, present)
   {
      const TestHeaderType&  header = store.getHeader("testfile1");
      headerTester.assert( true, "unexpected exception", __LINE__ );
      headerTester.assert( (1 == header.value), "unexpected header", __LINE__ );
   }
   catch (...)
   {
      headerTester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // non-empty store (getHeader, absent)
   {
      const TestHeaderType&  header = store.getHeader("foo");
      headerTester.assert( false, "expected exception", __LINE__ );
   }
   catch (InvalidRequest& ire)
   {
      headerTester.assert( true, "expected exception", __LINE__ );
   }
   catch (...)
   {
      headerTester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // non-empty store (addFile)
   {
      TestHeaderType  header(2);
      store.addFile("testfile2", header);

      addTester.assert( true, "unexpected exception", __LINE__ );
      sizeTester.assert( (2 == store.size() ), "two files expected", __LINE__ );
      nfilesTester.assert( (2 == store.nfiles() ), "two files expected", __LINE__ );

      std::vector<std::string>  filenames = store.getFileNames();
      getTester.assert( (2 == filenames.size() ), "two files expected", __LINE__ );
   }
   catch (...)
   {
      addTester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // non-empty store (getHeader, present)
   {
      const TestHeaderType&  header = store.getHeader("testfile2");
      headerTester.assert( true, "unexpected exception", __LINE__ );
      headerTester.assert( (2 == header.value), "unexpected header", __LINE__ );
   }
   catch (...)
   {
      headerTester.assert( false, "unexpected exception", __LINE__ );
   }

   try   // non-empty store (clear)
   {
      store.clear();
      clearTester.assert( true, "unexpected exception", __LINE__ );      
      sizeTester.assert( (0 == store.size() ), "empty store expected", __LINE__ );
      nfilesTester.assert( (0 == store.nfiles() ), "empty store expected", __LINE__ );
      getTester.assert( (0 == store.getFileNames().size() ), "empty store expected", __LINE__ );
   }
   catch (...)
   {
      clearTester.assert( false, "unexpected exception", __LINE__ );
   }

   return initTester.countFails() +
          sizeTester.countFails() +
          nfilesTester.countFails() +
          clearTester.countFails() +
          getTester.countFails() +
          addTester.countFails() +
          headerTester.countFails();
}


/** Initialize and run all tests.
 *
 * @return Total error count for all tests
 */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   FileStore_T  testClass;

   errorTotal += testClass.testEverything();
   
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal;
}