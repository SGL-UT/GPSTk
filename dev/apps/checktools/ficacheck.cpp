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
#include <iostream>
#include <fstream>
#include "FICAStream.hpp"
#include "FICData.hpp"

using namespace std;
using namespace gpstk;

int main(int argc, char* argv[])
{
   try
   {
      CheckFrame<FICAStream, FICData> cf(argv[0], std::string("FIC ASCII"));
      ofstream out;
      out.open ("out.txt");
      if (!cf.initialize(argc, argv))
      {       
	out << "here 1" << endl;
	return 0;
      }

      out << "here 2" << endl;

      if (!cf.run())
      {
	out << "here 3" << endl;
	return 1;
      } 
      
      return 0;   
      out.close();
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
