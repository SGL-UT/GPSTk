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

#include "datapoint.h"

dataPoint::dataPoint()
{
	X = 0;
	Y = 0;
	Z = 0;
	nullPoint = true;
}

dataPoint::dataPoint(double x, double y, double z)
{
	X = x;
	Y = y;
	Z = z;
	nullPoint = false;
}

void dataPoint::setX(double x)
{
	X = x;
	nullPoint = false;
}

void dataPoint::setY(double y)
{
	Y = y;
	nullPoint = false;
}

void dataPoint::setZ(double z)
{
	Z = z;
	nullPoint = false;
}

double dataPoint::getX()
{
	if(!nullPoint) return(X);
	else return(double(0));
}

double dataPoint::getY()
{
	if(!nullPoint) return(Y);
	else return(double(0));
}

double dataPoint::getZ()
{
	if(!nullPoint) return(Z);
	else return(double(0));
}
