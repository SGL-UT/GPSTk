#pragma ident "$Id$"
/**********************************************
/ GPSTk: Clock Tools
/ ffp.cpp
/
/ Converts fractional frequency data to phase data
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
          cout << "ffp: Converts fractional frequency data to phase data from the standard input." << endl << "Input lines must be of the form 'Time fractional-frequency'" << endl << endl << "Usage: cat fracfreq.dat | ffp > phase.dat" << endl << endl;
          return 1;
        }
    }

    double dataA[2] = {0.0};
    double dataB[2];
    unsigned long int i = 0;

    double x0=0.0;

    // input time tag & phase data from the standard input
    while(!feof(stdin))
    {
        dataB[0] = dataA[0];
        dataB[1] = dataA[1];

        cin >> dataA[0] >> dataA[1];
        if(feof(stdin)) break;

        // check that two points have been captured before conversion
        if(i != 0)
        {
            // convert fractional frequency to phase point
            x0 = dataB[1]*(dataA[0]-dataB[0]) + x0;
            printf("%.0f %.20f\n", dataA[0], x0);
        }

        i++;
    }

    return(0);
}
