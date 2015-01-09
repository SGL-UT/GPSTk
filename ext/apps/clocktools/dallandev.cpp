#pragma ident "$Id$"
/**********************************************
/ GPSTk: Clock Tools
/ dallandev.cpp
/
/ Computes the dynamic Allan deviation
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
#include <vector>
#include <string>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <cstdlib>

using namespace std;

int main(int argv, char **argc)
{
     //Check to display help dialog
        if(argc[1])
    {
        string str = argc[1];
        if((str == "-h") || (str == "--help"))
        {
          cout << "dallandev: Computes the dynamic Allan deviation from time tag & phase             data sent to the standard input." << endl;
          return 1;
        }
    }

    vector <double> timeArray;
    vector <double> phaseArray;
    long double time, phase;
    long unsigned int numPoints;

    // input time tag & phase data from the standard input
    while(!feof(stdin))
    {
        cin >> time >> phase;
        if(feof(stdin)) break;
        timeArray.push_back(time);
        phaseArray.push_back(phase);
    }

    numPoints = phaseArray.size();

    long unsigned int N;
    double Tau0;

    // determine Tau0 based on time tags
    N = numPoints;
    if(N > 1) Tau0 = timeArray[1] - timeArray[0];
    else
    {
        cout << "Not Enough Points to Calculate Tau0" << endl;
    }

    long unsigned int n, k, m, Nw, ns;

    // get windowing information from command line options
    // Nw is the windowing size
    // Ns is the step size
    // if neither Nw or Ns are specified, they are computed by the code
    if(argv > 2)
    {
        Nw = atoi(argc[1]);
        ns = atoi(argc[2]);
    }
    else if(argv > 1)
    {
        Nw = atoi(argc[1]);
        ns = Nw/10;
    }
    else
    {
        Nw = N/10;
        ns = Nw/10;
    }

    double sum;

    // calculation of the dynamic Allan deviation
    // see "L. Galleani, and P. Tavella, 2008, "Identifying Nonstationary Clock Noises in Navigation Systems," International Journal of Navigation and Observation, 2008."  for further explanation
    for(n = Nw; n <= N-Nw; n += ns)
    {

        for(k = 1; k <= Nw/3; k++)
        {
            sum = 0.0;
            for(m = n-Nw/2+k; m < n+Nw/2-k-1; m++)
            {
                sum += (phaseArray[m+k]-2.0*phaseArray[m]+phaseArray[m-k])*
                       (phaseArray[m+k]-2.0*phaseArray[m]+phaseArray[m-k]);
            }
            sum = sum / (2.0*k*k*Tau0*Tau0) / (N-2.0*k);
            printf("%lu %lu %.4e\n", (n-Nw)/ns+1, k, sum);
        }
        printf("\n");
    }

    return(0);
}
