#pragma ident "$Id$"
#ifndef _RESVEC_
#define _RESVEC_

#include "result.h"

class resvec
{
	public:
	 resvec();
	 resvec(resvec const & copy);
	 ~resvec();
	 void addResult(result res);
	 result getResult(int i);
	 int getLength();
	 resvec& resvec::operator=(const resvec& r);

	private:
	 void resize();
	 result *rvector;
	 int length;
	 int dlength;
};

#endif

