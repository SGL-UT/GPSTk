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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2006, The University of Texas at Austin
//
//============================================================================

#include "GPSZcount.hpp"
#include "TimeTag.hpp"
#include "TestUtil.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <sstream>

class GPSZcount_T
{
public:
   GPSZcount_T() {eps = 1E-12;}
   ~GPSZcount_T() {}
//=============================================================================
//      initializationTest ensures the constructors set the values properly
//=============================================================================
   int  initializationTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "ConstructorExplicit2Inputs", __FILE__, __LINE__ );

         //--------------------------------------------------------------------
         // Were the values set to expectation using the explicit
         // constructor with week and zcount inputs?
         //--------------------------------------------------------------------
         // Initialize a valid GPSZcount object
      try
      {
         gpstk::GPSZcount validObject(10, 35); 
         testFramework.assert(true, "Valid object constructed", __LINE__);
         testFramework.assert(validObject.getWeek() == 10  , "Explicit constructor could not set week properly"  , __LINE__);
         testFramework.assert(validObject.getZcount() == 35, "Explicit constructor could not set zcount properly", __LINE__);
         testFramework.assert(validObject.getFullZcount() == 5242915  , "getFullZcount did not return the expected value" , __LINE__);
         testFramework.assert(validObject.getTotalZcounts() == 4032035, "getTotalZcount did not return the expected value", __LINE__);
      }
      catch(...){testFramework.assert(false, "Valid object could not be constructed", __LINE__);}

         // Try to create an invalid week object (week < 0)
      try
      {
         gpstk::GPSZcount invalidWeek(-10, 35); 
         testFramework.assert(false, "Invalid week object could be constructed", __LINE__);
      }
      catch(gpstk::InvalidParameter e){testFramework.assert(true, "Expected exception caught", __LINE__);}
      catch(...){testFramework.assert(false, "Unexpected exception caught for creation of invalid week GPSZcount", __LINE__);}

         // Try to create an invalid zcount object with too high of a value (zcount > 403200)
      try
      {
         gpstk::GPSZcount invalidZcountHigh(10, 999999);
         testFramework.assert(false, "Invalid zcount object could be constructed", __LINE__);
      }
      catch(gpstk::InvalidParameter e){testFramework.assert(true, "Expected exception caught", __LINE__);}
      catch(...){testFramework.assert(false, "Unexpected exception caught for creation of invalid zcount GPSZcount", __LINE__);}


         // Try to create an invalid zcount object with too low of a value (zcount < 0)
      try
      {
         gpstk::GPSZcount invalidZcountHigh(10, -999999);
         testFramework.assert(false, "Invalid zcount object could be constructed", __LINE__);
      }
      catch(gpstk::InvalidParameter e){testFramework.assert(true, "Expected exception caught", __LINE__);}
      catch(...){testFramework.assert(false, "Unexpected exception caught for creation of invalid zcount GPSZcount", __LINE__);}



         //--------------------------------------------------------------------
         // Were the values set to expectation using the explicit
         // constructor with FullZcount input?
         //--------------------------------------------------------------------
      testFramework.changeSourceMethod("ConstructorExplicit1Input");
         // Initialize a valid GPSZcount object
      try
      {
         int inputFullZcount = 1211600; // = 403200*3+2000 = 3 weeks 2000 zcount 
         gpstk::GPSZcount validObject(inputFullZcount); 
         testFramework.assert(true, "Valid object constructed", __LINE__);
         testFramework.assert(validObject.getWeek() == 2       , "Explicit constructor could not set week properly"  , __LINE__);
         testFramework.assert(validObject.getZcount() == 163024, "Explicit constructor could not set zcount properly", __LINE__);
         testFramework.assert(validObject.getFullZcount() == 1211600 , "getFullZcount did not return the expected value" , __LINE__);
         testFramework.assert(validObject.getTotalZcounts() == 969424, "getTotalZcount did not return the expected value", __LINE__);
      }
      catch(...){testFramework.assert(false, "Valid object could not be constructed", __LINE__);}       

         // Try to create an invalid zcount object with too high of a value (403200+n*2^(20) <= FullZCount <= 524287+n*2^(20) for all non-negative integers n)
      try
      {
         gpstk::GPSZcount invalidZcountHigh(3548928);
         testFramework.assert(false, "Invalid FullZCount object could be constructed", __LINE__);
      }
      catch(gpstk::InvalidParameter e){testFramework.assert(true, "Expected exception caught", __LINE__);}
      catch(...){testFramework.assert(false, "Unexpected exception caught for creation of invalid FullZCount GPSZcount", __LINE__);}    


         //--------------------------------------------------------------------
         // Were the values set to expectation using the copy constructor?
         //--------------------------------------------------------------------
      testFramework.changeSourceMethod("ConstructorCopy");
         // Initialize a valid GPSZcount object
      try
      {
         gpstk::GPSZcount validObject(10, 35); 
         gpstk::GPSZcount Copy(validObject);
         testFramework.assert(true, "Valid object constructed", __LINE__);
         testFramework.assert(Copy.getWeek() == 10  , "Copy constructor could not set week properly"  , __LINE__);
         testFramework.assert(Copy.getZcount() == 35, "Copy constructor could not set zcount properly", __LINE__);
      }
      catch(...){testFramework.assert(false, "Copy constructor could not copy a valid GPSZcount object", __LINE__);}

         //--------------------------------------------------------------------
         // Were the values set to expectation using the set operator?
         //--------------------------------------------------------------------
      testFramework.changeSourceMethod("OperatorSet");
         // Initialize a valid GPSZcount object
      try
      {
         gpstk::GPSZcount validObject(10, 35); 
         gpstk::GPSZcount Copy(11,25);
         Copy = validObject;
         testFramework.assert(true, "Valid object constructed", __LINE__);
         testFramework.assert(Copy.getWeek() == 10  , "Set Operator could not set week properly"  , __LINE__);
         testFramework.assert(Copy.getZcount() == 35, "Set Operator could not set zcount properly", __LINE__);
      }
      catch(...){testFramework.assert(false, "Set Operator could not copy a valid GPSZcount object", __LINE__);}


         //--------------------------------------------------------------------
         // Were the values set to expectation using the set methods?
         //--------------------------------------------------------------------
      testFramework.changeSourceMethod("set");
         // Initialize a valid GPSZcount object

      gpstk::GPSZcount validObject(10, 35); 
      validObject.setWeek(9);
      validObject.setZcount(1000);

      testFramework.assert(validObject.getWeek() == 9  , "setWeek() did not set week properly"  , __LINE__);
      testFramework.assert(validObject.getZcount() == 1000, "setZcount did not set zcount properly", __LINE__);

      try{validObject.setWeek(-100); testFramework.assert(false, "setWeek allowed an invalid week to be set", __LINE__);}
      catch(gpstk::InvalidParameter e){testFramework.assert(true, "Expected exception thrown", __LINE__);}
      catch(...){testFramework.assert(false, "Unexpected exception thrown", __LINE__);}

      try{validObject.setZcount(-100); testFramework.assert(false, "setZcount allowed an invalid zcount to be set", __LINE__);}
      catch(gpstk::InvalidParameter e){testFramework.assert(true, "Expected exception thrown", __LINE__);}
      catch(...){testFramework.assert(false, "Unexpected exception thrown", __LINE__);}

      try{validObject.setZcount(999999); testFramework.assert(false, "setZcount allowed an invalid zcount to be set", __LINE__);}
      catch(gpstk::InvalidParameter e){testFramework.assert(true, "Expected exception thrown", __LINE__);}
      catch(...){testFramework.assert(false, "Unexpected exception thrown", __LINE__);}

      return testFramework.countFails();
   }



//=============================================================================
//      addWeeksTest ensures the functionality of addWeeks
//=============================================================================
   int  addWeeksTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "addWeeks", __FILE__, __LINE__ );

         //--------------------------------------------------------------------
         // Were the values set to expectation using the addWeeks?
         //--------------------------------------------------------------------
         // Increment the week
      gpstk::GPSZcount incWeek(10, 35);
      incWeek.addWeeks(2);
      testFramework.assert(incWeek.getWeek() == 12, "addWeeks() did not set week properly"  , __LINE__);
                
         // Decrement the week
      gpstk::GPSZcount decWeek(10, 35);
      decWeek.addWeeks(-2);
      testFramework.assert(decWeek.getWeek() == 8, "addWeeks() did not set week properly"  , __LINE__);

         // Try to make the week negative 
      try
      {
         gpstk::GPSZcount invalidWeek(10, 35);
         invalidWeek.addWeeks(-12);
         testFramework.assert(false, "addWeeks() allowed for change to an invalid week"  , __LINE__);   
      }
      catch(gpstk::InvalidRequest e){testFramework.assert(true, "Expected exception thrown", __LINE__);}
      catch(...){testFramework.assert(false, "Unexpected exception thrown", __LINE__);}

      return testFramework.countFails();
   }



//=============================================================================
//      addZcountsTest ensures the functionality of addZcounts
//=============================================================================
   int  addZcountsTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "addZcounts", __FILE__, __LINE__ );
         //--------------------------------------------------------------------
         // Were the values set to expectation using the addZcounts?
         //--------------------------------------------------------------------
         // Increment the zcount
      gpstk::GPSZcount incZcount(10, 35);
      incZcount.addZcounts(2L);
      testFramework.assert(incZcount.getZcount() == 37, "addZcounts() did not set zcount properly"  , __LINE__);
                
         // Decrement the zcount
      gpstk::GPSZcount decZcount(10, 35);
      decZcount.addZcounts(-2L);
      testFramework.assert(decZcount.getZcount() == 33, "addZcounts() did not set zcount properly"  , __LINE__);

         // Roll the week forward
      gpstk::GPSZcount rollWeekF(10, 403198);
      rollWeekF.addZcounts(4L);
      testFramework.assert(rollWeekF.getWeek() == 11 , "addZcounts() did not roll week forward properly", __LINE__);
      testFramework.assert(rollWeekF.getZcount() == 2, "addZcounts() did not set zcount properly"       , __LINE__);

         // Roll the week backward
      gpstk::GPSZcount rollWeekB(10, 0);
      rollWeekB.addZcounts(-2L);
      testFramework.assert(rollWeekB.getWeek() == 9       , "addZcounts() did not roll week backward properly", __LINE__);
      testFramework.assert(rollWeekB.getZcount() == 403198, "addZcounts() did not set zcount properly"        , __LINE__);

         // Try to make the week negative 
      try
      {
         gpstk::GPSZcount invalidWeek(0, 35);
         invalidWeek.addZcounts(-42);
         testFramework.assert(false, "addZcounts() allowed for change to an invalid week"  , __LINE__); 
      }
      catch(gpstk::InvalidRequest e){testFramework.assert(true, "Expected exception thrown", __LINE__);}
      catch(...){testFramework.assert(false, "Unexpected exception thrown", __LINE__);}                                         
 
      return testFramework.countFails();
   }



//=============================================================================
//      incrementTest ensures the functionality of addWeeks
//=============================================================================
   int  incrementTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "OperatorIncrement", __FILE__, __LINE__ );

         // Increment the zcount
      gpstk::GPSZcount arbitraryObject(10, 35);
      arbitraryObject++;
      testFramework.assert(arbitraryObject.getZcount() == 36, "Postfix Increment operator did not set zcount properly"  , __LINE__);
      ++arbitraryObject;
      testFramework.assert(arbitraryObject.getZcount() == 37, "Prefix Increment operator did not set zcount properly"  , __LINE__);
                
         // Roll the week forward
      gpstk::GPSZcount rollWeekF(10, 403199);
      rollWeekF++;
      testFramework.assert(rollWeekF.getWeek() == 11 , "Postfix Increment operator did not roll week forward properly", __LINE__);
      testFramework.assert(rollWeekF.getZcount() == 0, "Postfix Increment operator did not set zcount properly"       , __LINE__);

         // Roll the week forward
      gpstk::GPSZcount rollWeekF2(10, 403199);
      ++rollWeekF2;
      testFramework.assert(rollWeekF2.getWeek() == 11 , "Postfix Increment operator did not roll week forward properly", __LINE__);
      testFramework.assert(rollWeekF2.getZcount() == 0, "Postfix Increment operator did not set zcount properly"       , __LINE__);

      return testFramework.countFails();
   }


//=============================================================================
//      decrementTest ensures the functionality of addWeeks
//=============================================================================
   int  decrementTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "OperatorDecrement", __FILE__, __LINE__ );

         // Decrement the zcount
      gpstk::GPSZcount arbitraryObject(10, 35);
      arbitraryObject--;
      testFramework.assert(arbitraryObject.getZcount() == 34, "Postfix Decrement operator did not set zcount properly"  , __LINE__);
      --arbitraryObject;
      testFramework.assert(arbitraryObject.getZcount() == 33, "Prefix Decrement operator did not set zcount properly"  , __LINE__);
                
         // Roll the week backward
      gpstk::GPSZcount rollWeekB(10, 0);
      rollWeekB--;
      testFramework.assert(rollWeekB.getWeek() == 9 , "Postfix Decrement operator did not roll week forward properly", __LINE__);
      testFramework.assert(rollWeekB.getZcount() == 403199, "Postfix Decrement operator did not set zcount properly"       , __LINE__);

         // Roll the week backward
      gpstk::GPSZcount rollWeekB2(10, 0);
      --rollWeekB2;
      testFramework.assert(rollWeekB2.getWeek() == 9 , "Postfix Decrement operator did not roll week forward properly", __LINE__);
      testFramework.assert(rollWeekB2.getZcount() == 403199, "Postfix Decrement operator did not set zcount properly"       , __LINE__);

      return testFramework.countFails();
   }



//=============================================================================
//      additionTest ensures the functionality of the addition operators
//=============================================================================
   int  additionTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "OperatorAddition", __FILE__, __LINE__ );
         // Add to the zcount
      gpstk::GPSZcount justAddition(10, 35);
      gpstk::GPSZcount additionAssign(15, 1000);
      justAddition = justAddition + 35;
      additionAssign += 4000;

      testFramework.assert(justAddition.getZcount() == 70, "Addition operator did not set zcount properly"  , __LINE__);
      testFramework.assert(additionAssign.getZcount() == 5000, "Add and assign operator did not set zcount properly"  , __LINE__);      

        
         // Roll the week forward
      gpstk::GPSZcount justAdditionRollWeekF(10, 403199);
      justAdditionRollWeekF = justAdditionRollWeekF + 1000;
      testFramework.assert(justAdditionRollWeekF.getWeek() == 11 , "Addition operator did not roll week forward properly", __LINE__);
      testFramework.assert(justAdditionRollWeekF.getZcount() == 999, "Addition operator did not set zcount properly"       , __LINE__);

      gpstk::GPSZcount additionAssignRollWeekF(4, 403199);
      additionAssignRollWeekF += 2000;
      testFramework.assert(additionAssignRollWeekF.getWeek() == 5 , "Add and assign operator did not roll week forward properly", __LINE__);
      testFramework.assert(additionAssignRollWeekF.getZcount() == 1999, "Add and assign operator did not set zcount properly"       , __LINE__);


      return testFramework.countFails();
   }


//=============================================================================
//      subtractionTest ensures the functionality of the Subtraction operators
//=============================================================================
   int  subtractionTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "OperatorSubtraction", __FILE__, __LINE__ );
         // Subtract from the zcount
      gpstk::GPSZcount justSubtraction(10, 35);
      gpstk::GPSZcount subtractionAssign(15, 1000);
      justSubtraction = justSubtraction - 15;
      subtractionAssign -= 500;

      testFramework.assert(justSubtraction.getZcount() == 20, "Subtraction operator did not set zcount properly"  , __LINE__);
      testFramework.assert(subtractionAssign.getZcount() == 500, "Subtract and assign operator did not set zcount properly"  , __LINE__);       
                
         // Roll the week backward
      gpstk::GPSZcount justSubtractionRollWeekB(10, 0);
      justSubtractionRollWeekB = justSubtractionRollWeekB - 100;
      testFramework.assert(justSubtractionRollWeekB.getWeek() == 9 , "Subtraction operator did not roll week forward properly", __LINE__);
      testFramework.assert(justSubtractionRollWeekB.getZcount() == 403100, "Subtraction operator did not set zcount properly"       , __LINE__);

         // Roll the week backward
      gpstk::GPSZcount subtractionAssignRollWeekB(2, 0);
      subtractionAssignRollWeekB -= 200;
      testFramework.assert(subtractionAssignRollWeekB.getWeek() == 1 , "Subtract and assign operator did not roll week forward properly", __LINE__);
      testFramework.assert(subtractionAssignRollWeekB.getZcount() == 403000, "Subtract and assign operator did not set zcount properly"       , __LINE__);


      return testFramework.countFails();
   }



//=============================================================================
//      differenceTest ensures the functionality of the Subtraction
//      operator between two GPSZcount objects
//=============================================================================
   int  differenceTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "OperatorDifference", __FILE__, __LINE__ );

         // Find the zcount difference between the two objects
      double diffLessThan, diffMoreThan, diffSame;

      gpstk::GPSZcount object(10, 35);
      gpstk::GPSZcount lessThanObject(4, 13);
      gpstk::GPSZcount objectCopy(object);
      gpstk::GPSZcount moreThanObject(15, 1000);

      diffLessThan = object - lessThanObject;
      diffMoreThan = object - moreThanObject;
      diffSame     = object - objectCopy;

      testFramework.assert(diffLessThan == 2419222, "Difference operator did not obtain the correct result"  , __LINE__);
      testFramework.assert(diffMoreThan == -2016965, "Difference operator did not obtain the correct result"  , __LINE__);
      testFramework.assert(diffSame     == 0, "Difference operator did not obtain the correct result"  , __LINE__);


      return testFramework.countFails();
   }


//=============================================================================
//      remainderTest ensures the functionality of the remainder operator
//=============================================================================
   int  remainderTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "OperatorRemainder", __FILE__, __LINE__ );

         // Find the zcount remainder between the object and some test values
      long remaiderWeek, remainderValue;

      gpstk::GPSZcount object(10, 25);

      remaiderWeek = object % 403200;
      remainderValue = object % 25;

      testFramework.assert(remaiderWeek == 25, "Remainder operator did not obtain the correct result"  , __LINE__);
      testFramework.assert(remainderValue == 0, "Remainder operator did not obtain the correct result"  , __LINE__);


      return testFramework.countFails();
   }


//=============================================================================
//      comparisonTest ensures the functionality of the comparison operators
//=============================================================================
   int  comparisonTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "OperatorEquivalence", __FILE__, __LINE__ );

      gpstk::GPSZcount Compare(10, 25);
      gpstk::GPSZcount LessThanWeek(9,25);
      gpstk::GPSZcount LessThanZcount(10,12);
      gpstk::GPSZcount CompareCopy(Compare);

         //--------------------------------------------------------------------
         // Does the == Operator function?
         //--------------------------------------------------------------------
      testFramework.assert(  Compare == CompareCopy,     "Equivalence operator found equivalent objects to be not equivalent",     __LINE__);
      testFramework.assert(!(Compare == LessThanWeek),   "Equivalence operator found different week objects to be equivalent",     __LINE__);
      testFramework.assert(!(Compare == LessThanZcount), "Equivalence operator found different zcount objects to be equivalent",   __LINE__);   
                

      testFramework.changeSourceMethod("OperatorNotEquivalent");
         //--------------------------------------------------------------------
         // Does the != Operator function?
         //--------------------------------------------------------------------
      testFramework.assert(  Compare != LessThanWeek,   "Not-equal operator found different week objects to be equivalent",     __LINE__);
      testFramework.assert(  Compare != LessThanZcount, "Not-equal operator found different zcount objects to be equivalent",   __LINE__);
      testFramework.assert(!(Compare != Compare),       "Not-equal operator found equivalent objects to not be equivalent",     __LINE__);
                

      testFramework.changeSourceMethod("OperatorLessThan");
         //--------------------------------------------------------------------
         // Does the < Operator function?
         //--------------------------------------------------------------------
      testFramework.assert(  LessThanWeek < Compare,    "Less-than operator found less-than week object to not be less than",   __LINE__);
      testFramework.assert(  LessThanZcount < Compare,  "Less-than operator found less-than zcount object to not be less than", __LINE__);
      testFramework.assert(!(Compare < LessThanWeek),   "Less-than operator found greater-than week object to be less than",    __LINE__);
      testFramework.assert(!(Compare < LessThanZcount), "Less-than operator found greater-than zcount object to be less than",  __LINE__);
      testFramework.assert(!(Compare < CompareCopy),    "Less-than operator found equivalent object to be less than",           __LINE__);

      testFramework.changeSourceMethod("OperatorGreaterThan");
         //--------------------------------------------------------------------
         // Does the > Operator function?
         //--------------------------------------------------------------------
      testFramework.assert(!(LessThanWeek > Compare),   "Greater-than operator found less-than week object to be greater than",          __LINE__);
      testFramework.assert(!(LessThanZcount > Compare), "Greater-than operator found less-than zcount object to be greater than",        __LINE__);
      testFramework.assert(  Compare > LessThanWeek,    "Greater-than operator found greater-than week object to not be greater than",   __LINE__);
      testFramework.assert(  Compare > LessThanZcount,  "Greater-than operator found greater-than zcount object to not be greater than", __LINE__);
      testFramework.assert(!(Compare > CompareCopy),    "Greater-than operator found equivalent object to be greater than",              __LINE__);
                

      testFramework.changeSourceMethod("OperatorLessThanOrEqualTo");
         //--------------------------------------------------------------------
         // Does the <= Operator function?
         //--------------------------------------------------------------------
      testFramework.assert(  LessThanWeek <= Compare,    "Less-than operator found less-than week object to not be less-than-or-equal-to",   __LINE__);
      testFramework.assert(  LessThanZcount <= Compare,  "Less-than operator found less-than zcount object to not be less-than-or-equal-to", __LINE__);
      testFramework.assert(!(Compare <= LessThanWeek),   "Less-than operator found greater-than week object to be less-than-or-equal-to",    __LINE__);
      testFramework.assert(!(Compare <= LessThanZcount), "Less-than operator found greater-than zcount object to be less-than-or-equal-to",  __LINE__);
      testFramework.assert(  Compare <= CompareCopy,     "Less-than operator found equivalent object to not be less-than-or-equal-to",       __LINE__);
                

      testFramework.changeSourceMethod("OperatorGreaterThanOrEqualTo");
         //--------------------------------------------------------------------
         // Does the >= Operator function?
         //--------------------------------------------------------------------
      testFramework.assert(!(LessThanWeek >= Compare),   "Greater-than-or-equal-to operator found less-than week object to be greater-than-or-equal-to",          __LINE__);
      testFramework.assert(!(LessThanZcount >= Compare), "Greater-than-or-equal-to operator found less-than zcount object to be greater-than-or-equal-to",        __LINE__);
      testFramework.assert(  Compare >= LessThanWeek,    "Greater-than-or-equal-to operator found greater-than week object to not be greater-than-or-equal-to",   __LINE__);
      testFramework.assert(  Compare >= LessThanZcount,  "Greater-than-or-equal-to operator found greater-than zcount object to not be greater-than-or-equal-to", __LINE__);
      testFramework.assert(  Compare >= CompareCopy,     "Greater-than-or-equal-to operator found equivalent object to not be greater-than-or-equal-to",          __LINE__);


      return testFramework.countFails();
   }


//=============================================================================
//      stringTest ensures the functionality of the string operator
//=============================================================================
   int  stringTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "OperatorString", __FILE__, __LINE__ );

      std::string expectedResult = "1234w56789z";
      gpstk::GPSZcount object(1234, 56789);
      std::string obtainedResult = std::string(object);

      testFramework.assert(obtainedResult == expectedResult, "string operator did not obtain the expected result", __LINE__);

      return testFramework.countFails();
   }


//=============================================================================
//      dumpTest ensures the functionality of the dump method
//=============================================================================
   int  dumpTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "dump", __FILE__, __LINE__ );

      std::stringstream obtainedOutputStreamLevel0;
      std::stringstream obtainedOutputStreamLevel1;
      std::stringstream expectedOutputStreamLevel0;
      std::stringstream expectedOutputStreamLevel1;

      expectedOutputStreamLevel0 << "1234w56789z";
      expectedOutputStreamLevel1 << "GPS Full Week: " << std::setw(6) << 1234 << std::endl
                                 << "GPS Z-count:   " << std::setw(6) << 56789 << std::endl;
                
      gpstk::GPSZcount object(1234, 56789);
      object.dump(obtainedOutputStreamLevel0, 0);
      object.dump(obtainedOutputStreamLevel1, 1);


      std::string testMessage = " did not obtain the expected result";
      testFramework.assert(obtainedOutputStreamLevel0.str() == expectedOutputStreamLevel0.str(), "dump did not obtain the expected result", __LINE__);
      testFramework.assert(obtainedOutputStreamLevel1.str() == expectedOutputStreamLevel1.str(), "dump did not obtain the expected result", __LINE__);
      return testFramework.countFails();
   }


//=============================================================================
//      inSameTimeBlockTest ensures the functionality of the
//      inSameTimeBlock method
//=============================================================================
   int inSameTimeBlockTest(void)
   {
      gpstk::TestUtil testFramework( "GPSZcount", "inSameTimeBlock", __FILE__, __LINE__ );

      gpstk::GPSZcount compare(10,0);
      gpstk::GPSZcount oneWeekMinus1(10,403199);
      gpstk::GPSZcount compareOffset(10,20);
      gpstk::GPSZcount withinWeek(10, 1000);
      gpstk::GPSZcount oneWeek(11,0);
      gpstk::GPSZcount oneWeekOffset(11,10);
      gpstk::GPSZcount moreThanWeek(12,0);
      gpstk::GPSZcount withinMinuteOffset(10,50);

      bool isWithinMinute    =  compare.inSameTimeBlock(compareOffset, gpstk::GPSZcount::ZCOUNT_MINUTE  , 0);
      bool isNotWithinMinute = !compare.inSameTimeBlock(withinWeek   , gpstk::GPSZcount::ZCOUNT_MINUTE  , 0);           
      bool isWithinWeek      =  compare.inSameTimeBlock(withinWeek   , gpstk::GPSZcount::ZCOUNT_WEEK    , 0);
      bool isNotWithinWeek   = !compare.inSameTimeBlock(oneWeekOffset, gpstk::GPSZcount::ZCOUNT_WEEK    , 0);

         // For times at 1 week, the comparison is done assuming close
         // to week = 0, which is not any of these

         // Originally this was expected to be true, but it really
         // shouldn't be.  Week 11 is not in the same week as week 10.
      bool isAtWeek          =  compare.inSameTimeBlock(oneWeek, gpstk::GPSZcount::ZCOUNT_WEEK, gpstk::GPSZcount::ZCOUNT_WEEK*10);
      bool isNearWeek        =  compare.inSameTimeBlock(oneWeekMinus1, gpstk::GPSZcount::ZCOUNT_WEEK, gpstk::GPSZcount::ZCOUNT_WEEK*10);
      bool isWithin3Weeks       = compare.inSameTimeBlock(moreThanWeek, gpstk::GPSZcount::ZCOUNT_WEEK*3, gpstk::GPSZcount::ZCOUNT_WEEK*10);

      bool isWithinMinuteOffset = compareOffset.inSameTimeBlock(withinMinuteOffset, gpstk::GPSZcount::ZCOUNT_MINUTE  , 20);
      bool isWithinWeekOffset   = compareOffset.inSameTimeBlock(oneWeekOffset     , gpstk::GPSZcount::ZCOUNT_WEEK    , 20);


      testFramework.assert(isWithinMinute      , "Returned false when object difference is 20 and timeblock is 40"                , __LINE__);
      testFramework.assert(isNotWithinMinute   , "Returned true when object difference is 1000 and timeblock is 40"               , __LINE__);          
      testFramework.assert(isWithinWeek        , "Returned false when object difference is 1000 and timeblock is 403200"          , __LINE__);
      testFramework.assert(!isAtWeek           , "Returned true when object difference is 403200 and timeblock is 403200"        , __LINE__);  
      testFramework.assert(isNearWeek          , "Returned false when object difference is 403199 and timeblock is 403200"        , __LINE__);  
      testFramework.assert(isNotWithinWeek     , "Returned true when object difference is 403210 and timeblock is 403200"         , __LINE__);  
      testFramework.assert(isWithin3Weeks      , "Returned false when object difference is 806400 and timeblock is 1209600"       , __LINE__);  
      testFramework.assert(isWithinMinuteOffset, "Returned false when object difference is 30 and shifted timeblock is 40"        , __LINE__);  
      testFramework.assert(isWithinWeekOffset  , "Returned false when object difference is 403190 and shifted timeblock is 403200", __LINE__);          


      return testFramework.countFails();
   }
private:
   double eps;
};

int main() // Main function to initialize and run all tests above
{
   int check, errorCounter = 0;
   GPSZcount_T testClass;

   check = testClass.initializationTest();
   errorCounter += check;

   check = testClass.addWeeksTest();
   errorCounter += check;

   check = testClass.addZcountsTest();
   errorCounter += check;

   check = testClass.incrementTest();
   errorCounter += check;

   check = testClass.decrementTest();
   errorCounter += check;

   check = testClass.additionTest();
   errorCounter += check;

   check = testClass.subtractionTest();
   errorCounter += check;

   check = testClass.differenceTest();
   errorCounter += check;

   check = testClass.remainderTest();
   errorCounter += check;

   check = testClass.comparisonTest();
   errorCounter += check;

   check = testClass.inSameTimeBlockTest();
   errorCounter += check;
        
   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter << std::endl;

   return errorCounter; // Return the total number of errors
}
