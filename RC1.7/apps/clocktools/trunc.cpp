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
