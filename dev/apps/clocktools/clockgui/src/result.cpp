#pragma ident "$Id$"

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

#include "result.h"

result::result()
{
	points = 0;
	dlength = 0;
	length = 0;
	dem = _2D;
	strcpy(title, "unnamed");
}

result::result(result const &copy)
{
	points = new dataPoint [copy.length];
	dlength = copy.dlength;
	length = copy.length;
	dem = copy.dem;
	strcpy(title, copy.title);

	int i;
	for(i = 0; i < dlength; i++) points[i] = copy.points[i];
}

result::~result()
{
	delete [] points;
	points = 0;
}

void result::resize()
{
	dataPoint *temp;
	int i;

	temp = new dataPoint[dlength];
	for(i = 0; i < dlength; i++) temp[i] = points[i];

	delete [] points;
	points = new dataPoint[2*(dlength+1)];

	for( i = 0; i < dlength; i++) points[i] = temp[i];

	delete [] temp;
	temp = 0;

	length = 2*(dlength+1);
}

void result::addResult(double x, double y, double z)
{
	if(dlength >= length) resize();
	points[dlength] = dataPoint(x, y, z);
	dlength += 1;
}

void result::setDem(int d)
{
	dem = d;
}

void result::setTitle(char *str)
{
	strcpy(title, str);
}

dataPoint result::getResult(int i)
{
	if(i < dlength) return(points[i]);
	else return(dataPoint(double(NULL), double(NULL)));
}

char *result::getTitle()
{
	return(title);
}

int result::getLength()
{
	return(dlength);
}

int result::getDem()
{
	return(dem);
}

result& result::operator=(const result& r)
{
	if(this != &r)
	{
		points = new dataPoint[r.length];
		dlength = r.dlength;
		length = r.length;
		dem = r.dem;
		strcpy(title, r.title);

		int i;
		for(i = 0; i < dlength; i++) points[i] = r.points[i];
	}

	return *this;
}
