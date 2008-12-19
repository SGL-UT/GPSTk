/**********************************************
/ GPSTk: Clock Tools
/ mallandev.cpp
/ 
/ Computes the modified Allan deviation
/ (reference)
/ 
/ Written by Alex Hu
/ Last Updated: Dec. 11, 2008
**********************************************/

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
	
	// Variables used in Overlapping Allan Deviation calculations
	double Tau, Tau0, sum, sigma1, sigma2;
	int N, m;
	
	// Ensures there are at least the minimum number of points required to do calculations
	N = numPoints;
	if(N > 1) Tau0 = timeArray[1] - timeArray[0];
	else
	{
		cout << "Not Enough Points to Calculate Tau0" << endl;
		Tau0;
	}
	
	// Actual Overlapping Allan Deviation Calculation is done here
	// The Overlapping Allan Deviation is calculated as follows
	//  Sigma^2(Tau) = 1 / (2*(N-3*m+1)*Tau^2) * Sum(X[i+2*m]-2*X[i+m]+X[i], i=1, i=N-2*m)
	//  Where Tau is the averaging time, N is the total number of points, and Tau = m*Tau0
	//  Where Tau0 is the basic measurement interval
	
        for(m = 1; m <= (N-1)/2; m++)
	{
	        numGaps=0;
                Tau = m*Tau0;
		sigma1 = 0;
		sigma2 = 0;
		int y=0; //counts number of times a value is added to sigma
		
		for(int j=0; j < (N-3*m+1); j=j+1)
		{
			for(i = j; i <= (j+m-1); i++)
			{
				sum = 0;
				if((phaseArray[i+2*m]==0 ||  phaseArray[i+m]==0 || phaseArray[i]==0) 
				   && i!=0 && i!=(j+m-1))
				   numGaps++;
        	                else
                	            sum = phaseArray[i+2*m] - 2*phaseArray[i+m] + phaseArray[i];
				sigma1 += sum;
				
			}
			sigma2 += sigma1*sigma1;
			sigma1 = 0;
			y++;
		}
		
		sigma2 = sigma2 / (2.0*Tau*Tau*m*m*((double)N-(double)numGaps-3.0*m+1));
		sigma2 = sqrt(sigma2);
		
		fprintf(stdout, "%.1f %.4e \n", Tau, sigma2); // outputs results to the standard output
        }
	/*************
	*************/

	return(0);
}
