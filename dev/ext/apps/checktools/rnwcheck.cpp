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
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
//
//  Copyright 2006, The University of Texas at Austin
//
//============================================================================

#include "CheckFrame.hpp"

#include "RinexNavStream.hpp"
#include "RinexNavData.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   try
   {
      CheckFrame<RinexNavStream, RinexNavData> cf(argv[0],
                                                  std::string("Rinex Nav"));
      
      if (!cf.initialize(argc, argv))
         return 0;
      if (!cf.run())
         return 1;
      
      return 0;   
   }
   catch(gpstk::Exception& e)
   {
      cout << e << endl;
   }
   catch(exception& e)
   {
      cout << e.what() << endl;
   }
   catch(...)
   {
      cout << "unknown error" << endl;
   }
   return 1;
}
