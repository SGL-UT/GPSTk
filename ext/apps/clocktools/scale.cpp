#pragma ident "$Id$"
/**********************************************
/ GPSTk: Clock Tools
/ scale.cpp
/
/ Scales kalman phase data for use with the Clock Tools.
/ (reference)
/
/ Written by Timothy J.H. Craddock (tjhcraddock@gmail.com)
/ Last Updated: Dec. 11, 2008
**********************************************/

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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================


#include <iostream>
#include <stdio.h>
#include <string>

using namespace std;

int main(int argv, char **argc)
{
    if(argc[1])
    {
        string str = argc[1];
        if((str == "-h") || (str == "--help"))
        {
          cout << "scale: Scales kalman phase data for use with the Clock Tools." << endl;
          return 1;
        }
    }
    double data[2];

    // input data from the standard input
    while(!feof(stdin))
    {
        cin >> data[0] >> data[1];
        if(feof(stdin)) break;

        // scale and output data
        printf("%.1f %.15f\n", data[0], data[1]*0.000001);
    }

    return(0);
}
