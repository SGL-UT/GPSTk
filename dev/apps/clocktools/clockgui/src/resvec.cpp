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

#include "resvec.h"

resvec::resvec()
{
	rvector = 0;
	length = 0;
	dlength = 0;
}

resvec::resvec(resvec const & copy)
{
	rvector = new result[copy.length];
	length = copy.length;
	dlength = copy.dlength;

	int i;
	for(i = 0; i < dlength; i++) rvector[i] = copy.rvector[i];
}

resvec::~resvec()
{
	delete [] rvector;
	rvector = 0;
}

void resvec::resize()
{
	result *temp;
	int i;

	temp = new result[dlength];
	for(i = 0; i < dlength; i++) temp[i] = rvector[i];

	delete [] rvector;
	rvector = new result[2*(dlength+1)];

	for(i = 0; i < dlength; i++) rvector[i] = temp[i];

	delete [] temp;
	temp = 0;

	length = 2*(dlength+1);
}

void resvec::addResult(result res)
{
	if(dlength >= length) resize();
	rvector[dlength] = res;
	dlength += 1;
}

result resvec::getResult(int i)
{
	if(i < dlength) return(rvector[i]);
	else
	{
		result nullResult;
		nullResult.addResult(double(NULL), double(NULL));
		return(nullResult);
	}
}

int resvec::getLength()
{
	return(dlength);
}

resvec& resvec::operator=(const resvec& r)
{
	if(this != &r)
	{
		rvector = new result[r.length];
		dlength = r.dlength;
		length = r.length;

		int i;
		for(i = 0; i < dlength; i++) rvector[i] = r.rvector[i];
	}

	return *this;
}
