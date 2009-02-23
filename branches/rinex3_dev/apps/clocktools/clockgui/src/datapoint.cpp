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
