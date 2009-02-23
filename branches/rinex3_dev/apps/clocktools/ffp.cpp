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


#include <iostream>

#include <stdio.h>

using namespace std;

int main()
{
	double dataA[2];
	double dataB[2];
	unsigned long int i;
	
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

