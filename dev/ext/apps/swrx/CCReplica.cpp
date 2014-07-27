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

#include "CCReplica.hpp"

#include "complex_math.h"

using namespace gpstk;
using namespace std;


//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void CCReplica::dump(std::ostream& s) const
{
   s << "# " << codeGenPtr->sv << " " << ObsID::tcDesc[codeGenPtr->code]
     << ":" << endl
     << setprecision(4)
     << "# -- tickSize: " << tickSize * 1e6 << " us"
     << ", codeChipLen: " << codeChipLen * 1e6 << " us"
     << ", chipsPerTick:" << chipsPerTick
     << endl << setprecision(7)
     << "# -- LO carrierFreq: " << carrierFreq * 1e-3 << " kHz"
     << ", cyclesPerTick: " << cyclesPerTick
     << endl
     << "# -- localTime: " << localTime * 1e6 << " us"
     << ", codePhase: " << codePhase << " chips"
     << endl
     << "# -- codePhaseOffset: " << codePhaseOffset << " chips"
     << ", codeFreqOffset: " << codeFreqOffset << " chips/tick"
     << endl
     << "# -- carrierPhase: " << carrierPhase
     << ", carrierFreqOffset: " << carrierFreqOffset << " cycles/tick"
     << endl;
}


void CCReplica::reset()
{
   codePhase = 0;
   codePhaseOffset = 0;
   codeFreqOffset = 0;
   carrierPhase = 0;
   carrierPhaseOffset = 0;
   carrierFreqOffset = 0;
   carrierAccum = 0;
   localTime = 0;
   codeGenPtr->reset();
}

complex<double> CCReplica::getCarrier()
{
   return sincos(2.0*gpstk::PI*carrierPhase);
}


void CCReplica::moveCodePhase(double chips) throw()
{
   codePhase += chips;
   codePhaseOffset += chips;
   wrapCode();
}


void CCReplica::moveCarrierPhase(double cycles) throw()
{
   carrierPhase += cycles;
   carrierPhaseOffset += cycles;
   wrapCarrier();
}


void CCReplica::setCodeFreqOffsetHz(double freq)
{
   // Yea, my code offset calculation is fscked
   codeFreqOffset = 0 * (freq * codeChipLen) * tickSize / codeChipLen;
}


double CCReplica::getCodeFreqOffsetHz() const
{
   return  codeFreqOffset / tickSize;
}


double CCReplica::getCodePhaseOffsetSec() const
{
   return codePhaseOffset * codeChipLen;
}


void CCReplica::setCarrierFreqOffsetHz(double freq)
{
   carrierFreqOffset = freq * tickSize;
}


double CCReplica::getCarrierFreqOffsetHz() const
{
   return carrierFreqOffset / tickSize;
}


void CCReplica::tick() throw()
{
   localTime += tickSize;

   double codePhaseDelta = chipsPerTick + codeFreqOffset;
   codePhase += codePhaseDelta;
   codePhaseOffset += codeFreqOffset;
   wrapCode();
   
   double carrierUpdate = cyclesPerTick + carrierFreqOffset;
   carrierPhase += carrierUpdate;
   carrierPhaseOffset += carrierFreqOffset;
   wrapCarrier();
}


void CCReplica::wrapCode()
{
   if (codePhase<1)
      return;
   if (codePhase<2)
   {
      codePhase -= 1;
      ++(*codeGenPtr);
   }
   else
   {
      long dc = static_cast<long>(codePhase);
      codeGenPtr->setIndex(codeGenPtr->getIndex() + dc);
      codePhase -= static_cast<CodeIndex>(codePhase);
   }
}


void CCReplica::wrapCarrier()
{
   if (codePhase<1)
      return;
   else if (codePhase<2)
   {
      carrierPhase--;
      carrierAccum++;
   }
   else
   {
      long dc = static_cast<long>(carrierPhase);
      carrierPhase -= dc;
      carrierAccum += dc;
   }
}
