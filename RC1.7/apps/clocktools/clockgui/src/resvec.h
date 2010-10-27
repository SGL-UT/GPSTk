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
//  Copyright 2009, The University of Texas at Austin
//
//============================================================================


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

