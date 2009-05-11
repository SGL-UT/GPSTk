#pragma ident "$Id$"
#ifndef _RESULT_
#define _RESULT_

#define _2D	2
#define _3D	3

#include <string.h>
#include "datapoint.h"

class result
{
	public:
	 result();
	 result(result const &copy);
	 ~result();
	 void addResult(double x, double y, double z = 0);
	 void setTitle(char *str);
	 void setDem(int d);
	 dataPoint getResult(int i);
	 char *getTitle();
	 int getLength();
	 int getDem();
	 result& result::operator=(const result& r);

	private:
	 void resize();
	 dataPoint *points;
	 int dlength;
	 int length;
	 int dem;
	 char title[256];
};

#endif
