/*********************************************************************
*  $Id: xVectorBase.cpp 1896 2009-05-12 19:54:40Z rain $
*
*  Test program from November 2009.
*  Written to test the VectorBase unary operator.
*
*********************************************************************/

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

#include "Vector.hpp"

using namespace std;
using namespace gpstk;

int main( int argc, char * argv[] )
{
   // Instantiate two Vector<double> variables with default entries.
   gpstk::Vector<double> v1(6,1.0);
   gpstk::Vector<double> v2(6,0.0);

   cout << "v1 @ start = " << v1 << endl;
   cout << "v2 @ start = " << v2 << endl;
   v2 = -v1*v1;
   cout << "v2 @ mult. = " << v2 << endl;

   return(0);
}
