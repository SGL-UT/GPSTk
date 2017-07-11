#pragma ident "$Id$"
/**********************************************
/ GPSTk: Clock Tools
/ pff.cpp
/
/ Converts phase data to fractional frequency data
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
#include <string>

#include <stdio.h>

using namespace std;

int main(int argv, char **argc)
{
    if(argc[1])
    {
        string str = argc[1];
        if((str == "-h") || (str == "--help"))
        {
          cout << "pff: Converts phase data to fractional frequency data from the standard input." << endl << "Input lines must be of the form 'Time phase'" << endl << endl << "Usage: cat phase.dat | pff > fracfreq.dat" << endl << endl;
          return 1;
        }
    }
    double dataA[2];
    double dataB[2];
    unsigned long int i = 0;

    double ff;

    // inputs phase data from the standard input
    while(!feof(stdin))
    {
        if(i != 0)
        {
            dataB[0] = dataA[0];
            dataB[1] = dataA[1];
        }

        cin >> dataA[0] >> dataA[1];
        if(feof(stdin)) break;

        // esnure at least 2 data points are known
        if(i != 0)
        {
            // convert phase to fractional frequency, output data
            ff = (dataA[1] - dataB[1])/(dataA[0]-dataB[0]);
            printf("%.1f %.15f\n", dataB[0], ff);
        }

        i++;
    }

    return(0);
}
