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

#include "FileFilter.hpp"
#include "TestUtil.hpp"
#include <algorithm>
#include <iostream>
#include <list>

using namespace std;
using namespace gpstk;

class FileFilter_T
{
public: 

      // constructor
   FileFilter_T() { init(); }

      // desructor
   ~FileFilter_T() { cleanup(); }

      // initialize tests
   void init();

      // remove file system objectst created during tests
   void cleanup();

      // test creation of an initialization of FileFilter objects
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testInitialization();

      // test addData(), getData(), getDataCount(), empty(), and clear()
      // @return  number of failures, i.e., 0=PASS, !0=FAIL
   int testData();

   int testBeginEnd();
   int testFrontBack();
   int testBack();
   int testFilter();
   int testSort();
   int testMerge();
   int testTouch();
   int testUnique();
   int testDiff();
   int testFindAll();

private:

      // FileFilter Test Data - dummy data class with which to run tests
   class FFTData : public FFData
   {
   public:
      FFTData(int val) : value(val) {}
      ~FFTData() {}

      int  value;

   private:

      virtual void reallyGetRecord(FFStream& s)
         throw(std::exception, gpstk::StringUtils::StringException, 
               gpstk::FFStreamError) { /* do nothing */ }

         /// Does the actual writing from the stream into this FFData object.
      virtual void reallyPutRecord(FFStream& s) const
         throw(std::exception, gpstk::StringUtils::StringException, 
               gpstk::FFStreamError) { /* do nothing */ }

   }; // class Dummy

   typedef list<FFTData>  FFTDataList;

      // Unary predicate that returns true for data that has an even value
   struct IsEven
   {
      bool operator() (const FFTData& fftd);
   };

      // Unary predicate that returns true for data that has a value > 999
   struct Above999
   {
      bool operator() (const FFTData& fftd);
   };

      // Unary operation that doubles a data element if it is divisible by 5
      // and returns true.  Elements not divisible by 5 are unchanged
      // and false is returned.
   struct DoubleFives
   {
      bool operator() (FFTData& fftd);
   };

      // Binary predicate that implements "less-than" for data values
   struct LessThan
   {
      bool operator() (const FFTData& left, const FFTData& right) const;
   };

      // Binary predicate that implements "equal" for data values
   struct Equal
   {
      bool operator() (const FFTData& left, const FFTData& right) const;
   };

      // emit a list of objects to standard output
      // @param list objects to emit 
   void dump(const FFTDataList& list);

   FFTDataList  sampleData;

}; // class FileFilter_T


//---------------------------------------------------------------------------
bool FileFilter_T :: IsEven :: operator() (const FFTData& fftd)
{
   return (0 == (fftd.value % 2) );
}


//---------------------------------------------------------------------------
bool FileFilter_T :: Above999 :: operator() (const FFTData& fftd)
{
   return (fftd.value > 999);
}


//---------------------------------------------------------------------------
bool FileFilter_T :: DoubleFives :: operator() (FFTData& fftd)
{
   if (0 == (fftd.value % 5) )
   {
      fftd.value *= 2;
      return true;
   }
   return false;
}


//---------------------------------------------------------------------------
bool FileFilter_T :: LessThan :: operator() (const FFTData& left, const FFTData& right) const
{
   return (left.value < right.value);
}


//---------------------------------------------------------------------------
bool FileFilter_T :: Equal :: operator() (const FFTData& left, const FFTData& right) const
{
   return (left.value == right.value);
}


//---------------------------------------------------------------------------
void FileFilter_T :: init()
{
      // create 100 test objects
   for (int i = 0; i < 100; ++i)
      sampleData.push_back( FFTData(i) );
}


//---------------------------------------------------------------------------
void FileFilter_T :: cleanup()
{
   // empty
}


//---------------------------------------------------------------------------
int FileFilter_T :: testInitialization()
{
   TestUtil  tester( "FileFilter", "initialization/getDataCount/size/empty", __FILE__, __LINE__ );

   try
   {
      FileFilter<FFTData>  ff;
      tester.assert( true, "error creating FileFilter", __LINE__ );

      tester.assert( (0 == ff.getDataCount() ), "data count should be 0", __LINE__ );

      tester.assert( (0 == ff.size() ), "data size should be 0", __LINE__ );

      tester.assert( ff.empty(), "data list should be empty", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "error creating FileFilter", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileFilter_T :: testData()
{
   TestUtil  tester( "FileFilter", "addData/getData/getDataCount/size/empty/clear", __FILE__, __LINE__ );

   try   // empty data list
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      ff.addData(list);
      tester.assert( true, "error adding data", __LINE__ );

      tester.assert( (0 == ff.getDataCount() ), "data count should be 0", __LINE__ );

      tester.assert( (0 == ff.size() ), "data size should be 0", __LINE__ );

      tester.assert( ff.empty(), "data list should be empty", __LINE__ );

      FFTDataList  got = ff.getData();
      tester.assert( true, "error getting data", __LINE__ );

      tester.assert( (0 == got.size() ), "got data size should be 0", __LINE__ );

      ff.clear();
      tester.assert( true, "error clearing data", __LINE__ );

      tester.assert( (0 == ff.getDataCount() ), "cleared data count should be 0", __LINE__ );

      tester.assert( (0 == ff.size() ), "cleared data size should be 0", __LINE__ );

      tester.assert( ff.empty(), "cleared data list should be empty", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "error adding data", __LINE__ );
   }

   try   // single-item data list
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(1) );
      ff.addData(list);
      tester.assert( true, "error adding data", __LINE__ );

      tester.assert( (1 == ff.getDataCount() ), "data count should be 1", __LINE__ );

      tester.assert( (1 == ff.size() ), "data size should be 1", __LINE__ );

      tester.assert( !ff.empty(), "data list should not be empty", __LINE__ );

      FFTDataList  got = ff.getData();
      tester.assert( true, "error getting data", __LINE__ );

      tester.assert( (1 == got.size() ), "got data size should be 1", __LINE__ );

      ff.clear();
      tester.assert( true, "error clearing data", __LINE__ );

      tester.assert( (0 == ff.getDataCount() ), "cleared data count should be 0", __LINE__ );

      tester.assert( (0 == ff.size() ), "cleared data size should be 0", __LINE__ );

      tester.assert( ff.empty(), "cleared data list should be empty", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "error adding data", __LINE__ );
   }

   try   // many-item data list
   {
      FileFilter<FFTData>  ff;
      ff.addData( sampleData );
      tester.assert( true, "error adding data", __LINE__ );

      ostringstream  oss;
      oss << sampleData.size();

      tester.assert( (sampleData.size() == ff.getDataCount() ),
                     "data count should be " + oss.str() , __LINE__ );

      tester.assert( (sampleData.size() == ff.size() ),
                    "data size should be " + oss.str(), __LINE__ );

      tester.assert( !ff.empty(), "data list should not be empty", __LINE__ );

      FFTDataList  got = ff.getData();
      tester.assert( true, "error getting data", __LINE__ );

      tester.assert( (sampleData.size() == got.size() ),
                     "got data size should be " + oss.str(), __LINE__ );

      ff.clear();
      tester.assert( true, "error clearing data", __LINE__ );

      tester.assert( (0 == ff.getDataCount() ), "cleared data count should be 0", __LINE__ );

      tester.assert( (0 == ff.size() ), "cleared data size should be 0", __LINE__ );

      tester.assert( ff.empty(), "cleared data list should be empty", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "error adding data", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileFilter_T :: testBeginEnd()
{
   TestUtil  tester( "FileFilter", "begin/end", __FILE__, __LINE__ );

   try   // empty data list
   {
      FileFilter<FFTData>  ff;

      FFTDataList::iterator bIter = ff.begin();
      tester.assert( true, "iterator error", __LINE__ );

      FFTDataList::iterator eIter = ff.end();
      tester.assert( true, "iterator error", __LINE__ );

      tester.assert( (bIter == eIter), "iterator", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "iterator error", __LINE__ );
   }

   try   // single-item data list
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(1) );
      ff.addData(list);

      FFTDataList::iterator bIter = ff.begin();
      tester.assert( true, "iterator error", __LINE__ );

      FFTDataList::iterator eIter = ff.end();
      tester.assert( true, "iterator error", __LINE__ );

      tester.assert( (bIter != eIter), "iterator", __LINE__ );

      bIter++;
      tester.assert( true, "iterator error", __LINE__ );

      tester.assert( (bIter == eIter), "iterator", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "iterator error", __LINE__ );
   }

   try   // multiple-item data list
   {
      FileFilter<FFTData>  ff;
      ff.addData( sampleData );

      FFTDataList::iterator bIter = ff.begin();
      tester.assert( true, "iterator error", __LINE__ );

      FFTDataList::iterator eIter = ff.end();
      tester.assert( true, "iterator error", __LINE__ );

      tester.assert( (bIter != eIter), "iterator error", __LINE__ );

      int advances = 0;
      FFTDataList::const_iterator  sampleIter = sampleData.begin();
      while ( (bIter != eIter) && (sampleIter != sampleData.end() ) )
      {
         if ( bIter->value != sampleIter->value )
         {
            tester.assert( false, "iterator error - value mismatch", __LINE__ );
            break;
         }
         bIter++;
         sampleIter++;
         advances++;
      }
      tester.assert( (advances == sampleData.size() ), "iterator error - premature end", __LINE__ );

      tester.assert( (bIter == eIter), "iterator error - end not reached", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "iterator error", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileFilter_T :: testFrontBack()
{
   TestUtil  tester( "FileFilter", "front/back", __FILE__, __LINE__ );

   try   // empty data list - front()
   {
      FileFilter<FFTData>  ff;

      FFTData&  data = ff.front();
      tester.assert( false, "expected exception for empty list", __LINE__ );
   }
   catch (...)
   {
      tester.assert( true, "expected exception for empty list", __LINE__ );
   }

   try   // empty data list - back()
   {
      FileFilter<FFTData>  ff;

      FFTData&  data = ff.back();
      tester.assert( false, "expected exception for empty list", __LINE__ );
   }
   catch (...)
   {
      tester.assert( true, "expected exception for empty list", __LINE__ );
   }

   try   // single-item data list
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(1) );
      ff.addData(list);

      FFTData&  fdata = ff.front();
      tester.assert( (fdata.value == 1), "error accessing list front", __LINE__ );

      FFTData&  bdata = ff.back();
      tester.assert( (bdata.value == 1), "error accessing list back", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "error accessing list front/back", __LINE__ );
   }

   try   // multiple-item data list
   {
      FileFilter<FFTData>  ff;
      ff.addData( sampleData );

      FFTData&  fdata = ff.front();
      tester.assert( (fdata.value == sampleData.front().value),
                     "error accessing list front", __LINE__ );

      FFTData&  bdata = ff.back();
      tester.assert( (bdata.value == sampleData.back().value),
                     "error accessing list back", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "error accessing list front/back", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileFilter_T :: testFilter()
{
   TestUtil  tester( "FileFilter", "filter/getFiltered", __FILE__, __LINE__ );

   try   // empty data list
   {
      FileFilter<FFTData>  ff;

      tester.assert( (0 == ff.getFiltered() ),
                     "no filtering has occurred", __LINE__ );

      tester.assert( (0 == ff.filter(IsEven()).getFiltered() ),
                     "empty list expected after filtering", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception filtering empty list", __LINE__ );
   }

   try   // single-item data list filtered to empty
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(2) );
      ff.addData(list);

      tester.assert( (0 == ff.getFiltered() ),
                     "no filtering has occurred", __LINE__ );

      tester.assert( (1 == ff.filter(IsEven() ).getFiltered() ),
                     "expected filtering", __LINE__ );

      tester.assert( (0 == ff.getDataCount() ),
                     "empty list expected following filtering", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception filtering non-empty list", __LINE__ );
   }

   try   // single-item data list filtered to no effect
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(3) );
      ff.addData(list);

      tester.assert( (0 == ff.getFiltered() ),
                     "no filtering has occurred", __LINE__ );

      tester.assert( (0 == ff.filter(IsEven()).getFiltered() ),
                     "expected no filtering", __LINE__ );

      tester.assert( (1 == ff.getDataCount() ),
                     "non-empty list expected following filtering", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception filtering non-empty list", __LINE__ );
   }

   try   // multiple-item data list filtering
   {
      FileFilter<FFTData>  ff;
      ff.addData( sampleData );  // assumes 100 elements

      tester.assert( (0 == ff.getFiltered() ),
                     "no filtering has occurred", __LINE__ );

      tester.assert( (50 == ff.filter(IsEven()).getFiltered() ),
                     "expected filtering", __LINE__ );

      tester.assert( (50 == ff.getDataCount() ),
                     "non-empty list expected following filtering", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "error accessing list front/back", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileFilter_T :: testSort()
{
   TestUtil  tester( "FileFilter", "sort", __FILE__, __LINE__ );

   try   // empty data list
   {
      FileFilter<FFTData>  ff;

      tester.assert( (0 == ff.sort(LessThan() ).getDataCount() ),
                     "empty list expected after sorting", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception sorting empty list", __LINE__ );
   }

   try   // single-item data list
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(2) );
      ff.addData(list);

      tester.assert( (1 == ff.sort(LessThan() ).getDataCount() ),
                     "one item expected after sorting", __LINE__ );

      tester.assert( (2 == ff.front().value), "unexpected value", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception sorting non-empty list", __LINE__ );
   }

   try   // two-item, out-of-order data list
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(2) );
      list.push_back( FFTData(1) );
      ff.addData(list);

      tester.assert( (2 == ff.sort(LessThan() ).getDataCount() ),
                     "two items expected after sorting", __LINE__ );

      tester.assert( (1 == ff.front().value), "unexpected value", __LINE__ );
      tester.assert( (2 == ff.back().value), "unexpected value", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception sorting non-empty list", __LINE__ );
   }

   try   // multiple-item, pre-sorted data list
   {
      FileFilter<FFTData>  ff;
      ff.addData( sampleData );  // assumes 100 elements

      tester.assert( (100 == ff.sort(LessThan() ).getDataCount() ),
                     "100 items expected after sorting", __LINE__ );

      bool  sorted = true;
      int  previous = 0;
      FFTDataList::const_iterator  ffIter = ff.begin();
      for ( ; ffIter != ff.end(); ++ffIter)
      {
         if ( (ffIter != ff.begin()) && (ffIter->value < previous) )
         {
            sorted = false;
            break;
         }
         previous = ffIter->value;
      }
      tester.assert( sorted, "data list was not sorted", __LINE__ );

   }
   catch (...)
   {
      tester.assert( false, "error accessing list front/back", __LINE__ );
   }

   try   // multiple-item, un-sorted data list
   {
      FileFilter<FFTData>  ff;
      FFTDataList  reversedData( sampleData );
      reverse(reversedData.begin(), reversedData.end() );
      ff.addData( reversedData );  // assumes 100 elements

      tester.assert( (100 == ff.sort(LessThan() ).getDataCount() ),
                     "100 items expected after sorting", __LINE__ );

      bool  sorted = true;
      int  previous = 0;
      FFTDataList::const_iterator  ffIter = ff.begin();
      for ( ; ffIter != ff.end(); ++ffIter)
      {
         if ( (ffIter != ff.begin()) && (ffIter->value < previous) )
         {
            sorted = false;
            break;
         }
         previous = ffIter->value;
      }
      tester.assert( sorted, "data list was not sorted", __LINE__ );

   }
   catch (...)
   {
      tester.assert( false, "error accessing list front/back", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileFilter_T :: testMerge()
{
   TestUtil  tester( "FileFilter", "merge", __FILE__, __LINE__ );

   try   // empty data lists
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;

      tester.assert( (0 == ff.merge(ffOther).getDataCount() ),
                     "empty list expected", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception merging empty lists", __LINE__ );
   }

   try   // empty data lists (sorted)
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;

      tester.assert( (0 == ff.merge(ffOther, LessThan() ).getDataCount() ),
                     "empty list expected", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception merging empty lists", __LINE__ );
   }

   try   // one empty data list (first)
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;
      FFTDataList  list;
      list.push_back( FFTData(1) );
      ffOther.addData(list);

      tester.assert( (1 == ff.merge(ffOther).getDataCount() ),
                     "unexpected element count", __LINE__ );

      tester.assert( ( (1 == ff.getDataCount() ) && (1 == ff.front().value) ),
                     "unexpected value", __LINE__ );

      if ( 1 != ff.getDataCount() )
      {
         dump(ff.getData() );
      }
   }
   catch (...)
   {
      tester.assert( false, "exception merging lists", __LINE__ );
   }

   try   // one empty data list (second)
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;
      FFTDataList  list;
      list.push_back( FFTData(1) );
      ff.addData(list);

      tester.assert( (1 == ff.merge(ffOther).getDataCount() ),
                     "unexpected element count", __LINE__ );

      tester.assert( ( (1 == ff.getDataCount() ) && (1 == ff.front().value) ),
                     "unexpected value", __LINE__ );

      if ( 1 != ff.getDataCount() )
      {
         dump(ff.getData() );
      }
   }
   catch (...)
   {
      tester.assert( false, "exception merging lists", __LINE__ );
   }

   try   // two non-empty data lists
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;
      FFTDataList  list;
      list.push_back( FFTData(2) );
      ff.addData(list);
      list.clear();
      list.push_back( FFTData(1) );
      ffOther.addData(list);

      tester.assert( (2 == ff.merge(ffOther).getDataCount() ),
                     "unexpected element count", __LINE__ );

      if ( 2 != ff.getDataCount() )
      {
         dump(ff.getData() );
      }
   }
   catch (...)
   {
      tester.assert( false, "exception merging lists", __LINE__ );
   }

   try   // two non-empty data lists (sorted)
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;
      FFTDataList  list;
      list.push_back( FFTData(3) );
      list.push_back( FFTData(1) );
      ff.addData(list);
      list.clear();
      list.push_back( FFTData(4) );
      list.push_back( FFTData(2) );
      ffOther.addData(list);

      tester.assert( (4 == ff.merge(ffOther, LessThan() ).getDataCount() ),
                     "unexpected element count", __LINE__ );

      if ( 4 == ff.getDataCount() )
      {
         tester.assert( (1 == ff.front().value),
                        "unexpected value", __LINE__ );

         tester.assert( (4 == ff.back().value),
                        "unexpected value", __LINE__ );
      }
      else
      {
         dump(ff.getData() );
      }
   }
   catch (...)
   {
      tester.assert( false, "exception merging lists", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileFilter_T :: testTouch()
{
   TestUtil  tester( "FileFilter", "touch", __FILE__, __LINE__ );

   try   // empty data list
   {
      FileFilter<FFTData>  ff;

      tester.assert( (0 == ff.getFiltered() ),
                     "no operation has occurred", __LINE__ );

      tester.assert( (0 == ff.touch(DoubleFives() ).getFiltered() ),
                     "no operations expected", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception operating on an empty list", __LINE__ );
   }

   try   // single-item data list filtered to empty
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(5) );
      ff.addData(list);

      tester.assert( (0 == ff.getFiltered() ),
                     "no operation has occurred", __LINE__ );

      tester.assert( (1 == ff.touch(DoubleFives() ).getFiltered() ),
                     "expected a single operation", __LINE__ );

      tester.assert( (1 == ff.getDataCount() ),
                     "unexpected change to data count", __LINE__ );

      FFTData&  fdata = ff.front();
      tester.assert( (10 == fdata.value),
                     "operation was not applied", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception operating on a non-empty list", __LINE__ );
   }

   try   // single-item data list filtered to no effect
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(3) );
      ff.addData(list);

      tester.assert( (0 == ff.getFiltered() ),
                     "no operation has occurred", __LINE__ );

      tester.assert( (0 == ff.touch(DoubleFives() ).getFiltered() ),
                     "no operations expected", __LINE__ );

      tester.assert( (1 == ff.getDataCount() ),
                     "unexpected change to data count", __LINE__ );

      FFTData&  fdata = ff.front();
      tester.assert( (3 == fdata.value),
                     "operation was erroneously applied", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception operating on a non-empty list", __LINE__ );
   }

   try   // multiple-item data list filtering
   {
      FileFilter<FFTData>  ff;
      ff.addData( sampleData );  // assumes 100 elements

      tester.assert( (0 == ff.getFiltered() ),
                     "no operation has occurred", __LINE__ );

      tester.assert( (20 == ff.touch(DoubleFives() ).getFiltered() ),
                     "expected 50 operations", __LINE__ );

      tester.assert( (100 == ff.getDataCount() ),
                     "unexpected change to data count", __LINE__ );

         // check that the operation was applied to all appropriate elememts,
         // i.e. make sure every 5th element is divisible by 10
      bool  applied = true;
      int i = 0;
      FFTDataList::const_iterator  ffIter = ff.begin();
      for ( ; ffIter != ff.end(); ++i, ++ffIter)
      {
         if ( (0 == (i % 5) ) && (0 != (ffIter->value % 10) ) )
         {
            applied = false;
            break;
         }
      }
      tester.assert( applied, "operation was not applied to all approriate elments", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception operating on a non-empty list", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileFilter_T :: testUnique()
{
   TestUtil  tester( "FileFilter", "unique", __FILE__, __LINE__ );

   // @note - unique() requires a sorted list as input

   try   // empty data list
   {
      FileFilter<FFTData>  ff;

      tester.assert( (0 == ff.unique(Equal() ).getDataCount() ),
                     "empty list expected", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception for empty list", __LINE__ );
   }

   try   // single-item data list
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(2) );
      ff.addData(list);

      tester.assert( (1 == ff.unique(Equal() ).getDataCount() ),
                     "one item expected", __LINE__ );

      tester.assert( (2 == ff.front().value), "unexpected value", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception for non-empty list", __LINE__ );
   }

   try   // two-item data list
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(2) );
      list.push_back( FFTData(2) );
      ff.addData(list);

      tester.assert( (1 == ff.unique(Equal() ).getDataCount() ),
                     "one item expected", __LINE__ );

      tester.assert( (2 == ff.front().value), "unexpected value", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception for non-empty list", __LINE__ );
   }

   try   // multiple-item data list
   {
      FileFilter<FFTData>  ff;
      FFTDataList  list;
      list.push_back( FFTData(1) );
      list.push_back( FFTData(2) );
      list.push_back( FFTData(2) );
      list.push_back( FFTData(2) );
      list.push_back( FFTData(3) );
      ff.addData(list);

      tester.assert( (3 == ff.unique(Equal() ).getDataCount() ),
                     "three items expected", __LINE__ );

      tester.assert( (1 == ff.front().value), "unexpected value", __LINE__ );
      tester.assert( (3 == ff.back().value), "unexpected value", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception for non-empty list", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileFilter_T :: testDiff()
{
   TestUtil  tester( "FileFilter", "diff", __FILE__, __LINE__ );

   try   // empty data lists
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;

      std::pair< FFTDataList, FFTDataList >  results = ff.diff(ffOther, LessThan() );

      tester.assert( ( (0 == results.first.size() ) && (0 == results.second.size() ) ),
                     "empty lists expected", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception diffing empty lists", __LINE__ );
   }

   try   // one empty data list (first)
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;
      FFTDataList  list;
      list.push_back( FFTData(1) );
      ffOther.addData(list);

      std::pair< FFTDataList, FFTDataList >  results = ff.diff(ffOther, LessThan() );

      tester.assert( ( (0 == results.first.size() ) && (1 == results.second.size() ) ),
                     "unexpected element count(s)", __LINE__ );

      tester.assert( ( (1 == results.second.size() ) && (1 == results.second.front().value) ),
                     "unexpected value", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception diffing lists", __LINE__ );
   }

   try   // one empty data list (second)
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;
      FFTDataList  list;
      list.push_back( FFTData(1) );
      ff.addData(list);

      std::pair< FFTDataList, FFTDataList >  results = ff.diff(ffOther, LessThan() );

      tester.assert( ( (1 == results.first.size() ) && (0 == results.second.size() ) ),
                     "unexpected element count(s)", __LINE__ );

      tester.assert( ( (1 == results.first.size() ) && (1 == results.first.front().value) ),
                     "unexpected value", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception diffing lists", __LINE__ );
   }

   try   // two identical non-empty data lists
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;
      FFTDataList  list;
      list.push_back( FFTData(1) );
      ff.addData(list);
      ffOther.addData(list);

      std::pair< FFTDataList, FFTDataList >  results = ff.diff(ffOther, LessThan() );

      tester.assert( ( (0 == results.first.size() ) && (0 == results.second.size() ) ),
                     "empty lists expected", __LINE__ );

      if ( (0 != results.first.size() ) || (0 != results.second.size() ) )
      {
         dump(results.first);
         dump(results.second);
      }
   }
   catch (...)
   {
      tester.assert( false, "exception diffing lists", __LINE__ );
   }

   try   // two non-identical non-empty data lists
   {
      FileFilter<FFTData>  ff;
      FileFilter<FFTData>  ffOther;
      FFTDataList  list;
      list.push_back( FFTData(1) );
      ff.addData(list);
      list.clear();
      list.push_back( FFTData(2) );
      ffOther.addData(list);

      std::pair< FFTDataList, FFTDataList >  results = ff.diff(ffOther, LessThan() );

      tester.assert( ( (1 == results.first.size() ) && (1 == results.second.size() ) ),
                     "expected element count(s)", __LINE__ );

      tester.assert( ( (1 == results.first.size() ) && (1 == results.first.front().value) ),
                     "unexpected value", __LINE__ );

      tester.assert( ( (1 == results.second.size() ) && (2 == results.second.front().value) ),
                     "unexpected value", __LINE__ );

      if ( (1 != results.first.size() ) || (1 != results.second.size() ) )
      {
         dump(results.first);
         dump(results.second);
      }
   }
   catch (...)
   {
      tester.assert( false, "exception diffing lists", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
int FileFilter_T :: testFindAll()
{
   TestUtil  tester( "FileFilter", "findAll", __FILE__, __LINE__ );

   try   // empty data list
   {
      FileFilter<FFTData>  ff;

      FFTDataList  fftdList = ff.findAll(IsEven() );
      tester.assert( (0 == fftdList.size() ),
                     "empty list expected", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception searching empty list", __LINE__ );
   }

   try   // non-empty list, no matches
   {
      FileFilter<FFTData>  ff;
      ff.addData( sampleData );

      FFTDataList  fftdList = ff.findAll(Above999() );
      tester.assert( (0 == fftdList.size() ),
                     "empty list expected", __LINE__ );

   }
   catch (...)
   {
      tester.assert( false, "exception searching non-empty list", __LINE__ );
   }

   try   // non-empty list, several matches
   {
      FileFilter<FFTData>  ff;
      ff.addData( sampleData );

      FFTDataList  fftdList = ff.findAll(IsEven() );
      tester.assert( (50 == fftdList.size() ),
                     "50 element list expected", __LINE__ );
   }
   catch (...)
   {
      tester.assert( false, "exception searching non-empty list", __LINE__ );
   }

   return tester.countFails();
}


//---------------------------------------------------------------------------
void FileFilter_T :: dump(const FFTDataList& data)
{
   cout << "  DATA DUMP:" << endl;
   FFTDataList::const_iterator  iter = data.begin();
   for ( ; iter != data.end(); ++iter)
      cout << "    " << iter->value << endl;
}


 /** Initialize and run all tests.
  *
  * @return Total error count for all tests
  */
int main(int argc, char *argv[])
{
   int  errorTotal = 0;

   FileFilter_T  testClass;

   errorTotal += testClass.testInitialization();
   errorTotal += testClass.testData();
   errorTotal += testClass.testBeginEnd();
   errorTotal += testClass.testFrontBack();
   errorTotal += testClass.testFilter();
   errorTotal += testClass.testSort();
   errorTotal += testClass.testMerge();
   errorTotal += testClass.testTouch();
   errorTotal += testClass.testUnique();
   errorTotal += testClass.testDiff();
   errorTotal += testClass.testFindAll();
   
   cout << "Total Failures for " << __FILE__ << ": " << errorTotal << endl;

   return errorTotal;
}
