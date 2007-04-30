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

#ifndef COMPLEXMATH_H
#define COMPLEXMATH_H

#include <complex>

static inline std::complex<double> sincos(double radians) 
{
   std::complex<double> ret;
   sincos(radians,&(ret.imag()),&(ret.real()));
   return ret;
}

static inline std::complex<int> quantize(const std::complex<double> x)
{
   return std::complex<int>(
      static_cast<int>(::round(x.real())),
      static_cast<int>(::round(x.imag()))
      );
}

const std::complex<double> zero(0,0);
const std::complex<double> plusOne(1,0);
const std::complex<double> minusOne(-1,0);


//-----------------------------------------------------------------------------
// An function object that implements a second order filter for the PLL
// Specifically the filter in figure 8.2 of Tsui's book
//-----------------------------------------------------------------------------
template <class C>
class LoopFilter
{
public:
   LoopFilter() : c1(1), c2(1), z1(0) {};

   std::complex<C> operator()(std::complex<C> in) throw()
   {
      std::complex <C> out,z;
      z = in*c2+z1;
      out = in * c1 + z;
      z1 = z;
      return out;
   };

   C c1, c2;
   std::complex<C> z1;
};

#endif
