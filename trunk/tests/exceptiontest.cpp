#pragma ident "$Id: //depot/sgl/gpstk/dev/tests/exceptiontest.cpp#2 $"

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
