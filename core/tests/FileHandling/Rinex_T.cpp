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

#include "RinexNavData.hpp"
#include "TestUtil.hpp"
#include <sstream>

using namespace std;
using namespace gpstk;

class Rinex_T
{

public:

   Rinex_T()
   {
      init();
   }

   ~Rinex_T() {}

   void init( void );
   int run( void );

private:

   EngEphemeris ee_orig;
   EngEphemeris ee_copy;
   RinexNavData rnd;

   static const short weeknum;
   static const uint32_t  subframe1[10];
   static const uint32_t  subframe2[10];
   static const uint32_t  subframe3[10];
};

//------------------------------------------------------------
// Inialization of static class data members
//------------------------------------------------------------

// By rules of Kepler Orbit, this must be week of Toe
const short Rinex_T::weeknum   = 1638;

// Test data from 06/02/2011
const uint32_t Rinex_T::subframe1[10] =
{ 0x22C2663D, 0x1F0E29B8, 0x2664002B, 0x09FCC1B6, 0x0F60EB8A,
  0x1299CE93, 0x29CD3DB6, 0x0597BB0F, 0x00000B68, 0x17B28E5C };
const uint32_t Rinex_T::subframe2[10] =
{ 0x22C2663D, 0x1F0E4A28, 0x05809675, 0x0EBD8AF1, 0x00089344,
  0x008081F8, 0x1330CC2C, 0x0461E855, 0x034F8045, 0x17BB1E68 };
const uint32_t Rinex_T::subframe3[10] =
{ 0x22C2663D, 0x1F0E6BA0, 0x3FE129CD, 0x26E31837, 0x0006C96A,
  0x35A74DFC, 0x065C8B0F, 0x1E4F400A, 0x3FE8966D, 0x05860C44 };

//------------------------------------------------------------
// Rinext_T::init()
//------------------------------------------------------------
void Rinex_T :: init( void )
{
      // ...in contemplation of the meaning of life.
}

//------------------------------------------------------------
// Rinext_T::run()
//------------------------------------------------------------
int Rinex_T :: run( void )
{

   TestUtil testFramework( "Rinex", "run", __FILE__, __LINE__ );

   std::stringstream before;
   std::stringstream after;
   std::string       test_desc =
      "Test equality of data dumps for two data types with the same data.";
   std::string       test_fail = "These should be equal but they are not.";

      //The dump from ee_orig.dump() and ee_copy.dump() should be the same
      //    an EngEphemeris object is created, then used to create a RinexNavData

   ee_orig.addSubframe( subframe1, weeknum, 3, 1 );
   ee_orig.addSubframe( subframe2, weeknum, 3, 1 );
   ee_orig.addSubframe( subframe3, weeknum, 3, 1 );
   ee_orig.setFIC(false); // make the dump text look the same
   ee_orig.dump( before );

   RinexNavData rnd = RinexNavData( ee_orig ); //constructor

   ee_copy = EngEphemeris( rnd ); //cast
   ee_copy.setFIC(false); // make the dump text look the same
   ee_copy.dump( after );

   TUASSERTE( std::string, before.str(), after.str() );
   return( testFramework.countFails() );
}

//------------------------------------------------------------
// main()
//------------------------------------------------------------
int main( void )
{
   int check, errorCounter = 0;
   Rinex_T testClass;
   check = testClass.run();
   errorCounter += check;

   std::cout << "Total Failures for " << __FILE__ << ": " << errorCounter <<
             std::endl;

   return errorCounter; //Return the total number of errors
}
