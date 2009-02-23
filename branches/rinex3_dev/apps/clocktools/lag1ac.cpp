/*******************************************************************************/
/* This program reads in clock phase data provided by any of the phase parsers */
/*  and calculates the lag 1 autocorrelation function, outputting the the alpha*/ 
/*  value to the standard output in a format that can be used by allanplot     */
/*  to view the data.                                                        */
/*                                                                             */
/* Written by Alex Hu                                                          */
/* Last Updated July 18, 2008                                                  */
/*******************************************************************************/

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
	double delta =.25, p, Tau0;
	int N=i-1, d=0;
	
	// Ensures there are at least the minimum number of points required to do calculations
	if(N > 1) Tau0 = timeArray[1] - timeArray[0];
	else
	{
		cout << "Not Enough Points to Calculate Tau0" << endl;
		Tau0;
	}
	
	//Converts phase data to frequency data
	for(i=0; i<N-1; i++)
	{
		phaseArray[i] = (phaseArray[i+1]-phaseArray[i])/Tau0;
	}
	N--;
	
	//Calculates delta
	while(delta >= .25)
	{
		
		//Differences the data while delta > .25
		if(d>0)
		{
			for(i=0; i<N-1; i++)
			{
				phaseArray[i] = phaseArray[i+1]-phaseArray[i];
			}
			N--;
		}
		//Counts the number of times data has been differenced + 1
		d++;
		
		//Calculates Average
		double avg = 0.0;
		for(i = 0; i < N; i++) 
		{
			avg += phaseArray[i];
		}
		avg /= N;
		
		//Calculates delta
		double top=0, bottom=0;
		for(i=0; i<N-1; i++)
		{
			top += (phaseArray[i]-avg)*(phaseArray[i+1]-avg);
			bottom += (phaseArray[i]-avg)*(phaseArray[i]-avg);
		}
	
		bottom += (phaseArray[N-1]-avg)*(phaseArray[N-1]-avg);
		delta = top/bottom;
		delta = delta/(1+delta);
	}
	
	//Calculates the approximation for alpha
	p = -2*(delta+d-1);
	
	fprintf(stdout, "p= %f, d= %i, delta %f \n", p, d-1, delta);
	return(0);
}
