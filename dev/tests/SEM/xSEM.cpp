/*********************************************************************
*  $Id$
*
*  Test program from November 2006.  Written to test the SEMAlmRecord.cpp
*  module..
*
// *********************************************************************/

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

#include "SEMData.hpp"
#include "SEMStream.hpp"
#include "SEMHeader.hpp"
#include "SEMBase.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
      // Read an existing SEM almanac file and write it back out.
   try
   {
   SEMStream In("sem387.txt");
   SEMStream Out("sem.dbg", ios::out);
   SEMHeader Header;
   SEMData Data;
   
   In >> Header;
   Out << Header;
   while (In >> Data)
   {
      Out << Data;
   }
   
   
   }
   catch(gpstk::Exception& e)
   {
      cout << e;
      exit(1);
   }
   catch (...)
   {
      cout << "unknown error.  Done." << endl;
      exit(1);
   }

   
   return(0);
}
