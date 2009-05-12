#pragma ident "$Id$"
/**********************************************
/ GPSTk: Clock Tools
/ nallandev.cpp
/ 
/ Computes the normal Allan deviation
/ (reference)
/ 
/ Written by Alex Hu
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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//  Copyright 2009, The University of Texas at Austin
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
		if(feof(stdin)) break;
		timeArray.resize(i+1);
		phaseArray.resize(i+1);
		timeArray[i] = time;
		phaseArray[i] = phase;
		i++;
	}
	
	numPoints = i-1;
	
	// Variables used in Overlapping Allan Deviation calculations
	double Tau, Tau0, sum, sigma;
	int N, m;
	
	// Ensures there are at least the minimum number of points required to do calculations
	N = numPoints;
	if(N > 1) Tau0 = timeArray[1] - timeArray[0];
	else
	{
		cout << "Not Enough Points to Calculate Tau0" << endl;
		Tau0;
	}
	
	// Actual Allan Deviation Calculation is done here
	// The Allan Deviation is calculated as follows
	//  Sigma^2(Tau) = 1 / (2*(N-2)*Tau^2) * Sum(X[i*m+2]-2*X[i*m+1]+X[i*m], i=1, i= (N-2)/m)
	//  Where Tau is the averaging time, N is the total number of points, and Tau = m*Tau0
	//  Where Tau0 is the basic measurement interval
	
	//i iterates from 1 to N-2; is multiples of m, Tau = m*Tau0
        for(m = 1; m<(N-1)/2; m++)
        {
                int x=0;
		Tau = m*Tau0;
		sigma = 0;
		
		for(i = 0; i < N-2*m; i=i+m)
		{
			sum = 0;
                        sum = phaseArray[i+2*m] - 2*phaseArray[i+1*m] + phaseArray[i];
			sigma += sum*sum;
			x++;
		}
		
		sigma = sigma / (2.0*((double)x)*Tau*Tau);
		sigma = sqrt(sigma);
		fprintf(stdout, "%.1f %.4e \n", Tau, sigma); // outputs results to the standard output
	}
	/*************
	*************/

	return(0);
}
