#pragma ident "$Id$"
/*******************************************************************************/
/* This program reads in clock phase data provided by any of the phase parsers */
/*  and calculates the autocorrelation function, outputting the the alpha value*/
/*  to the standard output in a format that can be used by allanplot to view   */
/*  the data.                                                                  */
/*                                                                             */
/* Written by Alex Hu                                                          */
/* Last Updated July 18, 2008                                                  */
/*******************************************************************************/

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
//  Copyright 2008, The University of Texas at Austin
//
//============================================================================

#include <iostream>
#include <vector>

#include <stdio.h>
#include <string.h>
#include <math.h>

using namespace std;

int main()
{
    // Structures used to store time and clock phase information
    vector <double> timeArray;
    vector <double> phaseArray;
    long double time, phase;
    long unsigned int numPoints, i;

    // All of the time and clock phase data is read in from the standard input
    i = 0;
    while(!feof(stdin))
    {
        cin >> time >> phase;

        timeArray.resize(i+1);
        phaseArray.resize(i+1);

        timeArray[i] = time;
        phaseArray[i] = phase;
        i++;
    }

    /*************
    *************/

    // Variables used in Overlapping Allan Deviation calculations
    double r, Tau0, variance;
    int N=i-1, k;

    // Ensures there are at least the minimum number of points required to do calculations
    if(N > 1) Tau0 = timeArray[1] - timeArray[0];
    else
    {
        cout << "Not Enough Points to Calculate Tau0" << endl;
        Tau0;
    }

    //Calculates Average for the phase data
    double avg = 0.0;
    for(i = 0; i < N; i++)
    {
        avg += phaseArray[i];
    }
    avg /= N;

    //Calculates Variance for the phase data
    for(i=0; i<N; i++)
    {
        variance += (phaseArray[i]-avg)*(phaseArray[i]-avg);
        i++;
    }

    //Autocorrelation calculation is done here
    for(k=0; k<(N/2)-1; k++) //Maximum lag = half the length of data
    {
        double top=0;
        for(i=0; i<N-k; i++)
        {
            top += (phaseArray[i]-avg)*(phaseArray[i+k]-avg);
        }
        r = top/variance;

        fprintf(stdout, "%i %f \n", k, r/2);
    }

    return(0);
}
