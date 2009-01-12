#ifndef _DATAPOINT_
#define _DATAPOINT_

class dataPoint
{
	public:
	 dataPoint();
	 dataPoint(double x, double y, double z = 0);
	 void setX(double x);
	 void setY(double y);
	 void setZ(double z);
	 double getX();
	 double getY();
	 double getZ();
	 
	private:
	 double X, Y, Z;
	 bool nullPoint;
};

#endif
