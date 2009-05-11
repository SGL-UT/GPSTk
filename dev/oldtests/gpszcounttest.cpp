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

/**
 * @file gpszcounttest.cpp
 * A set of tests for the GPSZcount class.
 * tests gpslib::GPSZcount
 */

#include <limits>
#include "GPSZcount.hpp"
#include "CommandOptionParser.hpp"

using namespace std;
using namespace gpstk;

bool failure = false;
int verboseLevel = 0;
void say(const string& foo);
void process(bool result);

//
// returns zero if all tests pass
//
int main (int argc, char *argv[])
{
      // take care of command line options
   CommandOptionNoArg hOption('h', "help", "Display this helpful information.",
                              false);
   CommandOptionNoArg vOption('v', "verbose", "Increase the verbosity.", 
                              false);
   CommandOptionNoArg qOption('q', "quiet", "Only issue a return code.", 
                              false);
   CommandOptionParser cop("GPSZcount Test Suite");
   cop.parseOptions(argc, argv);
   if (cop.hasErrors())
   {
      cop.dumpErrors(cout);
      cop.displayUsage(cout);
      return 1;
   }
   
   if(hOption.getCount())
   {
      cop.displayUsage(cout);
      return 0;
   }
   
   verboseLevel = vOption.getCount();
   
   try
   {
         // Proceed with testing
      GPSZcount one, two;
      
      say("Equality Test: ");
      process (one == two);
      
      say("Assignment Test 1: ");
      one = GPSZcount(1000, 5);
      process(one.getWeek() == 1000 &&
              one.getZcount() == 5);
      
      say("Assignment Test 2: ");
      try
      {
            // this should throw an exception
         one.setZcount(GPSZcount::ZCOUNT_WEEK);
            // if we get here, it failed
         process(false);
      }
      catch (InvalidParameter& ip)
      {
         process(true);
      }
      
      say("Addition Test: ");
      one += GPSZcount::ZCOUNT_WEEK + 5 ;
      process(one.getWeek() == 1001 &&
              one.getZcount() == 10) ;
      
      say("Subtraction Test: ");
      one -= GPSZcount::ZCOUNT_WEEK + 5 ;
      process(one.getWeek() == 1000 &&
              one.getZcount() == 5) ;
      
      say("Invalid Week Test 1: ");
      one.setWeek(numeric_limits<short>::max());
      try
      {
            // this should throw an exception
         one += GPSZcount::ZCOUNT_WEEK;
            // if we get here, it failed
         process(false); 
      }
      catch (InvalidRequest& ir)
      {
         process(true);
      }
      
      say("Invalid Week Test 2: " );
      one.setWeek(0);
      try
      {
            // this should throw an exception
         one -= GPSZcount::ZCOUNT_WEEK;
            // if we get here, it failed
         process(false);
      }
      catch (InvalidRequest& ir)
      {
         process(true);
      }
      
      say("Less-Than Test: ");
      one.setWeek(1000).setZcount(GPSZcount::ZCOUNT_WEEK / 2) ;
      two = one + 5 ;
      process(two >= one) ;
      
      say("Greater-Than Test: ") ;
      two = one - 5 ;
      process(two <= one) ;

      say("SameTimeBlock Test 1: ") ;
         // set one to be at 4:20 and two at 4:00
      one.setWeek(1200).setZcount(4 * GPSZcount::ZCOUNT_HOUR 
                                  + 20 * GPSZcount::ZCOUNT_MINUTE) ;
      two.setWeek(1200).setZcount(4 * GPSZcount::ZCOUNT_HOUR) ;
         // test if one and two are between 4:00 and 5:00
      process(one.inSameTimeBlock(two, GPSZcount::ZCOUNT_HOUR)) ;
      
      say("SameTimeBlock Test 2: ") ;
         // test if one and two are NOT between 4:20 and 4:21
      process(! one.inSameTimeBlock(two, GPSZcount::ZCOUNT_MINUTE)) ;

      say("SameTimeBlock Test 3: ") ;
         // test if one and two are NOT between 4:15 and 5:15
      process(! one.inSameTimeBlock(two, GPSZcount::ZCOUNT_HOUR, 
                                    15 * GPSZcount::ZCOUNT_MINUTE)) ;
      
      say("Dump Test: ") ;
      if (verboseLevel)
      {
         cout << endl << one << endl;
         one.dump(cout, 1);
      }

      say("String Test: ");
      one.setWeek(1200).setZcount(123456);
      process(string(one) == string("1200w123456z"));
      
      if (qOption.getCount() == 0)
      {
            // Display the overall results
         cout << "GPSZcount Overall results: " 
              << (failure ? "Fail" : "Pass") << endl;
      }
      
      return failure ? 1 : 0 ;
   }
   catch(gpstk::Exception& exc)
   {
      cout << endl << endl << "Caught a " << exc.getName() << " exception:" 
           << endl << exc << endl ;
      return 1 ;
   }
}

void say(const string& foo)
{
   if (verboseLevel)
      cout << foo << flush;
}

void process(bool result)
{
   failure |= !result;

   if(verboseLevel)
   {
      cout << (result ? "Pass" : "Fail")  << endl;
   }
}
