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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================

#include "FileFilter.hpp"
#include "FFData.hpp"
#include "FFStream.hpp"

/**
 * @file AnotherFileFilterTest.cpp
 * Here's another test of the FileFilter class
 */

using namespace gpstk;
using namespace std;

// an FFData with just an int
class TestFFData : public FFData
{
public:
   TestFFData(int i = 0) : val(i) {}

   virtual ~TestFFData() {}

   void reallyPutRecord(FFStream& s) const 
      throw(FFStreamError, gpstk::StringUtils::StringException)
      {}


   void reallyGetRecord(FFStream& s) 
      throw(FFStreamError, gpstk::StringUtils::StringException)
      {}


   virtual void dump(std::ostream& s) const {s << val;}

   int val;
};


// an operator < for TestFFData
struct TestOperatorLessThan : 
   public binary_function<TestFFData, TestFFData, bool>
{
public:
   bool operator() (TestFFData l, TestFFData r) const
      {
         return (l.val < r.val);
      }
};

// an operator == for TestFFData
struct TestOperatorEquals : 
   binary_function<TestFFData, TestFFData, bool>
{
public:
   bool operator() (TestFFData l, TestFFData r) const
      {
         return (l.val == r.val);
      }
};

// a filter for a range of values for TestFFData
struct TestRangeFilter : 
   public unary_function<TestFFData, bool>
{
public:
   TestRangeFilter(const int b, const int e)
         : begin(b), end(e)
      {}
   
   bool operator() (TestFFData l) const
      {
         if ( (l.val < begin) ||
              (l.val > end) )
            return true;
         return false;
      }
   
private:
   int begin, end;
   
};


// a removing filter for a single value
struct TestValueFilter : 
   public unary_function<TestFFData, bool>
{
public:
   TestValueFilter(const int val)
         : value(val)
      {}

   bool operator() (TestFFData l) const
      {
         if (value == l.val)
            return true;
         return false;
      }

private:
   int value;
};



main (int argc, char *argv[])
{
   FileFilter<TestFFData> ff;

      // add data to the filter
   ff.addData(TestFFData(1));
   ff.addData(TestFFData(2));
   ff.addData(TestFFData(2));
   ff.addData(TestFFData(2));
   ff.addData(TestFFData(4));
   ff.addData(TestFFData(4));
   ff.addData(TestFFData(5));
   ff.addData(TestFFData(3));
   ff.addData(TestFFData(3));
   ff.addData(TestFFData(1));

      // do various operations on the data

   list<TestFFData>::iterator itr;

   cout << "unsorted" << endl;
   for(itr = ff.begin(); itr != ff.end(); itr++)
      (*itr).dump(cout << ' '); cout << endl;

   cout << "sorted" << endl;
   ff.sort(TestOperatorLessThan());
   for(itr = ff.begin(); itr != ff.end(); itr++)
      (*itr).dump(cout << ' '); cout << endl;

   cout << "filter out values > 3" << endl;
   ff.filter(TestRangeFilter(1,3));
   for(itr = ff.begin(); itr != ff.end(); itr++)
      (*itr).dump(cout << ' '); cout << endl;

   cout << "filter out 2" << endl;
   ff.filter(TestValueFilter(2));
   for(itr = ff.begin(); itr != ff.end(); itr++)
      (*itr).dump(cout << ' '); cout << endl;

   cout << "unique only" << endl;
   ff.unique(TestOperatorEquals());
   for(itr = ff.begin(); itr != ff.end(); itr++)
      (*itr).dump(cout << ' '); cout << endl;
}
