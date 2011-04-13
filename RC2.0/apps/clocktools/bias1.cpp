#pragma ident "$Id$"
/*******************************************************************************/
/* This program reads in clock phase data provided by any of the phase parsers */
/*  and calculates the Bias Function, outputting the results to                */
/*  the standard output.                                                       */
/*                                                                             */
/* Written by Alex Hu                                                          */
/* Last Updated July 3, 2008                                                  */
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
//  Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
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
	
	numPoints = i-1;
      
	/*************
	*************/
	
	//Constructs table containing bias function values for mu indexing
	double a[21][9] =
	{
		{ 0.8333, 0.75,  0.7083, 0.6875, 0.6771, 0.6719, 0.6693, 0.668,  0.6673 },
		{ 0.8581, 0.7827,0.7431, 0.7226, 0.7122, 0.7068, 0.7042, 0.7028, 0.7021 },
		{ 0.8866, 0.8221,0.7864, 0.7672, 0.757,  0.7517, 0.749,  0.7476, 0.7468 },
		{ 0.9193, 0.87,  0.841,  0.8245, 0.8154, 0.8105, 0.8079, 0.8065, 0.8058 },
		{ 0.9569, 0.9284,0.9105, 0.8997, 0.8933, 0.8897, 0.8877, .8866,  0.86 },
		{ 1,      1,     1,      1,      1,      1,      1,      1,      1 },
		{ 1.05,   1.088, 1.117,  1.137,  1.5,    1.16,   1.165,  1.169,  1.171 },
		{ 1.107,  1.197, 1.2471, 1.327,  1.37,   1.401,  1.422,  1.438,  1.448 },
		{ 1.172,  1.333, 1.476,  1.599,  1.7,    1.782,  1.847,  1.898,  1.938 },
		{ 1.247,  1.502, 1.754,  1.994,  2.216,  2.418,  2.599,  2.759,  2.9 },
		{ 1.333,  1.714, 2.133,  2.581,  3.048,  35.28,  4.016,  4.509,  5.005 },
		{ 1.432,  1.982, 2.658,  3.471,  4.432,  5.555,  6.858,  8.363,  10.1 },
		{ 1.546,  2.32,  3.391,  4.846,  6.801,  9.407,  12.87,  17.44,  23.5 },
		{ 1.677,  2.75,  4.424,  7.006,  10.96,  16.98,  26.14,  40.05,  61.14 },
		{ 1.827,  3.299, 5.894,  10.45,  18.41,  32.3,   56.52,  98.72,  172.2 },
		{ 2,      4,     8,      16,     32,     64,     128,    256,    512 },
		{ 2.198,  4.9,   11.04,  25.06,  17.17,  130.8,  299.9,  688.1,  1580 },
		{ 2.426,  6.059, 15.46,  39.99,  104.4,  273.8,  720.2,  1897,   5003 },
		{ 2.688,  7.555, 21.91,  64.79,  193.8,  583.3,  1762,   5331,   16150 },
		{ 2.988,  9.49,  30.38,  106.3,  364.6,  1260,   4372,   15190,  52860 },
		{ 3.333,  1.2,   45.33,  176,    693.3,  2752,   10970,  43780,  174900 }
	};
	
	// Variables used in Overlapping Allan Deviation calculations
	double Tau, Tau0, sum, sigma, N = numPoints;
	int m=1; 
	
	// Ensures there are at least the minimum number of points required to do calculations
	if(numPoints > 1) Tau0 = timeArray[1] - timeArray[0];
	else
	{
		cout << "Not Enough Points to Calculate Tau0" << endl;
		Tau0;
	} 
		//Calculates Standard Deviation- converts phase data into frequency data to calculate standard deviation
		
	for(m = 1; m<N/2; m++)
	{	
		double avg = 0.0;
		for(i = 0; i < N-1; i=i+m) 
		{
			avg += (phaseArray[i+m]-phaseArray[i])/(Tau0*m);
		}
		avg /= i/m;

		double stddev = 0.0;
		for(i = 0; i < N-m; i=i+m) 
		{
			stddev += ((phaseArray[i+m]-phaseArray[i])/(Tau0*m)-avg)*((phaseArray[i+m]-phaseArray[i])/(Tau0*m)-avg);
		}
	
		stddev /= double(i/m)-1;
		//stddev = sqrt(stddev);
		
		//Calculates Allan Variance where T/tau = 1
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
		//sigma = sqrt(sigma);
		double b1 = stddev/sigma;
	
	//Looks up calculated bias function in the table and returns alpha for noise identification	
	int col;
	x += 2;
	if(x>767) {col=8;}
	else if(x>383) {col=7;}
	else if(x>191) {col=6;}
	else if(x>95) {col=5;}
	else if(x>47) {col=4;}
	else if(x>23) {col=3;}
	else if(x>11) {col=2;}
	else if(x>5) {col=1;}
	else {col=0;}
	
	int v=0;
	int alpha;
	while(a[v][col]<b1 && v<20) {v++;}
	alpha = (-1.0*((double)v/5.0-2.0)-1.0)+.5;  
	
	fprintf(stdout, "%i %i\n", m, alpha);
	//fprintf(stdout, "Alpha %i, B1 %.4e, Stddev %.8e, Sigma %.4e\n", alpha, b1, stddev, sqrt(sigma));
	}
	return(0);
}
