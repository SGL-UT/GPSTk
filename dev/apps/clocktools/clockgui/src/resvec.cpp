#pragma ident "$Id$"
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
