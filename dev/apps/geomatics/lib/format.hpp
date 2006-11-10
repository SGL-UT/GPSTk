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
//  Copyright 2004, The University of Texas at Austin
//
//============================================================================

//============================================================================
//
//This software developed by Applied Research Laboratories at the University of
//Texas at Austin, under contract to an agency or agencies within the U.S. 
//Department of Defense. The U.S. Government retains all rights to use,
//duplicate, distribute, disclose, or release this software. 
//
//Pursuant to DoD Directive 523024 
//
// DISTRIBUTION STATEMENT A: This software has been approved for public 
//                           release, distribution is unlimited.
//
//=============================================================================

/**
 * @file format.hpp
 * Simple class to encapsulate output format
 */

#ifndef CLASS_FORMAT_HELPER_INCLUDE
#define CLASS_FORMAT_HELPER_INCLUDE

#include <ostream>

//------------------------------------------------------------------------------------
class format {
   int form;   // 0=general,1=float,2=scientific
   int wide;
   int prec;
public:
   explicit format(int w, int p, int f=1) : wide(w),prec(p),form(f) {}
   format& scientific() { form=2; return *this; }
   format& sci() { form=2; return *this; }
   format& fixed() { form=1; return *this; }
   format& fix() { form=1; return *this; }
   format& general() { form=0; return *this; }
   format& gen() { form=0; return *this; }
   format& width(int w) { wide=w; return *this; }
   format& w(int w) { wide=w; return *this; }
   format& precision(int p) { prec=p; return *this; }
   format& p(int p) { prec=p; return *this; }
   format& wp(int w, int p) { prec=p; wide=w; return *this; }

   friend std::ostream& operator<<(std::ostream& os, const format& f);
}; // end class format

//------------------------------------------------------------------------------------
#endif
// nothing below this..
