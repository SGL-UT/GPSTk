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

#include <iostream>
#include <string>

#include "Exception.hpp"

/**
 * @file exceptiontest.cpp
 * tests gpslib::Exception
 */

using namespace std;

class TestException
{
public:
   NEW_EXCEPTION_CLASS(TE, gpstk::Exception);

   int foo;

   void funcie() throw(TE)
   {
      try {
         foo=1;
         if (foo-1==0)
         {
            TE e("Blarfo");
            e.addLocation(FILE_LOCATION);
            throw e;
         }
         foo=4;
      } catch (TE& e) {
         e.addText("Ja mon");
            e.addLocation(FILE_LOCATION);
         throw;
      }
   }
};

// returns 0 if all tests pass
int main()
{
   TestException a;
   try {
      a.funcie();
   } catch (gpstk::Exception& e) {
      cout << "main " << e;
   }
}
