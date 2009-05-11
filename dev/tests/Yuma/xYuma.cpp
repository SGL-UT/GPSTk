/*********************************************************************
*  $Id$
*
*  Test program from November 2006.  Written to test the YumaAlmRecord.cpp
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
//============================================================================

#include "YumaData.hpp"
#include "YumaStream.hpp"
#include "YumaBase.hpp"
#include "SMODFData.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
      // Read an existing Yuma almanac file and write it back out.
   try
   {
   YumaStream In("yuma377.txt");
   YumaStream Out("yuma377.dbg", ios::out);
   YumaData Data;
   
   while (In >> Data)
   {
      Out << Data;
   }
   
   YumaStream In2("yuma377.dbg");
   YumaStream Out2("yuma377_2.dbg", ios::out);
   
   while (In2 >> Data)
   {
      Out2 << Data;
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
