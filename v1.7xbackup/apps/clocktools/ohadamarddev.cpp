#pragma ident "$Id$"
/**********************************************
/ GPSTk: Clock Tools
/ ohadamarddev.cpp
/ 
/ Computes the overlapping Hadamard deviation
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
	long unsigned int numPoints, i, numGaps;
	
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
	
	
	
	// Variables used in Overlapping Hadamard Deviation calculations
	double Tau, Tau0, sum, sigma;
	int N, m;
	
	// Ensures there are at least the minimum number of points required to do calculations
	N = numPoints;
	if(N > 2) Tau0 = timeArray[1] - timeArray[0];
	else
	{
		cout << "Not Enough Points to Calculate Tau0" << endl;
		Tau0;
	}
	
	// Overlapping Hadamard Calculation
	//  Done As Follows
	//  HSigma^2(Tau) = Sum((x[i+3m]-3x[i+2m]+3x[i+m]-x[i])^2, from i=1 to N-3m)/[6(N-3m)Tau^2]
	//  Where Tau = m*Tau0, Tau0 being the basic time interval
	//   m being the spacing, and N the total number of data points
	for(m = 1; m <= (N-1)/3; m++)
	{
		Tau = m*Tau0;
		sigma = 0;
		numGaps=0;
		for(i = 0; i < (N-3*m); i++)
		{
			sum = 0;
			if((phaseArray[i+3*m]==0 ||  phaseArray[i+2*m]==0 
			    || phaseArray[i+m]==0 || phaseArray[i]==0) 
			   && i!=0 && i!=(N-3*m-1))
			    numGaps++;
			else
			    sum = phaseArray[i+3*m] - 3*phaseArray[i+2*m] + 3*phaseArray[i+m] - phaseArray[i];
			sigma += sum * sum;
		}
		
		sigma = sigma / (6.0*((double)N-numGaps-3.0*(double)m)*Tau*Tau);
		sigma = sqrt(sigma);
		fprintf(stdout, "%.1f %.4e \n", Tau, sigma); // outputs results to the standard output
	}
	
	/*************
	*************/

	return(0);
}
