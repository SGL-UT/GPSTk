#pragma ident "$Id$"
/**********************************************
/ GPSTk: Clock Tools
/ rmoutlier.cpp
/ 
/ Removes outliers within phase data
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
//============================================================================

#include <iostream>
#include <vector>

#include <stdio.h>
#include <math.h>

using namespace std;

int main()
{
	double data[2];
	
	vector <double> timetags;
	vector <double> phase;
	vector <double> params;
	
	// inputs time tag & phase data from the standard input
	while(!feof(stdin))
	{
		cin >> data[0] >> data[1];
		if(feof(stdin)) break;
		timetags.push_back(data[0]);
		phase.push_back(data[1]);
	}
		
	double dataA[2];
	double dataB[2];
	unsigned long int i = 0;
	double ff;
	
	// convert phase data to fractional frequency data
	for(i = 0; i < phase.size(); i++)
	{
		if(i != 0)
		{
			dataB[0] = dataA[0];
			dataB[1] = dataA[1];
		}
		
		dataA[0] = timetags[i];
		dataA[1] = phase[i];
		
		if(i != 0)
		{
			params.push_back((dataA[1] - dataB[1])/(dataA[0]-dataB[0]));
		}
	}
	
	unsigned long int outliers, totaliers, gaps;
	double m, s;
	
	// remove outliers
	totaliers = 0;
	do
	{
		outliers = 0;
		gaps = 0;
		
		// calculate mean
		m = 0.0;
		for(i = 0; i < params.size(); i++)
		{
			if(i != 0 && i != params.size()-1 && params[i] == 0.0)
			{
				gaps++;
			}
			else
			{
				m += params[i];
			}
		}
		m = m / double(params.size()-gaps);
		
		// calculate standard deviation
		s = 0.0;
		for(i = 0; i < params.size(); i++)
		{
			if(i != 0 && i != params.size()-1 && params[i] == 0.0)
			{
			}
			else
			{
				s += (params[i] - m)*(params[i] - m);
			}
		}
		s = sqrt(s/(double(params.size()-1)-gaps));
		
		// remove outlying points
		for(i = 0; i < params.size(); i++)
		{
			if(i != 0 && i != params.size()-1 && params[i] == 0.0)
			{
			}
			else
			{
				if((params[i]-m)*(params[i]-m) > 15.6*s*s) // outlying data larger that n^2*s*s is removed.
				{
					params[i] = 0.0;
					outliers++;
					totaliers++;
				}
			}
		}
		
	} while(outliers > 0);
	
	// interpolate gaps
	if(params[0] == 0.0) params[0] = params[1];
	for(i = 1; i < params.size()-1; i++)
	{
		if(params[i] == 0.0) params[i] = (params[i-1]+params[i+1])/2.0;
	}
	if(params[params.size()-1] == 0.0) params[params.size()-1] = params[params.size()-2];
	
	// convert fractional frequency data back to phase data and output
	double x0 = 0.0;
	for(i = 0; i < params.size(); i++)
	{
	
		if(i != 0)
		{
			dataB[0] = dataA[0];
			dataB[1] = dataA[1];
		}
		
		dataA[0] = timetags[i];
		dataA[1] = params[i];
		
		if(i != 0)
		{
			x0 = dataB[1]*(dataA[0]-dataB[0]) + x0;
			printf("%.1f %.15f\n", dataA[0], x0);
		}
		else
		{
			printf("%.1f %.15f\n", timetags[0], x0);
		}
		
	}
	
	return(0);
}
