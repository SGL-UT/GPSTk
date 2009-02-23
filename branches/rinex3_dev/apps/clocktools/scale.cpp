/**********************************************
/ GPSTk: Clock Tools
/ scale.cpp
/ 
/ Scales kalman phase data for use with the Clock Tools.
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
	double data[2];
	
	// input data from the standard input
	while(!feof(stdin))
	{
		cin >> data[0] >> data[1];
		if(feof(stdin)) break;
		
		// scale and output data
		printf("%.1f %.15f\n", data[0], data[1]*0.000001);
	}

	return(0);
}
