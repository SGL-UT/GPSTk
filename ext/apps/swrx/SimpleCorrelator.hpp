//============================================================================
//
//  This file is part of GPSTk, the GPS Toolkit.
//
//  The GPSTk is free software; you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published
//  by the Free Software Foundation; either version 3.0 of the License, or
//  any later version.
//
//  The GPSTk is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with GPSTk; if not, write to the Free Software Foundation,
//  Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110, USA
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

#ifndef SIMPLECORRELATOR_HPP
#define SIMPLECORRELATOR_HPP

#include <complex>
#include <queue>
#include <iostream>

//-----------------------------------------------------------------------------
// A correlator with a built in delay line to offset the incoming code.
//-----------------------------------------------------------------------------
template <class C>
class SimpleCorrelator
{
public:
   
   typedef std::complex<C> Ctype;

   /// param d this is the number of samples to delay the code by
   SimpleCorrelator(unsigned d=0) : delay(d+1), sum(0,0)
   {}
   
   inline void process(std::complex<C> in, Ctype code) throw()
   {
      shiftReg.push(code);
      sum += in * shiftReg.front();
      while (shiftReg.size() > delay)
         shiftReg.pop();
   }
   
   inline void dump() throw() {sum=Ctype(0,0);}

   inline Ctype operator()() const throw() {return sum;}

   void setDelay(unsigned d) throw() {delay=d+1;}
   unsigned getDelay() const throw() {return delay-1;}

private:
   unsigned delay;
   std::queue< Ctype > shiftReg;
   Ctype sum;
};

#endif
