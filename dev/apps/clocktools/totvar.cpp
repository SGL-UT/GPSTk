/*******************************************************************************/
/* This program reads in clock phase data provided by any of the phase parsers */
/*  and calculates the Total Variance, outputting the results to the standard  */
/*  output in a format that can be used by allanplot to view the data.         */
/*                                                                             */
/*                                                                             */
/* Written by Tim Craddock                                                     */
/* Last Updated March 20, 2008                                                 */
/*******************************************************************************/

#include <iostream>
#include <vector>

#include <stdio.h>
#include <string.h>
#include <math.h>

using namespace std;

double Xstr(vector <double> phaseArray, int N, int index);

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
	
	// Variables used in Total Variance calculations
	double Tau, Tau0, sum, Tot;
	int N, m;
	
	// Ensures there are at least the minimum number of points required to do calculations
	N = numPoints;
	if(N > 1) Tau0 = timeArray[1] - timeArray[0];
	else
	{
		cout << "Not Enough Points to Calculate Tau0" << endl;
		Tau0;
	}
	
	// Splain tot var
	for(m = 1; m < N; m++)
	{
		Tau = m*Tau0;
		Tot = 0;
		
		for(i = 1; i < N-1; i++)
		{
			sum = 0;
			sum = Xstr(phaseArray, N, i-m) - 2*Xstr(phaseArray, N, i) + Xstr(phaseArray, N, i+m);
			Tot += sum * sum;
		}
		
		
		Tot = Tot / (2.0*Tau*Tau*((double)N-2.0));
		fprintf(stdout, "%.0f %.4e \n", Tau, Tot); // outputs results to the standard output
	}
	
	/*************
	*************/

	return(0);
}

double Xstr(vector <double> phaseArray, int N, int index)
{

	if(index < 0)
	{
		2.0*phaseArray[0]-phaseArray[abs(index)];
	}
	else if(index > (N-1))
	{
		2.0*phaseArray[N-1]-phaseArray[N-1-index];
	}
	
	return(phaseArray[index]);	
}
