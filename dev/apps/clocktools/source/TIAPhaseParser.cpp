/*************************************************************************/
/* This program reads in clock phase data files provided by the TIA      */
/*  and outputs the data in the format required by oallandev and         */
/*  ohadamarddev                                                         */
/*                                                                       */
/* Written by Tim Craddock                                               */
/* Last Updated March 18, 2008                                           */
/*************************************************************************/

#include <iostream>
#include <fstream>

#include <stdio.h>
#include <string.h>

using namespace std;

int main(int argc, char *argv[])
{
	char filename[256], dummy[512];
	long double phase;
	unsigned long int i, j;
	
	if(argc > 1)
	{
		strcpy(filename, argv[1]);
	}
	else
	{
		fprintf(stdout, "usage: TIAPhaseParser <filename>\n");
		fprintf(stdout, " Formats phase data from the 5110A Timing Interval Analyzer for use with Ghetto32\n");
		return(0);
	}
	
	ifstream data(filename);
	
	j = 0;
	while(!data.eof())
	{
		data.getline(dummy, 512);
		if(data.eof()) break;
		
		for(i = 0; i < strlen(dummy); i++)
		{
			if((dummy[i] != ' ') && (dummy[i] != '\n') && (dummy[i] != '\r'))
			{
				phase = atof(dummy);
				
				fprintf(stdout, "%.1Lf %.25Lf\n", 1.0*(long double)j, phase);
				
				j++;
			}
		}
	}
	
	data.close();
	return(0);
}
