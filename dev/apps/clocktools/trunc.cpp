#pragma ident "$Id$"
/**********************************************
/ GPSTk: Clock Tools
/ trunc.cpp
/ 
/ Truncates a data set after time specified by
/ command line input
/ (reference)
/
/ Written by Timothy J.H. Craddock (tjhcraddock@gmail.com)
/ Last Updated: Dec. 11, 2008
**********************************************/

#include <iostream>
#include <cstdio>
#include <cstdlib>

using namespace std;

int main(int argc, char **argv)
{
	double data[2];
	double trunctime;

	// ensure that a truncate time has been specified
	if(argc != 2)
	{
		printf("Usage: trunc <time>\n");
	}
	else
	{
		// read truncate time from command line
		trunctime = atof(argv[1]);
		
		// read data in through standard input
		while(!feof(stdin))
		{
			cin >> data[0] >> data[1];
			if(feof(stdin)) break;
			
			// if data time greater than truncate time stop outputting data
			if(data[0] > trunctime) break;
			else printf("%.1f %.15f\n", data[0], data[1]);
		}
	}

	return(0);
}
