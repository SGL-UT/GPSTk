/**********************************************
/ GPSTk: Clock Tools
/ pff.cpp
/ 
/ Converts phase data to fractional frequency data
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
	unsigned long int i = 0;
	
	double ff;
	
	// inputs phase data from the standard input
	while(!feof(stdin))
	{
		if(i != 0)
		{
			dataB[0] = dataA[0];
			dataB[1] = dataA[1];
		}
		
		cin >> dataA[0] >> dataA[1];
		if(feof(stdin)) break;
		
		// esnure at least 2 data points are known
		if(i != 0)
		{
			// convert phase to fractional frequency, output data
			ff = (dataA[1] - dataB[1])/(dataA[0]-dataB[0]);
			printf("%.1f %.15f\n", dataB[0], ff);
		}
		
		i++;
	}
	
	return(0);
}
